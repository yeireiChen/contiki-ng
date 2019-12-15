#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/memb.h"
#include "lib/list.h"
#include "contiki.h"
#include "blacklist.h"
#include "net/packetbuf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "TSCH BLACK"
#define LOG_LEVEL LOG_LEVEL_DBG

#if ROUTING_CONF_RPL_LITE
#include "net/routing/rpl-lite/rpl.h"
#elif ROUTING_CONF_RPL_CLASSIC
#include "net/routing/rpl-classic/rpl.h"
#endif

uint32_t black_backup_temp_asn = 0;
uint8_t channel_size_before = 0;
uint8_t channel_size_after = 0;


void 
black_changeChannel(const uint8_t *payload){

	
	uint8_t channelSize = 0;
 	char *pch;
	int index = 0;
	uint8_t temp = 0;
	
	pch = strtok((char *)payload, "[':] ");
	channelSize = (uint8_t)atoi(pch);
	//LOG_INFO("size is %d\n",channelSize);

	channel_size_before = sizeof(tsch_hopping_sequence);
	while(pch != NULL) {
		if(index==0){
			index++;
			pch = strtok(NULL, "[':] ");
			continue;
		}
		//LOG_INFO("channel is %s\n",pch);
		
		temp = (uint8_t)atoi(pch);
		//LOG_INFO("channel is %d,%d\n",index,temp);
		tsch_black_hopping_sequence[index-1] = temp;
		pch = strtok(NULL, "[':] ");
		index++;
	}
	TSCH_ASN_DIVISOR_INIT(tsch_black_hopping_sequence_length, channelSize);
	channel_size_after = channelSize;

	/*uint8_t i=0;
	for(i=0;i<channelSize;i++){
		LOG_INFO("channel is %d\n",tsch_black_hopping_sequence[i]);
	}*/

	
  	LOG_INFO("array size : %d\n",channelSize);
  	LOG_INFO("asn_ms1b_remainder : %u\n",tsch_black_hopping_sequence_length.asn_ms1b_remainder);

  	LOG_INFO("channel list first \n");
  	tsch_channel_print(0);

}

void
black_change(void){

	LOG_INFO("channel list change first \n");
	tsch_channel_print(1);
	LOG_INFO("channellist change: val:%u\n",tsch_hopping_sequence_length.val);
	LOG_INFO("channellist change: remainder:%u\n",tsch_hopping_sequence_length.asn_ms1b_remainder);

	memcpy(tsch_hopping_sequence, tsch_black_hopping_sequence, sizeof(tsch_black_hopping_sequence));
  	TSCH_ASN_DIVISOR_INIT(tsch_hopping_sequence_length, channel_size_after);
  	
  	LOG_INFO("channel list change after \n");
  	tsch_channel_print(1);
  	LOG_INFO("channellist change: val:%u\n",tsch_hopping_sequence_length.val);
	LOG_INFO("channellist change: remainder:%u\n",tsch_hopping_sequence_length.asn_ms1b_remainder);

}

uint32_t 
getBlackASN(void){

	uint32_t asn = black_backup_temp_asn;
	black_backup_temp_asn = 0;
	return asn;
}

void 
black_wait_asn_change(uint32_t temp_asn){

	black_backup_temp_asn = temp_asn;
	LOG_INFO("Got ASN IN blacklist : %lu\n", black_backup_temp_asn);
}