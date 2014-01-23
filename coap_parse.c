/*
 * coap-parse.c
 *
 *  Created on: Aug 1, 2013
 *      Author: poppe
 */
/* IP Headers */
#include <stdint.h>

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"// ntohs, htons
#include "lwip/udp.h"

#include "coap.h"

/*--------------------*
 * PROTOTYPES
 *--------------------*/
static int coap_parseOptions(coap_pkt_t *pkt, void **buffer, size_t size);



uint32_t coap_parseUint(uint8_t *buf, uint32_t len)
{
  uint32_t value = 0, cnt;
  for (cnt = 0; cnt < len; ++cnt)
    value = (value << 8) + buf[cnt];

  return value;
}

/**
 * take in a packet and break it down.
 */
coap_err_t coap_parsePacket(coap_pkt_t *pkt, void *buf, size_t size )
{
	RT_ASSERT(pkt);

	if(size < COAP_MIN_PKT_SIZE)
	{
		return coap_pkt_to_small;
	}

	memcpy(&pkt->header, buf, 4);

	buf += sizeof(coap_pkt_hdr_t);
	size -= sizeof(coap_pkt_hdr_t);

	LWIP_DEBUGF(COAP_DEBUG, ("ver: %u t: %u tkl:%u code: %u \n", pkt->header.bits.ver,
			pkt->header.bits.t, pkt->header.bits.tkl, pkt->header.code));

	LWIP_DEBUGF(COAP_DEBUG, ("msg id: %u\n", htons(pkt->header.msgID)));

	if(pkt->header.bits.tkl)
	{
		//FIXME: I may not always get ascii for the token fix this for all cases
		memcpy(pkt->token, buf, pkt->header.bits.tkl);
		LWIP_DEBUGF(COAP_DEBUG, ("Recieved the tk: %s\n", pkt->token));
		buf += pkt->header.bits.tkl;
		size -= pkt->header.bits.tkl;
	}

	size = coap_parseOptions(pkt, &buf, size);



	if(*((uint8_t *)buf) == 0xFF && pkt->payload.data)
	{
		size--;
		buf++;

		pkt->payload.len = size;
		pkt->payload.data = (uint8_t *)coap_malloc(size + 1);
		memcpy(pkt->payload.data, buf, size);
	}


	return coap_OK;
}

/**
 * This function takes the currently parsed packet and the buffer and finds all the options and
 * sorts them.
 */
static int coap_parseOptions(coap_pkt_t *pkt, void **buffer, size_t size)
{
	uint8_t opt_sum = 0;
	uint8_t opt_num, opt_len;
	uint8_t loop = 1;
	coap_option_t *opt, *last_opt, **temp_opt = &pkt->options;
	uint8_t *buf = *buffer;
	uint8_t cnt=0;

	while(loop && size)
	{
		opt_num = (uint8_t)((*buf >> 4) & 0xf);
		opt_len = (uint8_t)(*buf & 0xf);

		LWIP_DEBUGF(COAP_DEBUG, ("LENGTH left in buffer %u\n", size));
		LWIP_DEBUGF(COAP_DEBUG, ("ADDR: %x OPT: %u LEN: %u\n", buf, opt_num, opt_len));

		buf++; (size)--;

		if(opt_num < 13)
		{
			opt_num += opt_sum;
		} else if (opt_num == 13)
		{
			opt_num = *buf++ + 13 + opt_sum;
			size--;
		} else if(opt_num == 14)
		{
			opt_num = (uint8_t)(ntohs(*buf)+ 269 + opt_sum);//TODO I might need to increase the buffer by one
			buf += 2;
			size -= 2;
		} else if(opt_num == 15)
		{
			return size;
		}


		if(opt_len == 13)
		{
			opt_len = *buf++ + 13;
			size--;
		} else if (opt_len == 14)
		{
			opt_len = (uint8_t)(ntohs(*(buf))+ 269);
			buf += 2;
			size -= 2;
		} else if (opt_len == 15)
		{
				return size;
		}

		opt = (coap_option_t *)coap_malloc(sizeof(coap_option_t)+opt_len+1);
		if(!opt)
		{
			LWIP_DEBUGF(COAP_DEBUG, ("Coap Parse: Out of Memory!!\n"));
		}
		opt_sum = opt_num;
		opt->num = opt_num;
		opt->len = opt_len;
		memcpy(opt->value, buf, opt_len);
		opt->value[opt_len] = '\0';
		opt->next = NULL;

		LWIP_DEBUGF((COAP_DEBUG | LWIP_DBG_TRACE), ("coap option: [%u (len=%u)]: %s\n", opt_num, opt_len, opt->value));

		if(*temp_opt)
		{
			last_opt->next = opt;
		}
		else
		{
			*temp_opt = opt;
		}
		last_opt = opt;

		buf += opt_len;
		size -= opt_len;
		if(cnt >= 6) loop=0; //This is temp I need to take this out when debugging is done
		cnt++;
	}

		return size;
}


coap_attribute_t *coap_attrKeyFromOpt(coap_option_t *opt)
{

}

/**
 * dismiss the payload of the coap message
 */
coap_err_t coap_dismissPayload(coap_pkt_t *pkt)
{

}


uint32_t coap_Hash(const char *str)
{
	return coap_runningHash(str, 5381);
}

uint32_t coap_runningHash(const char *str, uint32_t runHash)
{
	uint32_t hash = runHash;
	char c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}
