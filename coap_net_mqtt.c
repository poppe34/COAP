/*
 * coap_net_mqtt.c
 *
 *  Created on: Jan 6, 2014
 *      Author: poppe
 */

#include "coap.h"
#include "lwip/debug.h"


void coap_outsideDataInput(uint8_t *buf, uint32_t len)
{
	LWIP_DEBUGF(COAP_DEBUG, ("----->Incoming packet size: %u\n", len));

	coap_pkt_t *pkt = (coap_pkt_t *)coap_malloc(sizeof(coap_pkt_t));
	memset(pkt, '\0', sizeof(coap_pkt_t));

	coap_parsePacket(pkt, buf, len);
	coap_resourceDiscovery(pkt);

	coap_free(pkt);
}
