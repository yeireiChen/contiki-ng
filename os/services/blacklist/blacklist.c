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
#define LOG_LEVEL LOG_LEVEL_MAC


void 
black_changeChannel(uint8_t ch1, uint8_t ch2){

	uint8_t changed[] = {ch1,ch2};

	/* Initialize blacklist hopping sequence test*/
  	memcpy(tsch_black_hopping_sequence, changed, sizeof(changed));
  	TSCH_ASN_DIVISOR_INIT(tsch_black_hopping_sequence_length, sizeof(changed));

}