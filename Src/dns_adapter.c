/* Includes ------------------------------------------------------------------*/
#include <unabto/unabto_external_environment.h>
#include "lwip/api.h"
#include "lwip/dns.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t resolved_v4addr = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void dns_callback(const char *name, struct ip_addr *ipAddr, void *arg)
{
  NABTO_NOT_USED(arg);

  if (ipAddr && ipAddr->addr)
    resolved_v4addr = ntohl(ipAddr->addr);
}

void nabto_dns_resolve(const char* id)
{
  struct ip_addr resolvedIp;
  switch (dns_gethostbyname(id, &resolvedIp, dns_callback, NULL)) {
  case ERR_OK:
    resolved_v4addr = ntohl(resolvedIp.addr);
    break;
  case ERR_INPROGRESS:
    resolved_v4addr = 0;
    break;
  default:
    NABTO_LOG_ERROR(("DNS call failed"));
  }
}

nabto_dns_status_t nabto_dns_is_resolved(const char *id, uint32_t* v4addr)
{
  if (resolved_v4addr) {
    *v4addr = resolved_v4addr;
    NABTO_LOG_TRACE(("DNS resolved: %03d.%03d.%03d.%03d", MAKE_IP_PRINTABLE(*v4addr)));
    return NABTO_DNS_OK;
  } else {
    return NABTO_DNS_ERROR;
  }
}
