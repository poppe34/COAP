/*
 * coap_shell.c

 *
 *  Created on: Aug 3, 2013
 *      Author: poppe
 */
#include "coap.h"
#include "finsh.h"
#include "lwip/debug.h"

static void coap_defaultGet(coap_pkt_t *pkt, coap_resource_t *rSrc);
static void coap_defaultPost(coap_pkt_t *pkt, coap_resource_t *rSrc);
static void coap_defaultPut(coap_pkt_t *pkt, coap_resource_t *rSrc);
static void coap_defaultDelete(coap_pkt_t *pkt, coap_resource_t *rSrc);

void coapStart(void)
{
	coapd_init(NULL, NULL);
	coap_resource_t *rSrc;
	rSrc = coap_resourceRegister("Matt");
	coap_resourceRegisterCB(rSrc, coap_defaultGet, coap_get);
	rSrc = coap_resourceRegister("Matt/Poppe");
	coap_resourceRegisterAttribute(rSrc, "Dog", "Eat");
	coap_resourceRegisterCB(rSrc, coap_defaultDelete, coap_delete);
	coap_resourceRegisterCB(rSrc, coap_defaultGet, coap_get);

	rSrc = coap_resourceRegister("what/a/day");
	coap_resourceRegisterAttribute(rSrc, "Pussy", "Willow");
	coap_resourceRegisterCB(rSrc, coap_defaultGet, coap_get);
	coap_resourceRegisterCB(rSrc, coap_defaultPut, coap_put);
	coap_resourceRegisterCB(rSrc, coap_defaultPost, coap_post);
	coap_resourceRegisterCB(rSrc, coap_defaultDelete, coap_delete);
	coap_resourceRegisterAttribute(rSrc, "new", "name");
	coap_resourceRegisterAttribute(rSrc, "Power Services", "Group");

	rSrc = coap_resourceRegister("feel/the/burn/please");
	coap_resourceRegisterAttribute(rSrc, "Hello", "Kitty");
	coap_resourceRegisterCB(rSrc, coap_defaultPut, coap_put);

}
FINSH_FUNCTION_EXPORT(coapStart, start the coap server);

void coapShowRes(void)
{
	coap_displayResources();
}
FINSH_FUNCTION_EXPORT(coapShowRes, display all registered resources);


void coapDummyPkt(void)
{
	coap_pkt_t pkt;

	uint8_t buf[9] = { 0x40, 1, 0x33, 0x44, 0xb4, 'M', 'a', 't', 't'};

	coap_outsideDataInput(buf, sizeof(buf));
	uint8_t buf2[15] = { 0x40, 1, 0x33, 0x44, 0xba, 'M', 'a', 't', 't', '/', 'P', 'o', 'p', 'p', 'e'};

	coap_outsideDataInput(buf2, sizeof(buf2));

}
FINSH_FUNCTION_EXPORT(coapDummyPkt, simulate a incoming pkt);

static void coap_defaultGet(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	RT_ASSERT(pkt);

	coap_pkt_t outPkt;
	memset(&outPkt, 0, sizeof(coap_pkt_t));

	coap_pkt_hdr_t header;
	coap_data_t data;
	uint8_t buf[] = "Matt Poppe is Cool";

	LWIP_DEBUGF(COAP_DEBUG, ("Get message received for topic: %s\n", rSrc->fullUri));

	outPkt.header = &header;
	outPkt.payload = &data;
	header.bits.ver = pkt->header->bits.ver;
	header.bits.t = COAP_ACK;
	header.bits.tkl = pkt->header->bits.tkl;
	header.msgID = pkt->header->msgID;

	outPkt.ip_addr = pkt->ip_addr;
	outPkt.port = pkt->port;

	if(header.bits.tkl)
	{
		size_t numTks = (size_t)pkt->header->bits.tkl;
		memcpy(&outPkt.token, pkt->token, numTks);
		outPkt.token[numTks] = '\0';
	}

	header.code = COAP_CODE(205);

	outPkt.payload->data = buf;
	outPkt.payload->len = rt_strlen(outPkt.payload->data);
	coap_sendPkt(&outPkt);

}

static void coap_defaultPost(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	LWIP_DEBUGF(COAP_DEBUG, ("Post message received for topic: %s\n", rSrc->fullUri));
}
static void coap_defaultPut(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	LWIP_DEBUGF(COAP_DEBUG, ("Put message received for topic: %s\n", rSrc->fullUri));
}
static void coap_defaultDelete(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	LWIP_DEBUGF(COAP_DEBUG, ("Delete message received for topic: %s\n", rSrc->fullUri));
}
