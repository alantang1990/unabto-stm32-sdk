/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <ctype.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "string.h"
#include <modules/util/read_hex.h>
#include "unabto_main.h"
#include "cmsis_os.h"
#include "unabto_main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NABTO_THREAD_PRIO    ( tskIDLE_PRIORITY + 4 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t display_state = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  uNabto server thread
  * @param arg: None
  * @retval None
  */
static void unabto_thread()
{
  const char* device_id = "<DEVICE ID>";
  const char* pre_shared_key = "<KEY>";

  nabto_main_setup* nms = unabto_init_context();
  nms->id = strdup(device_id);

  nms->secureAttach = true;
  nms->secureData = true;
  nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

  if (!unabto_read_psk_from_hex(pre_shared_key, nms->presharedKey, 16)) {
    NABTO_LOG_ERROR(("Invalid cryptographic key specified", pre_shared_key));
    return;
  }

  if (!unabto_init()) {
    NABTO_LOG_FATAL(("Failed at nabto_main_init"));
  }

  for (;;) {
    osDelay(10);
    unabto_tick();
  }
}

application_event_result application_event(application_request* appreq,
                                           unabto_query_request* r_b,
                                           unabto_query_response* w_b)
{
  switch (appreq->queryId) {
  case 1: {
    // <query name="light_write.json" description="Turn light on and off" id="1">
    // <request>
    // <parameter name="light_id" type="uint8"/>
    // <parameter name="light_on" type="uint8"/>
    // </request>
    // <response>
    // <parameter name="light_state" type="uint8"/>
    // </response>
    // </query>

    uint8_t light_id;
    uint8_t light_on;

    // Read parameters in request
    if (!unabto_query_read_uint8(r_b, &light_id))
      return AER_REQ_TOO_SMALL;
    if (!unabto_query_read_uint8(r_b, &light_on))
      return AER_REQ_TOO_SMALL;

    // Set display according to request
    display_state = light_on;
    if (display_state)
      BSP_LCD_DisplayOn();
    else
      BSP_LCD_DisplayOff();

    // Write back display state
    if (!unabto_query_write_uint8(w_b, display_state))
      return AER_REQ_RSP_TOO_LARGE;

    return AER_REQ_RESPONSE_READY;
  }
  case 2: {
    // <query name="light_read.json" description="Read light status" id="2">
    // <request>
    // <parameter name="light_id" type="uint8"/>
    // </request>
    // <response>
    // <parameter name="light_state" type="uint8"/>
    // </response>
    // </query>

    uint8_t light_id;

    // Read parameters in request
    if (!unabto_query_read_uint8(r_b, &light_id))
      return AER_REQ_TOO_SMALL;

    // Write back led state
    if (!unabto_query_write_uint8(w_b, display_state))
      return AER_REQ_RSP_TOO_LARGE;

    return AER_REQ_RESPONSE_READY;
  }
  }
  return AER_REQ_INV_QUERY_ID;
}

/**
  * @brief  Start the uNabto server
  * @param  none
  * @retval None
  */
void unabto_start()
{
  sys_thread_new("uNabto", unabto_thread, NULL, DEFAULT_THREAD_STACKSIZE, NABTO_THREAD_PRIO);
}

