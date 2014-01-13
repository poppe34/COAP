/*
 * coap_msgEncoder.c
 *
 *  Created on: Dec 6, 2013
 *      Author: poppe
 */
#include "coap.h"

static uint8_t outBuf[128];
static uint16_t msgID;

static uint16_t coap_getMSGID(void);


void coap_sendPkt(coap_pkt_t *pkt)
{
	RT_ASSERT(pkt);
	uint8_t *buf = &outBuf;
	coap_encodePkt(pkt, &buf, 128);
}
/**
 * This is the main function call to send the Coap Pkt
 */
int coap_encodePkt(coap_pkt_t *pkt, void **buffer, uint32_t size)
{
	RT_ASSERT(pkt);
	RT_ASSERT(pkt->header);


	uint8_t *ptr = (uint8_t *)*buffer;

	//Add the header to the packet
	memcpy(ptr, pkt->header, 4);
	ptr += 4;
	size -= 4;

	//Add the Token to the packet
	if(0 < pkt->header->bits.tkl < 9)
	{
		memcpy(ptr, pkt->token, pkt->header->bits.tkl);
		ptr += pkt->header->bits.tkl;
		size -= pkt->header->bits.tkl;
	}
	//Add options to the packet
	if(pkt->options)
		ptr = coap_encodeOptions(pkt, ptr, size);

	//Add marker for payload
	*ptr++ = 0xff;

	//Add Payload to the packet
	if(pkt->payload.data)
	{
		memcpy(ptr, pkt->payload.data, pkt->payload.len);
		size += pkt->payload.len;
		ptr += pkt->payload.len;
	}

#if 0
	for(uint16_t x=0; x < (ptr-outBuf); x++)
	{
		LWIP_DEBUGF(COAP_DEBUG, ("0x%02x ", outBuf[x]));
	}
#endif
	coapd_sendto(outBuf, (ptr - outBuf), pkt->ip_addr, pkt->port);

	return 0;

}

coap_err_t coap_sendAckFrom(uint16_t msgID, ip_addr_t *addr, uint16_t port)
{
	coap_pkt_hdr_t header;

	header.code = COAP_CODE(205);
	header.msgID = msgID;
	header.bits.t = COAP_ACK;
	header.bits.ver = 1;
	header.bits.tkl = 0;

	coapd_sendto(&header, 4, addr, port);

	return coap_OK;
}

/**
 * This function takes in the URL and decomposes it into coap options
 *
 * TODO coap-decompose_url has alot of work to do
 */
coap_err_t coap_decompose_url(struct coap_pkt *pkt)
{
	/* 1. fail if it is not an absolute URL defined in RFC 3986 */
	//TODO 1.

	return coap_OK;
}


uint8_t *coap_encodeOptions(coap_pkt_t *pkt, uint8_t *buf, uint32_t size)
{
	RT_ASSERT(pkt);
	uint16_t opt_sum, opt_num, opt_len, running_num = 0, delta = 0;
	uint8_t *deltaLen;
	coap_option_t *opt = pkt->options;

	while(opt && size)
	{
		opt_num = opt->num;
		opt_len = opt->len;

		uint32_t sizeNeeded = opt->len + 1;
		if(delta > 12)
		{
			sizeNeeded++;
			if(delta > 269)
				sizeNeeded++;
		}
		if(opt_len >12)
		{
			sizeNeeded++;
			if(opt_len > 269)
				sizeNeeded++;
		}

		if(sizeNeeded > size)
			return -1;

		LWIP_ASSERT("Options are not in descending order\n", running_num <= opt_num);

		delta = opt_num - running_num;

		//get the location that the option number and len go
		deltaLen = buf++;
		size--;
		if(delta < 13)
		{
			*deltaLen = (delta << 4);
		}
		else if(delta > 12 && delta < 269)
		{
			*deltaLen = (13 << 4);
			*buf++ = (delta - 13);
			size--;
		}
		else
		{
			*deltaLen = (14 << 4);
			*((uint16_t *)buf) = htons(delta - 269);
			buf += 2;
			size -= 2;
		}

		if(opt_len < 13)
		{
			*deltaLen |= opt_len;
		}
		else if(opt_len > 12 && opt_len < 269)
		{
			*deltaLen |= 13;
			*buf++ = opt_len - 13;
			size--;
		}
		else
		{
			*deltaLen = 14;
			*((uint16_t *)buf) = htons(opt_len - 269);
			buf += 2;
			size -= 2;
		}

		memcpy(buf, opt->value, opt->len);
		buf += opt->len;
		size -= opt->len;
		running_num = opt_num;
		SLL_NEXT(opt);
	}

	return buf;
}

static uint16_t coap_getMSGID(void)
{
	return ++msgID;
}



