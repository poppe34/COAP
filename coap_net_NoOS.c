/*
s * coap_net_NoOS.c
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
	coap_resource_t *foundRsrc;
	coap_option_t *opt;
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
	opt = coap_findOption(pkt, coap_block2);
	if(opt)
	{

		pkt->block.m = opt->value[opt->len-1] & 0x8;
		pkt->block.szx = opt->value[opt->len-1] & 0x7;
		pkt->block.num = coap_parse_bytes(opt->value, (opt->len - 1));
		pkt->block.num = pkt->block.num << 4 | ((opt->value[opt->len-1] & 0xf0) >> 4);


		LWIP_DEBUGF(COAP_DEBUG, ("Block cnt: %i size: %i %s", pkt->block.num, pkt->block.szx, \
				(pkt->block.m ? "with more to come\n":"\n")));


		/* FIXME: I need to make this use actual block size instead of just 64 */
		//pkt->block.offset = pkt->block.bits.num * (64);
	}
	err = coap_resourceDiscovery(pkt, &foundRsrc);

	coap_resourceCallback(foundRsrc, pkt);
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
