/*
 * coap_resources.c
 *
 *  Created on: Dec 5, 2013
 *      Author: poppe
 */

#include <coap.h>

void nullCB(coap_pkt_t *, coap_resource_t *);

SLL(coapRSrcList);
SLL(rSrcDirAttrList);

static coap_resource_t rsrcDir;
static char rsrcDirPath[] = "/.well-known/core";

/**
 * Static Prototypes
 */
static int coap_findWithHash(coap_resource_t *rSrc, uint32_t *hash);
static void coap_buildRunningHash(void *data, void *arg1, void *arg2);
static void rSrcDirCB(coap_pkt_t *, coap_resource_t *);
static char *coap_printResource(coap_resource_t *rSrc, char *ptr);
static uint8_t coap_resourceContainsQuery(coap_resource_t *rSrc, coap_option_t *opt);

coap_resource_t *coap_resourceRegister(const char *URI)
{
	coap_resource_t *newRSrc = coap_malloc(sizeof(coap_resource_t));
	memset(newRSrc, '\0', sizeof(coap_resource_t));

	char *newStr = coap_malloc(strlen(URI+1));
	strcpy(newStr, URI);

	sll_addData(coapRSrcList, newRSrc);

	newRSrc->fullUri = newStr;
	newRSrc->fullUriHash = coap_Hash(newStr);
	newRSrc->attrList = (struct sll_list *)coap_malloc(sizeof(struct sll_list));
	newRSrc->pathList = (struct sll_list *)coap_malloc(sizeof(struct sll_list));

	sll_init(newRSrc->attrList);
	sll_init(newRSrc->pathList);
	LWIP_DEBUGF(COAP_DEBUG, ("Register URI: %s with Hash: %i\n", newRSrc->fullUri, newRSrc->fullUriHash));

	return newRSrc;
}

void coap_resourceRegisterCB(coap_resource_t *rSrc, coap_resourceCB callback, coap_methods_t method )
{
	RT_ASSERT(rSrc);
	RT_ASSERT(callback);

	switch(method)
	{
	case coap_get:
		rSrc->getCB = callback;
		break;
	case coap_post:
		rSrc->postCB = callback;
		break;
	case coap_put:
		rSrc->putCB = callback;
		break;
	case coap_delete:
		rSrc->deleteCB = callback;
		break;
	}

	LWIP_DEBUGF(COAP_DEBUG, ("Registered callback: %x with method %i\n", callback, method));
}

void coap_resourceRegisterAttribute(coap_resource_t *rSrc, const char *key, const char *value)
{
	RT_ASSERT(rSrc);

	coap_attribute_t *newAttr = coap_malloc(sizeof(coap_attribute_t));
	char *newKey, *newValue;

	newKey = coap_malloc(strlen(key)+1);
	newValue = coap_malloc(strlen(value)+1);

	strcpy(newKey, key);
	strcpy(newValue, value);

	newAttr->key = newKey;
	newAttr->value = newValue;
	newAttr->next = NULL;

	sll_addData(rSrc->attrList, newAttr);
	LWIP_DEBUGF(COAP_DEBUG, ("Registered Key %s with Value: %s\n", newKey, newValue));
	return;

}

coap_attribute_t *coap_resourceContainsAttribute(coap_resource_t *rSrc, const char *key)
{

}


uint32_t coap_buildURI(coap_pkt_t *pkt)
{
	RT_ASSERT(pkt);
	coap_option_t *uriOpt;
	uint32_t hash;
	uint8_t buf[128];
	uint8_t *ptr = buf;
	uint8_t multiQuery;
	uriOpt = pkt->options;

	while(uriOpt)
	{
		if(uriOpt->num == coap_Uri_path)
		{
			*ptr++ = '/';
			memcpy(ptr, uriOpt->value, uriOpt->len);
			ptr += uriOpt->len;

		}
		if(uriOpt->num == coap_uri_query)
		{
			if(!multiQuery)
				*ptr++ = '?';
			else
				*ptr++ = '&';

			multiQuery++;

			memcpy(ptr, uriOpt->value, uriOpt->len);
		}

		SLL_NEXT(uriOpt);
	}
	*ptr = '\0';

	hash = coap_Hash(buf);

	LWIP_DEBUGF(COAP_DEBUG, ("incoming URI:%s with a hash of:%i\n", buf, hash));

	return hash;
}


