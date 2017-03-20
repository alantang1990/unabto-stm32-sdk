#ifndef _FP_ACL_FLASH_H_
#define _FP_ACL_FLASH_H_

#include <modules/fingerprint_acl/fp_acl.h>
#include <modules/fingerprint_acl/fp_acl_memory.h>

#include <stdio.h>

/**
 * The fp acl flash saves the fingerprint config in the following format
 * 
 * Version 2 of the file format is as follows:
 * 
 * uint32_t version
 * settings { uint32_t systemPermissions, uint32_t defaultUserPermissions, uint32_t firstUserPermissions}
 * number of users: uint32_t,
 * [users] user = {fingerprint: uint8_t[16], name: char[64], permissions: uint32_t}
 *
 * saving rewrites the full flash segment.
 * 
 * init loads the flash segment the first time it
 * 
 * in version 1 the name is always 64 bytes long
 */

#define FP_ACL_FILE_USERNAME_LENGTH 64

#define FP_ACL_FILE_VERSION 2

#if FP_ACL_USERNAME_MAX_LENGTH != FP_ACL_FILE_USERNAME_LENGTH
#error incompatible user name length with current acl file format
#endif

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) /* Base address of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) /* Base address of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) /* Base address of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) /* Base address of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base address of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) /* Base address of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) /* Base address of Sector 7, 256 Kbytes */

fp_acl_db_status fp_acl_flash_save(struct fp_mem_state* acl);

fp_acl_db_status fp_acl_flash_load(struct fp_mem_state* acl);

fp_acl_db_status fp_acl_flash_erase();

/**
 * Inititalize the flash persisting backend.
 */
fp_acl_db_status fp_acl_flash_init(uint32_t flash_start_addr, uint32_t flash_end_addr, struct fp_mem_persistence* p);


#endif
