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
#include "net/mac/tsch/tsch-schedule.h"

#include "../../tsch-project-conf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "checkApp"
#define LOG_LEVEL LOG_LEVEL_DBG

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

PERIODIC_RESOURCE(res_check,
                  "title=\"check resourse\";obs",
                  res_get_handler,
                  res_post_handler,
                  NULL,
                  NULL,
                  1 * CLOCK_SECOND,
                  res_periodic_handler);

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_periodic_handler() or PUT or POST.
 */
static uint32_t event_counter = 0;

#if CONTIKI_TARGET_COOJA
/* inter-packet time we generate a packet to send to observer */
static uint8_t event_threshold = 20;
#else
/* inter-packet time we generate a packet to send to observer */
static uint8_t event_threshold = 140;
#endif

/* Record the packet have been generated. (Server perspective) */
static uint32_t packet_counter = 0;

#include "net/mac/tsch/tsch.h"
extern struct tsch_asn_t tsch_current_asn;



static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  /*
   * For minimal complexity, request query and options should be ignored for GET on observable resources.
   * Otherwise the requests must be stored with the observer list and passed by REST.notify_subscribers().
   * This would be a TODO in the corresponding files in contiki/apps/erbium/!
   */

   //REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "[Collect] ec: %lu, et: %lu, lc, %lu, pc: %lu", event_counter, event_threshold, event_threshold_last_change,packet_counter));

  /* The REST.subscription_handler() will be called for observable resources by the REST framework. */
}


/* Used for update the threshold */
static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  
}

/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_handler()
{
  /* This periodic handler will be called every second */
  ++event_counter;

  /* Will notify subscribers when inter-packet time is match */
  if(event_counter % event_threshold == 0) {
    LOG_PRINT("Generate a new packet! , %08x. \n",tsch_current_asn.ls4b);
    //tsch_schedule_print();
    tsch_channel_print();
    ++packet_counter;
    //PRINTF("Generate a new packet! , %08x. \n",tsch_current_asn.ls4b);
    /* Notify the registered observers which will trigger the res_get_handler to create the response. */
    coap_notify_observers(&res_check);
  }
}