/*
 * coap_net_NoOS.c
 *
 *  Created on: Aug 1, 2013
 *      Author: poppe
 */

/* Standard Headers */

/* IP Headers */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"


/* Static variables */
static struct udp_pcb *coap_pcb;

#include "coap.h"
static void coap_close_conn(struct udp_pcb *pcb);
static coap_err_t coapd_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port);

static void coap_close_conn(struct udp_pcb *pcb)
{

}

static void coapd_err(void *arg, err_t err)
{

}

static coap_err_t coapd_sent(void *arg, struct udp_pcb *pcb, u16_t len)
{

}
static coap_err_t coapd_poll(void *arg, struct ucp_pcb *pcb)
{

}
static coap_err_t coapd_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
{
	LWIP_DEBUGF(COAP_DEBUG, ("----->Incoming packet size: %u\n", p->len));
	coap_err_t err;

	coap_pkt_t *pkt = (coap_pkt_t *)coap_malloc(sizeof(coap_pkt_t));
	memset(pkt, '\0', sizeof(coap_pkt_t));

	pkt->ip_addr = addr;
	pkt->port = port;

	err = coap_parsePacket(pkt, p->payload, p->len);
	if(err)
	{
		LWIP_DEBUGF(COAP_DEBUG, ("Coap err: %i", err));
		goto memerr;
	}


	err = coap_resourceDiscovery(pkt);

	memerr:
	pbuf_free(p);
	coap_free(pkt);

	return coap_OK;
}

coap_err_t coapd_sendto(uint8_t *buf, uint32_t len, ip_addr_t *addr, u16_t port)
{
	err_t err;
	struct pbuf *p;

	p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

	memcpy(p->payload, buf, len);

	p->tot_len = len;

	err = udp_sendto(coap_pcb, p, addr, port);

	pbuf_free(p);

}


void coapd_init(struct ip_addr *local_addr, int portNum)
{
	  /* initialize default DNS server address */


	  LWIP_DEBUGF(COAP_DEBUG, ("coap_init: initializing\n"));

	  portNum = portNum ? portNum : COAP_DEFAULT_PORT;
	  local_addr = local_addr ? local_addr : IP_ADDR_ANY;

	  coap_resourceInit();
	  /* if dns client not yet initialized... */
	  if(coap_pcb == NULL)
	  {
	    coap_pcb = udp_new();
	  }
	    if (coap_pcb != NULL) {

	      /* initialize COAP client */
	      udp_bind(coap_pcb, local_addr, portNum);
	      udp_recv(coap_pcb, coapd_recv, NULL);
	      LWIP_DEBUGF(COAP_DEBUG, ("Binding to IPs: "));
	      ip_addr_debug_print(COAP_DEBUG, local_addr->addr);
	      LWIP_DEBUGF(COAP_DEBUG, (" on Port:%i\n", portNum));
	    }
}
