#ifndef __BlACK_CONF_H__
#define __BlACK_CONF_H__


#include "net/mac/tsch/tsch.h"
#include "blacklist-conf.h"


/* change channel list*/
void black_changeChannel(const uint8_t *payload); //uint8_t ch1, uint8_t ch2,uint8_t ch3 -const uint8_t *payload

void black_change(void);
uint32_t getBlackASN(void);
void black_wait_asn_change(uint32_t temp_asn);
uint16_t channel_size(void);
uint8_t *channels(void);


#endif /* __BlACK_CONF_H__ */



