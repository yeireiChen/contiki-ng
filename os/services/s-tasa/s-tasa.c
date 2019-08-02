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
 *         TASA is a CoAP interface of IEEE802.15.4 PHY, MAC (incl. TSCH) and RPL resources.
 *
 * \author White.Ho <t106598040@ntut.org.tw>
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/memb.h"
#include "lib/list.h"
#include "contiki.h"
#include "s-tasa.h"
#include "net/packetbuf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "TSCH TASA"
#define LOG_LEVEL LOG_LEVEL_MAC

#if ROUTING_CONF_RPL_LITE
#include "net/routing/rpl-lite/rpl.h"
#elif ROUTING_CONF_RPL_CLASSIC
#include "net/routing/rpl-classic/rpl.h"
#endif

#define LENGTH 20

static const uint16_t slotframe_handle = 1; /* for tasa of slotframe */

static int save_slot_numbers = 0;
static int number_count = 0;
struct tsch_slotframe *sf_cent = NULL;

uint16_t slot_list[LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t backup_temp_asn = 0;

static int ID = 0;
MEMB(temp_sche_table_memb,tsch_new_table, 48);
//LIST(temp_sche_table_list);

int 
s_tasa_add_slots_of_slotframe(uint16_t timeslot, uint16_t channel_offset, int slot_numbers, uint8_t linkoptions)
{
  int i = 0;
  
  if (sf_cent) {
    // tsch_schedule_remove_slotframe(sf_cent);
    // LOG_INFO("Remove other tasa scheduling...");
  } else {
    sf_cent = tsch_schedule_add_slotframe(slotframe_handle, TSCH_SCHEDULE_DEFAULT_LENGTH);
    LOG_INFO("Adding tasa schedule\n");
    //tsch_schedule_print();
  }

  if (linkoptions == 0x01) {
    for (i=0; i<LENGTH; i++) {
      if (slot_list[i] == 0) {
        if (slot_list[i-1] == timeslot) break; // if retransmit packet, just break,
        slot_list[i] = timeslot;
        LOG_INFO("add slot_list[ %d ] the value = %u\n",i,timeslot);
        ++number_count;
        break;
      }
    }
    // tx_slot = InsertTXslot(tx_slot, NULL, timeslot);
  }

  if(sf_cent != NULL && timeslot > 0x00){
    if(!tsch_is_locked()){
      for(i=0 ; i<slot_numbers ; i++){
        tsch_schedule_add_link(sf_cent,
        linkoptions,
        LINK_TYPE_NORMAL, &tsch_broadcast_address,
        timeslot+i, channel_offset);
        LOG_INFO("TSCH add link %u to current slotframe was successful.\n", linkoptions);
      }
      return 1;
    }else {
      LOG_INFO("TSCH was locking.\n");
    }
  }else {
    LOG_INFO("Can't find current slotframe.\n");
  }
  //tsch_schedule_print();
  return 0;

}

void
s_tasa_del_slots_of_slotframe()
{

  int i = 0;
  for(i=0; i<LENGTH; i++){
    slot_list[i] = 0;
  }

  if (sf_cent) {
    if(!tsch_is_locked()) {
      tsch_schedule_remove_slotframe(sf_cent);
      LOG_INFO("Remove the slotframe : %s \n",tsch_schedule_remove_slotframe(sf_cent)? "YES": "NO" );
      sf_cent = NULL;
    } else {
      LOG_INFO("TSCH was locking.\n");
    }
  }else {
    LOG_INFO("Slot frame is NULL. \n");
  }
  LOG_INFO("TSCH schedule removing\n");
  
}

void 
s_tasa_cache_schedule_table(uint8_t timeslot, uint8_t channel_offset, uint8_t linkoptions)
{
  ID = ID + 1;
  tsch_new_table *table;
  table = table_list_add_child(ID);
  if (table) {
    if(sch_table_list_set_query(table, timeslot, channel_offset, linkoptions)) {
      LOG_INFO("Timeslot : %u ,",timeslot);
      LOG_INFO("Channel_offset : %u ,",channel_offset);
      LOG_INFO("linkoptions : %u .\n",linkoptions);
    }
  }
}

void 
flash_new_schedule_table(int flag)
{
  tsch_new_table *table;
  table = find_table(ID);
  if(flag) s_tasa_del_slots_of_slotframe();
  while (table != NULL) {
    LOG_INFO("Timeslot : %u ,",table->timeslot);
    LOG_INFO("Channel_offset : %u ,",table->channel_offset);
    LOG_INFO("linkoptions : %u .\n",table->linkoptions);
    //LOG_INFO("flush_temp_sch_table data : %s \n", (char *)table->sche_payload);
    s_tasa_add_slots_of_slotframe(table->timeslot, table->channel_offset, 1,table->linkoptions);
    table_list_remove(table);
    ID = ID - 1;
    table = find_table(ID);
  }
}

uint16_t *
getTimeslots() 
{
  return slot_list;
}

void
first_created_self_packet(void)
{
  LOG_INFO("Reset save_slot_numbers to 0 ! \n");
  save_slot_numbers = 0;
}

void 
s_tasa_wait_asn_update_schedule(uint32_t temp_asn)
{
  backup_temp_asn = temp_asn;
  LOG_INFO("Got ASN IN s-tasa : %lu\n", backup_temp_asn);
}

uint32_t
getTempASN(void)
{ 
  uint32_t temp_asn = backup_temp_asn;
  backup_temp_asn = 0;
  return temp_asn;
}

tsch_new_table *sch_table_list_head() {
  return head.next;
}

tsch_new_table *sch_table_list_next(tsch_new_table *table) {
  return table->next;
}

void sch_table_list_push(tsch_new_table *table) {
  tsch_new_table* current_head = head.next;
  head.next = table;
  table->next = current_head;
}

void sch_table_list_remove(tsch_new_table * table) {
  tsch_new_table* current_table;
  tsch_new_table* privious_table = &head;

  for (current_table = sch_table_list_head(); 
        current_table != NULL;
        current_table = sch_table_list_next(current_table) ) {

          if(current_table == table) {
            privious_table->next = current_table->next;
            break;
          }
          privious_table = current_table;
        }
}

tsch_new_table *find_table(int ID){
  tsch_new_table *table;
  for(table = sch_table_list_head();
      table != NULL;
      table = sch_table_list_next(table)) {
        if(ID == table->ID) {
          return table;
        }
      }
  return NULL;
}

tsch_new_table *table_list_add_child(int ID) {
  tsch_new_table *table;
  table = find_table(ID);
  if (table && table_list_remove(table)) {
    table = NULL;
  }

  if(table == NULL) {
    table = memb_alloc(&temp_sche_table_memb);
    LOG_INFO("Allocate new memb \n");

    if(table) {
      table->ID = ID;
      //strcpy((int *)table->ID, (const int *)ID);
      // linkaddr_copy(&table->address, address);
      sch_table_list_push(table);
      return table;
    }
  }
  return NULL;
}

int table_list_remove(tsch_new_table* table){
  sch_table_list_remove(table);
  return memb_free(&temp_sche_table_memb, table) == 0? 1:0;
}

int sch_table_list_set_query(tsch_new_table *table, uint8_t timeslot, uint8_t channel_offset, uint8_t linkoptions){
  if (table) {
    table->timeslot = timeslot;
    table->channel_offset = channel_offset;
    table->linkoptions =linkoptions;
    //table->sche_payload = payload_data;
    return 1;
  }
  return 0;
}

int
s_tasa_init()
{
  memb_init(&temp_sche_table_memb);
  head.next = NULL;
  return 1;
}