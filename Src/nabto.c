/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <ctype.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "string.h"
#include "nabto.h"
#include "cmsis_os.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NABTO_THREAD_PRIO    ( tskIDLE_PRIORITY + 4 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t display_state = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int hctoi(const unsigned char h)
{
  if (isdigit(h))
    return h - '0';
  else
    return toupper(h) - 'A' + 10;
}

/**
  * @brief  nabto server thread
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
static void nabto_thread(void *argument)
{
  const char* nabtoId = "...";
  const char* presharedKey = "...";

  struct netif *netif = (struct netif *) argument;

  // Initialize Nabto
  nabto_main_setup* nms = unabto_init_context();
  nms->ipAddress = netif->ip_addr.addr;
  nms->id = nabtoId;
  nms->secureAttach = 1;
  nms->secureData = 1;
  nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

  const char *p;
  unsigned char *up;
  for (p = presharedKey, up = nms->presharedKey; *p; p += 2, ++up)
    *up = hctoi(p[0]) * 16 + hctoi(p[1]); // convert hex string to byte array

  unabto_init();

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
  * @brief  Initialize the UDP server (start its thread)
  * @param  none
  * @retval None
  */
void nabto_init(struct netif *netif)
{
  sys_thread_new("Nabto", nabto_thread, netif, DEFAULT_THREAD_STACKSIZE, NABTO_THREAD_PRIO);
}

