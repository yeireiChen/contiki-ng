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

#include "../../tsch-project-conf.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "BlackApp"
#define LOG_LEVEL LOG_LEVEL_DBG


static void res_post_blacklist_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

PARENT_RESOURCE(res_blacklist,                /* name */
                  "title=\"Blacklist\";obs",  /* attributes */
                  NULL,                       /*GET handler*/
                  res_post_blacklist_handler, /*POST handler*/
                  NULL,                       /*PUT handler*/
                  NULL);                      /*DELETE handler*/

static const uint16_t blacklist_handle = 1;

/* Used for control the slotframe */
static void
res_post_blacklist_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const char *channel1_t = NULL;
  const char *channel2_t = NULL;
  const char *channel3_t = NULL;
  uint8_t channel1 = 0;
  uint8_t channel2 = 0;
  uint8_t channel3 = 0;


  if(coap_get_query_variable(request, "first", &channel1_t)) {
    channel1 = (uint8_t)atoi(channel1_t);
  }

  if(coap_get_query_variable(request, "two", &channel2_t)) {
    channel2 = (uint8_t)atoi(channel2_t);
  }

  if(coap_get_query_variable(request, "three", &channel3_t)) {
    channel3 = (uint8_t)atoi(channel3_t);
  }

  LOG_PRINT(" ch1:%s, ch2:%s, ch3:%s\n",channel1_t,channel2_t,channel3_t);
  LOG_PRINT(" ch1:%u, ch2:%u, ch3:%u\n",channel1,channel2,channel3);

  TSCH_BLACK_CHANGE_CHANNEL(channel1,channel2);
}