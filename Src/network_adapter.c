/* Includes ------------------------------------------------------------------*/
#include <unabto/unabto_external_environment.h>
#include "lwip/api.h"
#include "lwip/udp.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct RecvBuffer
{
  struct pbuf *buf;
  u32_t recvAddr;
  u16_t recvPort;
} RecvBuffer;

typedef struct Socket
{
  struct udp_pcb *pcb;
  RecvBuffer recvBuffer;
} Socket;

/* Private define ------------------------------------------------------------*/
/* One for local connections and one for remote connections. */
#define MAX_SOCKETS  2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct Socket sockets[MAX_SOCKETS];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void udp_receive_callback(void*           arg,
                                 struct udp_pcb* pcb,
                                 struct pbuf*    buf,
                                 const ip_addr_t* addr,
                                 u16_t           port)
{
  NABTO_NOT_USED(pcb);
  static int dropped = 0;

  RecvBuffer *rb = &((Socket*) arg)->recvBuffer;
  if (rb->buf == NULL) {
    rb->buf = buf;
    rb->recvAddr = addr->addr;
    rb->recvPort = port;
  } else {
    pbuf_free(buf); // drop
    dropped++;
  }
}

bool nabto_init_socket(uint32_t        localAddr,
                       uint16_t*       localPort,
                       nabto_socket_t* socket)
{
  NABTO_LOG_TRACE(("nabto_init_socket start socket=%u localPort=%u", (uint8_t) *socket, *localPort));

  if (socket == NULL)
    return false;

  int i;
  for (i = 0; i < MAX_SOCKETS && sockets[i].pcb; i++);
  if (i == MAX_SOCKETS)
    return false;

  sockets[i].pcb = udp_new();
  sockets[i].pcb->so_options |= SOF_BROADCAST;

  udp_recv(sockets[i].pcb, udp_receive_callback, &sockets[i]);

  ip_addr_t ipAddr;
  ipAddr.addr = localAddr;

  if (udp_bind(sockets[i].pcb, (localAddr == 0) ? IP_ADDR_ANY : &ipAddr, *localPort) != ERR_OK)
    return false;

  *socket = i;
  return true;
}

void nabto_close_socket(nabto_socket_t* socket)
{
  NABTO_LOG_TRACE(("nabto_close_socket start socket=%u", (uint8_t) *socket));
  if (socket == NULL)
    return;

  if (0 > *socket || *socket >= MAX_SOCKETS)
    return;

  if (sockets[*socket].pcb != NULL) {
    udp_remove(sockets[*socket].pcb);
    sockets[*socket].pcb = NULL;
  }
}

ssize_t nabto_read(nabto_socket_t socket,
                   uint8_t*       buf,
                   size_t         len,
                   uint32_t*      addr,
                   uint16_t*      port)
{
  ssize_t res = (ssize_t) len;

  if (0 > socket || socket >= MAX_SOCKETS)
    return -1;

  RecvBuffer *rb = &sockets[socket].recvBuffer;
  if (rb->buf != NULL) {
    struct pbuf *p, *pHead = rb->buf;
    p = pHead;
    while (p && p->len <= len) {
      memcpy(buf, p->payload, p->len);
      len -= p->len;
      buf += p->len;
      p = p->next;
    }
    pbuf_free(pHead);
    *addr = ntohl(rb->recvAddr);
    *port = rb->recvPort;
    rb->buf = NULL;
  }
  return res - (ssize_t) len;
}

ssize_t nabto_write(nabto_socket_t socket,
                    const uint8_t* buf,
                    size_t         len,
                    uint32_t       addr,
                    uint16_t       port)
{
  ssize_t res = -1;
  static struct pbuf *p = NULL;
  ip_addr_t ipAddr;

  if (0 > socket || socket >= MAX_SOCKETS || buf == NULL || len == 0)
    return -1;

  p = pbuf_alloc(PBUF_TRANSPORT, (unsigned short) len, PBUF_RAM);
  if (p == NULL)
    return -1;

  pbuf_take(p, buf, (unsigned short) len);
  ipAddr.addr = htonl(addr);
  if (ERR_OK == udp_sendto(sockets[socket].pcb, p, &ipAddr, port))
    res = (ssize_t) len;
  pbuf_free(p);
  return res;
}
