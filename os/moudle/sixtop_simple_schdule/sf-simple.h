/*
 * Copyright (c) 2016, Yasuyuki Tanaka
 * Copyright (c) 2016, Centre for Development of Advanced Computing (C-DAC).
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
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file
 *         A 6P Simple Schedule Function
 * \author
 *         Shalu R <shalur@cdac.in>
 *         Lijo Thomas <lijo@cdac.in>
 *         Yasuyuki Tanaka <yasuyuki.tanaka@inf.ethz.ch>
 */

#ifndef _SIXTOP_SF_SIMPLE_H_
#define _SIXTOP_SF_SIMPLE_H_

#include "net/linkaddr.h"

int sf_simple_add_links(linkaddr_t *peer_addr, uint8_t num_links);
int sf_simple_remove_links(linkaddr_t *peer_addr);

/*Only support one cell now*/
int sf_simple_realocate_links(linkaddr_t *peer_addr,uint16_t timeslot,uint16_t channel);
int sf_simple_remove_direct_link(linkaddr_t *peer_addr,uint16_t timeslot);
void sf_simple_switching_parent_callback(linkaddr_t *old_addr, linkaddr_t *new_addr,uint16_t default_tx_timeslot);

/*An interface for outer to set slotframe handle*/
int sf_set_slotframe_handle(uint16_t handle);
int sf_set_default_slot(uint16_t slot);

#define SF_LINK_MAINTAIN_PERIOD 60

#ifdef SF_CONF_SIX_TOP_SLOTFRAME_LENGTH
#define SF_SIX_TOP_SLOTFRAME_LENGTH SF_CONF_SIX_TOP_SLOTFRAME_LENGTH
#else
#define SF_SIX_TOP_SLOTFRAME_LENGTH 17
#endif

#define SF_SIMPLE_MAX_LINKS  3
#define SF_SIMPLE_SFID       0xf0
#define TIMEOUT_WAIT_FOR_RETRY_RANDOM 5
extern const sixtop_sf_t sf_simple_driver;

#endif /* !_SIXTOP_SF_SIMPLE_H_ */