/*
 * coap_msg.h
 *
 *  Created on: Aug 1, 2013
 *      Author: poppe
 */
#ifndef COAP_MSG_H_
#define COAP_MSG_H_

#include "sll.h"
#include "lwip/ip_addr.h"

struct coap_pkt_hdr_s {
	union {
		struct {
			uint8_t tkl:4;
			uint8_t t:2;
			uint8_t ver:2;
		}__attribute__( ( __packed__ ) )bits;
		uint8_t firstByte;
	}__attribute__( ( __packed__ ) );
	union {
		struct {
			uint8_t code:5;
			uint8_t type:3;
		}__attribute__( ( __packed__ ) )code_bits;
		uint8_t code;
	}__attribute__( ( __packed__ ) );
	uint16_t msgID;
}__attribute__( ( __packed__ ) );

/**
 * Forward Declarations
 */
struct coap_pkt;
struct coap_resource;
struct coap_uriPath;
struct coap_option;
struct coap_attribute;

typedef struct coap_pkt_s coap_pkt_t;
typedef struct coap_resource coap_resource_t;
typedef struct coap_uriPath coap_uriPath_t;
typedef struct coap_option coap_option_t;
typedef struct coap_attribute coap_attribute_t;

typedef void (*coap_resourceCB)(coap_pkt_t *, coap_resource_t *);


typedef struct coap_data {
	uint8_t *	data;
	uint32_t 	len;
}coap_data_t;


/**
 * Based of RFC 6690...
 */
struct coap_attribute {
	coap_attribute_t		*next;
	char 					*key;
	char 					*value;
};




typedef struct coap_pkt_hdr_s coap_pkt_hdr_t;

struct coap_option {
	coap_option_t 	*next;
	uint16_t		num;
	uint8_t			len;
	uint8_t			format;
	uint8_t			value[];
};



struct coap_pkt_s {
	coap_pkt_hdr_t *	header;
	uint8_t  			token[9];
	coap_option_t *		options;
	coap_data_t			payload;
	ip_addr_t *			ip_addr;
	uint16_t			port;
};


enum coap_option_format_e {
	coap_opt_opaque,
	coap_opt_string,
	coap_opt_uint,
	coap_opt_none,
};

struct coap_uriPath {
	coap_uriPath_t			*next;
	char *					path;
	uint32_t 				hash;
	uint32_t 				size;
};

struct coap_resource {
	void *					next;
	char *					fullUri;
	uint32_t				fullUriHash;
	sll_list_t 			  	pathList;
	sll_list_t				attrList;
	uint32_t				flags;
	coap_resourceCB			getCB;
	coap_resourceCB 		postCB;
	coap_resourceCB 		deleteCB;
	coap_resourceCB			putCB;
};

enum coap_options {
	coap_If_Match = 1,
	coap_Uri_host = 3,
	coap_ETAG = 4,
	coap_If_None_Match = 5,
	coap_Uri_Port = 7,
	coap_location_path = 8,
	coap_Uri_path = 11,
	coap_content_format = 12,
	coap_max_age = 14,
	coap_uri_query = 15,
	coap_accept = 17,
	coap_location_query = 20,
	coap_proxy_uri = 35,
	coap_proxy_scheme = 39,
	coap_size1 = 60,
};

typedef enum coap_methods {
	coap_get,
	coap_post,
	coap_put,
	coap_delete,
}coap_methods_t;

enum coap_mime_types {
	mime_text_plain = 0,
	mime_application_link_format = 40,
	mime_application_xml = 41,
	mime_application_octet_stream = 42,
	mime_application_exi = 47,
	mime_application_json = 50,
};



enum coap_respSuccess {
	created = 1,
	deleted = 2,
	valid = 3,
	changed = 4,
	content = 5,
};

enum coap_respClientErr {
	bad_request = 0,
	unauthorized = 1,
	bad_option = 2,
	forbidden = 3,
	not_found = 4,
	method_not_allowed = 5,
	not_acceptable = 6,
	precondition_failed = 12,
	enitiy_to_large = 13,
	unsupported_format = 15,
};

enum coap_respServerErr {
	internal_err = 0,
	not_implemented = 1,
	bad_gateway = 2,
	service_unavail = 3,
	gateway_timeout = 4,
	proxy_not_supported =  5,
};
#endif /* COAP_MSG_H_ */
