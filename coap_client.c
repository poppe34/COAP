/*
 * coap_client.c
 *
 *  Created on: Dec 3, 2013
 *      Author: poppe
 */
#include "coap.h"
#include "stdint.h"

void coap_send_nonConfirmable(uint8_t *data, size_t len, coap_methods_t method)
{

}

void coap_simpleReply(coap_pkt_t *pkt, uint8_t code)
{
	coap_pkt_hdr_t header;

	header.bits.ver = pkt->header.bits.ver;
	header.bits.t = COAP_ACK;
	header.bits.tkl = pkt->header.bits.tkl;

	header.code = code;
	header.msgID = pkt->header.msgID;

	coapd_sendto((uint8_t *)&header, 4, pkt->ip_addr, pkt->port);

}

void coap_reply(coap_pkt_t *pkt, uint8_t *data, size_t len, uint8_t code, coap_option_t *opts)
{
	RT_ASSERT(pkt);
	coap_pkt_t outPkt;

	memset(&outPkt, 0, sizeof(coap_pkt_t));

	outPkt.header.bits.ver = pkt->header.bits.ver;
	outPkt.header.bits.t = COAP_ACK;
	outPkt.header.bits.tkl = pkt->header.bits.tkl;

	outPkt.header.code = code;
	outPkt.header.msgID = pkt->header.msgID;

	outPkt.ip_addr = pkt->ip_addr;
	outPkt.port = pkt->port;

	outPkt.options = opts;

	if(outPkt.header.bits.tkl)
	{
		size_t numTks = (size_t)pkt->header.bits.tkl;
		memcpy(outPkt.token, pkt->token, numTks);
	}

	outPkt.payload.data = data;
	outPkt.payload.len = len;

	LWIP_DEBUGF(COAP_DEBUG, ("Sending Coap Code: %i with data len %i\n", code, len));

	coap_sendPkt(&outPkt);
}
