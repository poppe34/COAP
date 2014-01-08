/*
 * coap_proxy.c
 *
 *  Created on: Dec 21, 2013
 *      Author: poppe
 */

#include "coap.h"

uint8_t proxy_on = COAP_FALSE;
static ip_addr_t addr;
static uint16_t port;

void coap_forwardPkt(coap_pkt_t *pkt)
{

	pkt->ip_addr = &addr;
	pkt->port = port;
	coap_sendPkt(pkt);
}
