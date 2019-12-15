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