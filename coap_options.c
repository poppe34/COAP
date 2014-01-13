/*
 * coap_options.c
 *
 *  Created on: Aug 4, 2013
 *      Author: poppe
 */


#include "coap.h"
//TODO: make sure it return alll the options with that resource num
coap_option_t *coap_findOption(coap_pkt_t *pkt, uint16_t num)
{
	RT_ASSERT(pkt);

	coap_option_t *opt = pkt->options;

	for(;opt; opt = opt->next)
	{
		if(opt->num == num)
			return opt;
	}

	return NULL;
}


void coap_addOption(coap_pkt_t *pkt, uint8_t optNum, uint8_t *data, uint32_t len, uint8_t optFormat)
{
	RT_ASSERT(pkt);

	coap_option_t *opt = (coap_option_t *)coap_malloc(sizeof(coap_option_t) + len);
	coap_option_t *tempOpt;

	opt->num = optNum;
	opt->len = len;
	opt->format = optFormat;
	opt->next = NULL;
	memcpy(opt->value, data, len);

	//Need to make sure that the options are in ascending order also if the number is the same put it after existing

	tempOpt = pkt->options;

	if(tempOpt == NULL)
		pkt->options = opt;
	else
	{
		while(tempOpt->num <= opt->num )
			SLL_NEXT(tempOpt);

		sll_insertBefore(&pkt->options, opt, tempOpt);
	}
}