coap_err_t coap_resourceDiscovery(coap_pkt_t *pkt, coap_resource_t **foundRes)
{
	RT_ASSERT(pkt);
	coap_option_t *uriOpt;
	uint32_t hash;
	//TODO: move the option variable into the function
	//uriOpt = coap_findOption(pkt, coap_Uri_path);

	if(proxy_on)
	{
		coap_forwardPkt(pkt);
	}

	hash = coap_buildURI(pkt);

	*foundRes = (coap_resource_t *)sll_searchWithin(coapRSrcList, coap_findWithHash, &hash);

	LWIP_DEBUGF(COAP_DEBUG, ("Resource found: %s\n", (*foundRes)->fullUri));

	//callback function for found resource method
	//TODO:This might become its own function after we find the resource
	//TODO: This needs to move to allow for block transfers

	return coap_OK;
}
sll_list_t coap_breakupURI(coap_resource_t *rSrc, uint8_t *uri)
{
	uint8_t *ptr = uri, *start, end;
	uint32_t len = 0;
	coap_option_t *opt, tempOpt;
	SLL_MALLOC(optList);

	LWIP_DEBUGF(COAP_DEBUG, ("Full URI of %s is being broken-up\n", uri));

	while(*ptr)
	{
		if(('a'<= *ptr && *ptr <= 'z') || ('A'<= *ptr && *ptr <= 'Z'))
		{
			if(!start)
				start = ptr;
			len++;
		} else if( *ptr == '/')
		{
			end = 1;

		} else if(*ptr == '\%')
		{

		}

		if(end)
		{
			end = 0;
			opt = (coap_option_t *)malloc(sizeof(coap_option_t)+len+1);
			opt->num = coap_Uri_path;
			opt->len = len;
			opt->value[opt->len] = '\0';
			memcpy(opt->value, start, (opt->len));

			start = NULL;
			sll_addData(optList, opt);

			LWIP_DEBUGF(COAP_DEBUG, ("URI section: %s of len %i added to list\n", opt->value, opt->len));
		}

		ptr++;
	}

	return optList;
}

void coap_resourceCallback(coap_resource_t *foundRes, coap_pkt_t *pkt)
{

		switch(pkt->header.code_bits.type)
		{
		case coap_get:
			LWIP_DEBUG(COAP_DEBUG, ("Get found:\n"));
			if(foundRes->getCB)
				foundRes->getCB(pkt, foundRes);
			else coap_simpleReply(pkt, COAP_CODE(204));
		break;

		case coap_post:
			LWIP_DEBUG(COAP_DEBUG, ("Post found:\n"));
			if(foundRes->postCB)
				foundRes->postCB(pkt, foundRes);
			else coap_simpleReply(pkt, COAP_CODE(204));
		break;

		case coap_put:
			LWIP_DEBUG(COAP_DEBUG, ("Put found:\n"));
			if(foundRes->putCB)
				foundRes->putCB(pkt, foundRes);
			else coap_simpleReply(pkt, COAP_CODE(204));
		break;

		case coap_delete:
			LWIP_DEBUG(COAP_DEBUG, ("Delete found:\n"));
			if(foundRes->deleteCB)
				foundRes->deleteCB(pkt, foundRes);
			else coap_simpleReply(pkt, COAP_CODE(204));
		break;

		default:
			LWIP_DEBUGF(COAP_DEBUG, ("Code Not Found\n"));

		}

}

