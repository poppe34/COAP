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
	uint8_t blockdata[4];

	if(more)
		block.m = 1;

	block.num = offset;
	block.szx = ((size >> 5) & 0x7);

	LWIP_DEBUGF(COAP_DEBUG,("Block num: %i size:%i more: %i\n", block.num, block.szx, block.m));

	optLen = COAP_ENCODE_BYTES(blockdata, ((block.num << 4) |  (block.m << 3) | (block.szx)));

	//FIXME: this doesn't seem right at all... Make sure that this passes the right block number for all numbers
	return coap_createOption(coap_block2, &blockdata, optLen, NULL);
}

uint32_t coap_blockSize(coap_option_t *opt)
{
	coap_block_t block;
	uint32_t optLen = opt->len;
	uint32_t blockLen;

	rt_memcpy((&block + (3 - optLen)), opt->value, optLen);
	blockLen = block.szx << 4;

	LWIP_DEBUGF(COAP_DEBUG, ("block len: %i", blockLen));

	return blockLen;
}
