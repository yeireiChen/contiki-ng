/*
 * Copyright (c) 2019, National Taipei University of Technology. Taipei, Taiwan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file
 *         Simple-TASA is a CoAP interface of IEEE802.15.4 PHY, MAC (incl. TSCH) and RPL resources.
 *
 * \author White.Ho <t106598040@ntut.org.tw>
 *
 */

#ifndef __S_TASA_H__
#define __S_TASA_H__

#include "net/mac/tsch/tsch.h"
#include "s-tasa-conf.h"


struct tsch_new_schedule {
    uint8_t timeslot;
    uint8_t channel_offset;
    uint8_t link_option;
};

/* ADD slot / return 1 was success, return 0 was false. */
int s_tasa_add_slots_of_slotframe(uint16_t timeslot, uint16_t channel_offset, int slot_numbers, uint8_t linkoptions);

/* DEL slot / return 1 was success, return 0 was false. */
void s_tasa_del_slots_of_slotframe(void);

void s_tasa_wait_asn_update_schedule(uint32_t temp_asn);

void s_tasa_cache_schedule_table(uint8_t timeslot, uint8_t channel_offset, uint8_t link_option);

/* count down trigger to flush new schedule table */
void flash_new_schedule_table(void);

uint32_t getTempASN(void);

uint16_t * getTimeslots(void);

void first_created_self_packet(void);

int s_tasa_init(void);


#endif /* __S_TASA_H__ */