void coap_displayResources(void)
{
	sll_node_t curRsrc = coapRSrcList->first;
	coap_resource_t *cur = (coap_resource_t *)curRsrc;
	coap_attribute_t *attr;
	sll_list_t attrList;
	LWIP_DEBUGF(COAP_DEBUG,
			("+++++++++======My Coap implementation: ver: %i.%i===========++++++++++\n",
					COAP_VER_MAJOR, COAP_VER_MINOR));

	while(cur)
	{
		LWIP_DEBUGF(COAP_DEBUG, ("******URI: \"%s\" with hash: %i ********\n", cur->fullUri, cur->fullUriHash));
		if(cur->getCB)
			LWIP_DEBUGF(COAP_DEBUG, ("    Has Get CB\n"));
		if(cur->postCB)
				LWIP_DEBUGF(COAP_DEBUG, ("    Has Post CB\n"));
		if(cur->deleteCB)
				LWIP_DEBUGF(COAP_DEBUG, ("    Has Delete CB\n"));
		if(cur->putCB)
				LWIP_DEBUGF(COAP_DEBUG, ("    Has Put CB\n"));

		attr = cur->attrList->first;

		//sll_forEachData(coapRSrcList,
		while(attr)
		{
			LWIP_DEBUGF(COAP_DEBUG, ("         Attribute: %s Value: %s\n", attr->key, attr->value));
			attr = (coap_attribute_t *)sll_next(attr);
		}
		LWIP_DEBUGF(COAP_DEBUG, ("\n"));
		SLL_NEXT(cur);
	}
}

void nullCB(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	RT_ASSERT(pkt);
	coap_pkt_t outPkt;
	uint8_t *newPtr;

	memset(&outPkt, 0, sizeof(coap_pkt_t));

	newPtr = (uint8_t *)&outPkt.header;
	LWIP_DEBUGF(COAP_DEBUG, ("Using Default Callback\n"));


	outPkt.header.bits.ver = pkt->header.bits.ver;
	outPkt.header.bits.t = COAP_ACK;
	outPkt.header.bits.tkl = pkt->header.bits.tkl;
	outPkt.header.msgID = pkt->header.msgID;

	outPkt.header.code_bits.code = 4; //COAP_CODE(404);
	outPkt.header.code_bits.type = 4;

	outPkt.ip_addr = pkt->ip_addr;
	outPkt.port = pkt->port;

	if(outPkt.header.bits.tkl)
	{
		size_t numTks = (size_t)pkt->header.bits.tkl;
		memcpy(outPkt.token, pkt->token, numTks);
	}



	LWIP_DEBUGF(COAP_DEBUG, ("4 Byte Header %o %o %o %o\n", *newPtr, *(newPtr+1), *(newPtr+2), *(newPtr+3)));

	coap_sendPkt(&outPkt);

}
/**
 * LINK FORMAT
 */

//TODO: Make special note that the old packet is now the reply packet.... Edit it if you want options and or payload, but just \
		replay the packet code though
static void rSrcDirCB(coap_pkt_t *pkt, coap_resource_t *rSrc)
{
	//FIXME: I need to be able to send this in blocks.  Right now I am going to assume that it fits fine into the buffer but in the future that will not be the case


	declareBlockVar(buf, 64, cnt);

	LWIP_DEBUGF(COAP_DEBUG, ("Blockwise receive Block:%i Start:%i End:%i\n", pkt->block.bits.num, cntStart, cntEnd));

	coap_attribute_t *attrs;
	coap_option_t *opt;

	rSrc = (coap_resource_t *)coapRSrcList->first;
	/**
	 * FIXME even numbers right now have quotes around it
	 * TODO implement query filter functionality... Looks tough
	 */
	LWIP_DEBUGF(COAP_DEBUG, ("/.well-known/core CB executed\n"));

	opt = coap_findOption(pkt, coap_uri_query);

	if(opt)
	{
		while(rSrc)
		{
			if(coap_resourceContainsQuery(rSrc, opt))
			{
				//coap_printResource(rSrc, ptr);
			}
		}
	}
	{
		while(rSrc)
		{
			 // This gives me the format ---> <URI>;attr="value";attr="value, <URI>;attr="value"
			coap_attribute_t *attrs;
			//URI-Reference

			if(cntRunning != 0)
			{
				blockAddChar(buf, cnt, ',');
			}

			blockAddChar(buf, cnt, '<');

			blockAddString(buf, cnt, rSrc->fullUri);

			blockAddChar(buf, cnt, '>');

			//blockReturn(cnt);
			//link-params
			attrs = rSrc->attrList->first;
			while(attrs)
			{
				blockAddChar(buf, cnt, ';');

				blockAddString(buf, cnt, attrs->key);

				blockAddChar(buf, cnt, '=');
				blockAddChar(buf, cnt, '"');

				blockAddString(buf, cnt, attrs->value);

				blockAddChar(buf, cnt, '"');

				attrs = attrs->next;
			}

			rSrc = rSrc->next;
		}
	}
	*bufPtr = '\0';

	LWIP_DEBUGF(COAP_DEBUG,(".well-known reply: %s", buf));

	uint8_t format = mime_application_link_format;
	coap_option_t *optList;
	coap_option_t *optBlock;

	coap_addOptionList(&optList, coap_content_format, &format, 1, 0);

	if(cntRunning > cntEnd || cntStart != 0)
	{
		optBlock = coap_blockCreateOpt(64, (cntStart/64), 0);
		coap_addOptionToList(&optList, optBlock);
	}

	coap_reply(pkt, buf, rt_strlen(buf), COAP_CODE(205), optList);

}

