/*
 * coap_block.c
 *
 *  Created on: Jan 19, 2014
 *      Author: poppe
 */

#include "coap.h"

coap_option_t *coap_blockCreateOpt(uint16_t size, uint32_t offset, uint8_t more)
{
	coap_block_t block;
	uint8_t optLen = 1;

	if(more)
		block.bits.m = 1;

	block.offset = offset;
	block.bits.szx = ((size >> 4) & 0x7);

	LWIP_DEBUGF(COAP_DEBUG,("Block num: %i size:%i more: %i\n", block.bits.num, block.bits.szx, block.bits.m));

	if(offset > 15)
		optLen++;

	return coap_createOption(coap_block2, (&block+2), optLen, NULL);
}

