/*
 * coap_err.h
 *
 *  Created on: Aug 2, 2013
 *      Author: poppe
 */

#ifndef COAP_ERR_H_
#define COAP_ERR_H_

typedef enum coap_err_e coap_err_t;

enum coap_err_e {
	coap_OK,
	coap_format_error,
	coap_pkt_to_small,
	coap_memory_error,
};


#endif /* COAP_ERR_H_ */
