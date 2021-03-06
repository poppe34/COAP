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

void coap_addOptionList(coap_option_t **optList, uint8_t optNum, uint8_t *data, uint32_t len, uint8_t optFormat)
{
	RT_ASSERT(optList);

	coap_option_t *opt = (coap_option_t *)coap_malloc(sizeof(coap_option_t) + len);
	coap_option_t *tempOpt = *optList;

	opt->num = optNum;
	opt->len = len;
	opt->format = optFormat;
	opt->next = NULL;
	memcpy(opt->value, data, len);

	//Need to make sure that the options are in ascending order also if the number is the same put it after existing

	if(tempOpt == NULL)
		*optList = opt;
	else
	{
		while(tempOpt->num <= opt->num )
			SLL_NEXT(tempOpt);

		sll_insertBefore(optList, tempOpt, opt);
	}
}

void coap_addOptionToList(coap_option_t **optList, coap_option_t *opt)
{
	coap_option_t *tempOpt = *optList;

	if(tempOpt == NULL)
		*optList = opt;
	else
	{
		while(tempOpt->num <= opt->num )
		{
			if(tempOpt->next == NULL);
			{
				tempOpt->next = opt;
				return;
			}
			SLL_NEXT(tempOpt);
		}

		sll_insertBefore(optList, tempOpt, opt);
	}
}

coap_option_t *coap_createOption(uint8_t optNum, uint8_t *data, uint32_t len, uint8_t optFormat)
{

		coap_option_t *opt = (coap_option_t *)coap_malloc(sizeof(coap_option_t) + len);
		rt_memset(opt, '\0', (sizeof(coap_option_t) + len));

		opt->num = optNum;
		opt->len = len;
		opt->format = optFormat;
		opt->next = NULL;
		memcpy(opt->value, data, len);

		return opt;
}
