#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"
#include <arpa/inet.h>
#define NEXT_HOP_OFFSET 0x04
#define COST_OFFSET 0x06
#define ROUTER_OFFSET 0x08

void routing_table(int sock_index){
	//Use this function assuming the routing table is accurate, only to send
	//Include the actual bellman ford implementation in globals
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	const char *key;
	map_iter_t iter = map_iter(&port_router_map);
	int a = 0;
	while ((key = map_next(&port_router_map, &iter))) {
		uint16_t port = *map_get(&port_router_map, key);
  		printf("%s -> %u\n", key, port);
		++a;

	}
        payload_len = 8*a;
	printf("payload_len: %i\n", payload_len);
        cntrl_response_payload = (char *) malloc(payload_len);
	memset(cntrl_response_payload, '\0', payload_len);
	// Add the id's		
	printf("ROUTINGTABLE\nROUTINGTABLE\nROUTINGTABLE\nBEGIN\n");	
	iter = map_iter(&index_map);
	short b = 0; 
	while ((key = map_next(&index_map, &iter))) {
		uint16_t id = *map_get(&index_map, key);
  		printf("%s -> %u\n", key, id);
		uint16_t  _key = strtol(key, NULL, 10);
		printf("key (before htons): %u\n", _key);	
		_key = htons(_key);
		printf("key (after htons): %u\n", _key);	
		memcpy(cntrl_response_payload + b*ROUTER_OFFSET, &_key, sizeof(_key));
			
		//nexthop
		uint16_t _nexthop = *map_get(&next_hop, key);
		_nexthop = htons(_nexthop);
		printf("next hop: %u\n", _nexthop);
		memcpy(cntrl_response_payload + b*ROUTER_OFFSET + NEXT_HOP_OFFSET, &_nexthop, sizeof(_nexthop));

		//cost
		//cost is gonna be cost to get to next hop and from there onwards
		//get next hop
		//get cost to next hop
		//get cost from dv?
//		uint16_t _cost = *map_get(&weight_map, key);
		char nhkey[50];
		memset(nhkey, '\0', sizeof(nhkey));
		//_COST IS the cost between the next hop and the current router
		sprintf(nhkey, "%u", _nexthop);
		//uint16_t _cost = *map_get(&weight_map, nhkey);
		unsigned long index = *map_get(&index_map, key);
		uint16_t _cost = dv[_row][index];	
		_cost = htons(_cost);
		printf("cost: %u\n", _cost);
		memcpy(cntrl_response_payload + b*ROUTER_OFFSET + COST_OFFSET, &_cost, sizeof(_cost));
		b++;
	}
	iter = map_iter(&next_hop);

	while ((key = map_next(&next_hop, &iter))) {
  		printf("next hop map: \n%s -> %u\n", key, *map_get(&next_hop, key));
	}
        cntrl_response_header = create_response_header(sock_index, 2, 0, payload_len);

        response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
        cntrl_response = (char *) malloc(response_len);
        /* Copy Header */
        memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
        free(cntrl_response_header);
        /* Copy Payload */
        memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
        free(cntrl_response_payload);

        sendALL(sock_index, cntrl_response, response_len);

	printf("ROUTINGTABLE\nROUTINGTABLE\nROUTINGTABLE\nEND\n");	
        free(cntrl_response);	
	
}