static uint8_t coap_resourceContainsQuery(coap_resource_t *rSrc, coap_option_t *opt)
{
#if 0
	char
	sll_searchWithin(rSrc->attrList, coap_sllFindAttribute, opt->)
#endif
	return 0;

}

static char *coap_printResource(coap_resource_t *rSrc, char *ptr)
{
	 // This gives me the format ---> <URI>;attr="value";attr="value, <URI>;attr="value"
	coap_attribute_t *attrs;
	//URI-Reference
	*ptr++ = '<';
	strcpy(ptr, rSrc->fullUri);
	ptr += strlen(rSrc->fullUri);
	*ptr++ = '>';

	//link-params
	attrs = rSrc->attrList->first;
	while(attrs)
	{
		*ptr++ = ';';
		strcpy(ptr, attrs->key);
		ptr += strlen(attrs->key);
		*ptr++ = '=';
		*ptr++ = '"';
		strcpy(ptr, attrs->value);
		ptr += strlen(attrs->value);
		*ptr++ = '"';
		attrs = attrs->next;
	}
	if(rSrc = rSrc->next)
		*ptr++ = ',';

	return ptr;
}
static int coap_findWithHash(coap_resource_t *rSrc, uint32_t *hash)
{
	LWIP_DEBUGF(COAP_DEBUG, ("Find Hash: %i compare Hash: %i\n", *hash, rSrc->fullUriHash));

	if(rSrc->fullUriHash == *hash)
		return -1;

	return 0;
}

static void coap_buildRunningHash(void *data, void *arg1, void *arg2)
{

	coap_option_t *uriOpt = (coap_option_t *)data;
	enum coap_options *optNum = (enum coap_options *)(arg1);
	uint32_t *hash = (uint32_t *)(arg2);

	if(uriOpt->num == *optNum)
		*hash = coap_runningHash(uriOpt->value, *hash);


}

void coap_resourceInit(void)
{
	SLL_MALLOC(pathList);

	rsrcDir.fullUri = rsrcDirPath;
	rsrcDir.fullUriHash = coap_Hash(rsrcDirPath);

	rsrcDir.attrList = rSrcDirAttrList;
	/* Not sure if I need this so I left it as a malloc */
	rsrcDir.pathList = pathList;

	coap_resourceRegisterCB(&rsrcDir, rSrcDirCB, coap_get);
	sll_addData(coapRSrcList, &rsrcDir);

	LWIP_DEBUGF(COAP_DEBUG, ("Resource Init'd\n"));
}

static int coap_sllFindAttribute(void *arg1, void *arg2)
{
	coap_attribute_t *attr = (coap_attribute_t *)arg1;
	char *key = (char *)arg2;
	//TODO: in the future I need to add *for general broader searches
	if(!rt_strcmp(attr->key, key))
	{
		return -1;
	}

	return 0;
}
