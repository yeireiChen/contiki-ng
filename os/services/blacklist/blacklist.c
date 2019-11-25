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


void 
black_changeChannel(uint8_t ch1, uint8_t ch2,uint8_t ch3){

	uint8_t changed[] = {ch1,ch2,ch3};

	/* Initialize blacklist hopping sequence test*/
  	memcpy(tsch_black_hopping_sequence, changed, sizeof(changed));
  	TSCH_ASN_DIVISOR_INIT(tsch_black_hopping_sequence_length, sizeof(changed));
  	LOG_INFO("balcklist get blacklist: divisor_init val:%u\n",tsch_black_hopping_sequence_length.val);

  	LOG_INFO("array size : %u\n",sizeof(changed));
  	LOG_INFO("asn_ms1b_remainder : %u\n",tsch_black_hopping_sequence_length.asn_ms1b_remainder);

  	LOG_INFO("channel list first \n");
  	tsch_channel_print(0);

}

void
black_change(void){

	LOG_INFO("channel list change first \n");
	tsch_channel_print(1);

	memcpy(tsch_hopping_sequence, tsch_black_hopping_sequence, sizeof(tsch_black_hopping_sequence));
  	//TSCH_ASN_DIVISOR_INIT(tsch_hopping_sequence_length, sizeof(tsch_black_hopping_sequence));
  	LOG_INFO("balcklist change: divisor_init val:%u\n",tsch_hopping_sequence_length.val);
  	

  	LOG_INFO("asn_ms1b_remainder : %u\n",tsch_black_hopping_sequence_length.asn_ms1b_remainder);
  	LOG_INFO("channel list change after \n");
  	tsch_channel_print(1);
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