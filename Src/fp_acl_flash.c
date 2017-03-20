#include "fp_acl_flash.h"

#include <unabto/unabto_util.h>
#include "stm32746g_discovery.h"

static uint32_t flash_start_addr = 0;
static uint32_t flash_end_addr = 0;

volatile uint8_t byte = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

/**
  * @brief  Gets the sector of a given address
  * @param  Address  given address
  * @retval The sector of a given address
  */
static uint32_t get_flash_sector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7) */
  {
    sector = FLASH_SECTOR_7;
  }
  return sector;
}

fp_acl_db_status fp_acl_flash_load(struct fp_mem_state* acl)
{
    uint8_t buffer[128];
    uint32_t flash_addr = flash_start_addr;

    // load version
    for(size_t i = 0; i < 4; ++i)
	{
    	byte = *(volatile uint8_t *)flash_addr++;
		buffer[i] = byte;
	}

    uint32_t version;
    READ_U32(version, buffer);
    if (version != FP_ACL_FILE_VERSION) {
        // there is no saved acl database, consider it as a completely normal bootstrap scenario
        return FP_ACL_DB_OK;
    }

    // load system settings
    for(size_t i = 0; i < 16; ++i)
	{
		byte = *(volatile uint8_t *)flash_addr++;
		buffer[i] = byte;
	}

    uint8_t* ptr = buffer;

    uint32_t numUsers;
    
    memset(acl, 0, sizeof(struct fp_mem_state));
    READ_FORWARD_U32(acl->settings.systemPermissions, ptr);
    READ_FORWARD_U32(acl->settings.defaultUserPermissions, ptr);
    READ_FORWARD_U32(acl->settings.firstUserPermissions, ptr);
    READ_FORWARD_U32(numUsers, ptr);

    uint32_t i;
    enum {
        USER_RECORD_SIZE = FP_ACL_FP_LENGTH + FP_ACL_FILE_USERNAME_LENGTH + 4
    };
    for(i = 0; i < numUsers && i < FP_MEM_ACL_ENTRIES; i++) {
        for(size_t j = 0; j < USER_RECORD_SIZE; ++j)
		{
			byte = *(volatile uint8_t *)flash_addr++;
			buffer[j] = byte;
		}
        memcpy(acl->users[i].fp, buffer, FP_ACL_FP_LENGTH);
        memcpy(acl->users[i].name, buffer + FP_ACL_FP_LENGTH, FP_ACL_FILE_USERNAME_LENGTH); // guaranteed by compile time check
        READ_U32(acl->users[i].permissions, buffer + FP_ACL_FP_LENGTH + FP_ACL_FILE_USERNAME_LENGTH);
    }

    return FP_ACL_DB_OK;
}

fp_acl_db_status fp_acl_flash_save(struct fp_mem_state* acl)
{
	uint32_t flash_addr = flash_start_addr;
	uint8_t buffer[128];
	uint8_t* ptr = buffer;
	uint32_t users = 0;
	int i;
	for (i = 0; i < FP_MEM_ACL_ENTRIES; i++) {
		struct fp_acl_user* it = &acl->users[i];
		if (!fp_mem_is_slot_free(it)) {
			users++;
		}
	}

    HAL_FLASH_Unlock();

    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = get_flash_sector(flash_start_addr);
    EraseInitStruct.NbSectors     = get_flash_sector(flash_end_addr) - EraseInitStruct.Sector + 1;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
             you have to make sure that these data are rewritten before they are accessed during code
             execution. If this cannot be done safely, it is recommended to flush the caches by setting the
             DCRST and ICRST bits in the FLASH_CR register. */
    uint32_t SECTORError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
    	 return FP_ACL_DB_SAVE_FAILED;
    }

    WRITE_FORWARD_U32(ptr, FP_ACL_FILE_VERSION);
	WRITE_FORWARD_U32(ptr, acl->settings.systemPermissions);
	WRITE_FORWARD_U32(ptr, acl->settings.defaultUserPermissions);
	WRITE_FORWARD_U32(ptr, acl->settings.firstUserPermissions);
	WRITE_FORWARD_U32(ptr, users);

	for(size_t i = 0; i < 20; ++i)
	{
		byte = buffer[i];
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flash_addr++, byte) != HAL_OK)
			return FP_ACL_DB_SAVE_FAILED;
	}

	// write user records

	for (i = 0; i < users; i++) {
		struct fp_acl_user* it = &acl->users[i];
		if (!fp_mem_is_slot_free(it)) {
			memcpy(buffer, it->fp, 16);
			memcpy(buffer+16, it->name, 64);
			WRITE_U32(buffer + 16 + 64, it->permissions);
			for(size_t i = 0; i < 84; ++i)
			{
				byte = buffer[i];
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, flash_addr++, byte) != HAL_OK)
					return FP_ACL_DB_SAVE_FAILED;
			}
		}
	}

	HAL_FLASH_Lock();

	return FP_ACL_DB_OK;
}

fp_acl_db_status fp_acl_flash_init(uint32_t start_addr, uint32_t end_addr, struct fp_mem_persistence* p)
{
    p->load = &fp_acl_flash_load;
    p->save = &fp_acl_flash_save;

    flash_start_addr = start_addr;
    flash_end_addr = end_addr;

    return FP_ACL_DB_OK;
}
