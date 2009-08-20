#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include "ipfilter.h"


IPLIST *iplist = NULL;


void iplist_init()
{
	iplist = (IPLIST*)calloc(IP_LIST_LEN, sizeof(IPLIST));
	assert(iplist);

	return;
}							   

int iplist_insert(unsigned netlong, unsigned mask, unsigned region)
{
	IPNODE *tmpnode;
	unsigned net_high16_bit;

	net_high16_bit = ntohs(netlong);

	//printf("net_high16_bit : %u %u %u %u\n",net_high16_bit, mask, region, netlong);
	
	tmpnode = iplist[net_high16_bit].ipnode;
	while(tmpnode != NULL)
	{
		if(tmpnode->netlong != netlong)
		{
			tmpnode = tmpnode->next;
		}
		else
		{
			return 0;
		}
	}

	tmpnode = (IPNODE*)calloc(1, sizeof(IPNODE));
	assert(tmpnode);

	tmpnode->mask 	 = mask;
	tmpnode->region  = region;
	tmpnode->netlong = netlong;

	tmpnode->next = iplist[net_high16_bit].ipnode;
	iplist[net_high16_bit].ipnode = tmpnode;

	return 0;
}

int iplist_load(const u_char *pathname)
{
	unsigned netlong,mask,region;
	char *t,src[128],ip[128];
	FILE *fp;
	struct in_addr inp;
	
	fp=fopen(pathname, "r");
	assert(fp);
			
	while(fgets(src,128,fp) != NULL)
	{
		assert((t=strchr(src, '/')));
		*t = ' ';

		sscanf(src,"%s %u %u\n", ip, &mask, &region);

		assert(inet_aton(ip, &inp) != 0);

		netlong = inp.s_addr;
		
		iplist_insert(netlong, mask, region);
	}

	return 0;
}

int iplist_search(unsigned long netlong)
{
	IPNODE *tmpnode;
	unsigned net_high16_bit,mask_bit;

	net_high16_bit = ntohs(netlong);

	tmpnode = iplist[net_high16_bit].ipnode;
	while(tmpnode != NULL)
	{
		mask_bit = 0xFFFFFFFF << (32-tmpnode->mask);
		
		if((ntohl(netlong) & mask_bit) == (ntohl(tmpnode->netlong) & mask_bit))
		{
			return tmpnode->region;
		}
		else
		{
			tmpnode = tmpnode->next;
		}
	}

	return -1;
}			   

void iplist_destroy()
{
	int i;
	
	for(i=0; i<IP_LIST_LEN; i++)
	{
		if(iplist[i].ipnode != NULL)
			free(iplist[i].ipnode);
	}
	
	free(iplist);

	return;
}
							 

