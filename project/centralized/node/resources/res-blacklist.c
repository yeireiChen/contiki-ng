/**
 * \file
 *      Bcollect resource
 * \author
 *      Randy_BlackList
 */

#include <string.h>
#include <stdio.h>
//#include "rest-engine.h"
#include "coap-engine.h"
//#include "coap.h"

#include "net/ipv6/uip.h"
#include "net/routing/rpl-lite/rpl.h"
#include "net/routing/rpl-lite/rpl-icmp6.h"
#include "net/routing/rpl-lite/rpl-types.h"
#include "net/routing/rpl-lite/rpl-dag.h"
#include "net/routing/routing.h"
#include "net/link-stats.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-slot-operation.h"
#include "os/services/blacklist/blacklist.h"

#include "../../tsch-project-conf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "BlackApp"
#define LOG_LEVEL LOG_LEVEL_DBG


static void res_post_blacklist_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);



PERIODIC_RESOURCE(res_blacklist,
                  "title=\"Blacklist\";obs",
                  res_get_handler,
                  res_post_blacklist_handler,
                  NULL,
                  NULL,
                  1 * CLOCK_SECOND,
                  res_periodic_handler);

static const uint16_t blacklist_handle = 1;
static uint32_t event_counter = 0;

#if CONTIKI_TARGET_COOJA
/* inter-packet time we generate a packet to send to observer */
static uint8_t event_threshold = 20;
#else
/* inter-packet time we generate a packet to send to observer */
static uint8_t event_threshold = 140;
#endif

/* record last change event threshold's event_counter */
//static uint32_t event_threshold_last_change = 0;

/* Record the packet have been generated. (Server perspective) */
static uint32_t packet_counter = 0;


static uint8_t packet_priority = 1;

static uint8_t local_queue = 3;

#include "net/mac/tsch/tsch.h"
extern struct tsch_asn_t tsch_current_asn;

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  uint8_t *channeltemp;
  uint16_t *txTemp, *taAckTemp;
  uint8_t counterE = 0;
  uint16_t counterS = 0;
  uint8_t phyRange = 11;

  struct 
  {
    // 32bits to 1 block
    uint8_t flag[2];  // 0 1
    uint8_t priority; // 2
    uint8_t channelSize; // 3
    uint32_t start_asn; // 4 5 6 7
    uint32_t end_asn; // 8 9 10 11
    uint8_t channel[8]; // 12 13 14 15
    uint16_t channelTx[8];
    uint16_t channelTxAck[8];
    unsigned char parent_address[2];
    uint8_t end_flag[2]; // 36 37
  } message;

  memset(&message, 0, sizeof(message));

  message.flag[0] = 0x54;
  message.flag[1] = 0x66;
  message.end_flag[0] = 0xf0;
  message.end_flag[1] = 0xff;

  //message.local_queue = local_queue;
  message.start_asn = tsch_current_asn.ls4b;

  // for priority
  message.priority = packet_priority;
  message.channelSize = (uint8_t)channel_size();
  LOG_INFO("channel size is %u\n",(uint8_t)channel_size());

  //uint8_t packet_length = 0;
  rpl_dag_t *dag;
  rpl_parent_t *preferred_parent;
  linkaddr_t parent;
  linkaddr_copy(&parent, &linkaddr_null);
  //const struct link_stats *parent_link_stats;


  //PRINTF("I am B_collect res_get hanlder!\n");
  coap_set_header_content_format(response,APPLICATION_OCTET_STREAM);
  coap_set_header_max_age(response, res_blacklist.periodic->period / CLOCK_SECOND);

  
  dag = rpl_get_any_dag();
  if(dag != NULL) {
    preferred_parent = dag->preferred_parent;
    if(preferred_parent != NULL) {
      uip_ds6_nbr_t *nbr;
      nbr = uip_ds6_nbr_lookup(rpl_parent_get_ipaddr(preferred_parent));
      if(nbr != NULL) {
        /* Use parts of the IPv6 address as the parent address, in reversed byte order. */
        parent.u8[LINKADDR_SIZE - 1] = nbr->ipaddr.u8[sizeof(uip_ipaddr_t) - 2];
        parent.u8[LINKADDR_SIZE - 2] = nbr->ipaddr.u8[sizeof(uip_ipaddr_t) - 1];
      }
    }
    //message.rank = dag->rank;
  }
  message.parent_address[0] = parent.u8[LINKADDR_SIZE - 1];
  message.parent_address[1] = parent.u8[LINKADDR_SIZE - 2];


  channeltemp = channels();
  for(counterE=0;counterE<message.channelSize;counterE++){
      message.channel[counterE] = *(channeltemp+counterE);
      LOG_INFO("channel[%u] : %u\n",counterE,message.channel[counterE]);
  }

  txTemp = txCount();
  for(counterS=0;counterS<message.channelSize;counterS++){
      //phyRange
      uint8_t temp;
      temp = message.channel[counterS] - phyRange;
      message.channelTx[counterS] = (uint16_t)*(txTemp+temp);
      LOG_INFO("channeltx[%u] : %u\n",counterS,message.channelTx[counterS]);
  }

  taAckTemp = txAckCount();
  for(counterS=0;counterS<message.channelSize;counterS++){
      uint8_t temp;
      temp = message.channel[counterS] - phyRange;
      message.channelTxAck[counterS] = (uint16_t)*(taAckTemp+temp);
      LOG_INFO("channelack[%u] : %u\n",counterS,message.channelTxAck[counterS]);
  }


  memcpy(buffer, &message, sizeof(message));

  coap_set_uip_traffic_class(packet_priority);
  coap_set_uip_stasa(local_queue);
  coap_set_payload(response, buffer, sizeof(message));

  
  LOG_INFO("want here,%u\n",sizeof(message));
}

/* Used for control the slotframe */
static void
res_post_blacklist_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *asn_t = NULL;
  const uint8_t *request_content = NULL;
  uint32_t asn = 0;

  if(coap_get_query_variable(request, "asn", &asn_t)) {
    asn = (uint32_t)atoi(asn_t);
  }

  unsigned int accept = -1;
  coap_get_header_accept(request, &accept);
  if (accept == -1) {
    //LOG_INFO("it's here\n");
    coap_get_payload(request, &request_content);
    black_changeChannel(request_content);
  }
  //LOG_INFO("%s\n",request_content);

  

  TSCH_BLACK_WAIT_CHANGE(asn);
}

/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_handler()
{
  // This periodic handler will be called every second 
  ++event_counter;

  // Will notify subscribers when inter-packet time is match 
  if(event_counter % event_threshold == 0) {
#if WITH_CENTRALIZED_TASA
    if (coap_has_observers("res/blacklist")) {
      if (tsch_get_schedule_table_event() || 1) {
        //PRINTF("Numbers of Observe : %u \n", coap_has_observer_numbers());
        ++packet_counter;
        //LOG_INFO("Generate a new packet! , %08x. \n",tsch_current_asn.ls4b);
        // Notify the registered observers which will trigger the res_get_handler to create the response. 
        coap_notify_observers(&res_blacklist);
      
      }
    }
#else
    ++packet_counter;
    //PRINTF("Generate a new packet! , %08x. \n",tsch_current_asn.ls4b);
    // Notify the registered observers which will trigger the res_get_handler to create the response. 
    coap_notify_observers(&res_blacklist);
#endif // WITH_CENTRALIZED_TASA 
  }
}