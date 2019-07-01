/**
 * \file
 *      Bcollect resource
 * \author
 *      White_TASA_Centralized
 */

#include <string.h>
//#include "rest-engine.h"
#include "coap-engine.h"

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
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP


static void res_post_slotframe_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

PARENT_RESOURCE(res_slotframe,                /* name */
                  "title=\"Slotframe\";obs",  /* attributes */
                  NULL,                       /*GET handler*/
                  res_post_slotframe_handler, /*POST handler*/
                  NULL,                       /*PUT handler*/
                  NULL);                      /*DELETE handler*/

static const uint16_t slotframe_handle = 1;

/* Used for control the slotframe */
static void
res_post_slotframe_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  const char *asn_c = NULL;
  const char *option_c = NULL;
  const uint8_t *request_content = NULL;
  char *pch;
  int index = 0;
  uint8_t option = 0;
  uint32_t temp_asn = 0;


  // post query asn number to control slotframe
  if(coap_get_query_variable(request, "asn", &asn_c)) {
    temp_asn = (uint32_t)atoi(asn_c);
    printf("IN Slotframe ASN : %lu \n",temp_asn);
#if WITH_CENTRALIZED_TASA
    TSCH_S_TASA_WAIT_ASN_UPDATE_SCHEDULE(temp_asn);
#endif 
  }
  // option can control add or delete.
  // 1 = ADD , 2 = DEL
  if(coap_get_query_variable(request, "option", &option_c)) {
    option = (uint8_t)atoi(option_c);
    printf("Got the Option : %u \n",option);
    if (option == 2) {
#if WITH_CENTRALIZED_TASA && 0
      TSCH_S_TASA_DEL_SLOT();
#endif
    }
  }

  unsigned int accept = -1;
  coap_get_header_accept(request, &accept);
  if (accept == -1) {
    coap_get_payload(request, &request_content);
    printf("requeset_content : %s \n", (char *)request_content);
    printf("address : %d \n", &request_content);
    // uint8_t temp_buffer = 0;
    // memcpy(temp_buffer, request_content, sizeof())
#if WITH_CENTRALIZED_TASA
    TSCH_S_TASA_CACHE_SCHEDULE_TABLE((uint8_t *)request_content);
#endif /*WITH_CENTRALIZED_TASA*/

#if WITH_CENTRALIZED_TASA
    pch = strtok((char *)request_content, "[':] ");
    uint8_t slot_s=0;
    uint8_t channel_c=0;
    uint8_t link_l=0;
    while(pch != NULL) {
      /* Slot Offset */
      if(index % 3 == 0) {
        printf("Slot offset : %s ,",pch);
        slot_s = (uint8_t)atoi(pch);
      }
      /* Channel Offset */
      if(index % 3 == 1) {
        printf("Channel offset : %s ,",pch);
        channel_c = (uint8_t)atoi(pch);
      }
      /* Link Options */
      if(index % 3 == 2) {
        printf("Link Options : %s .\n",pch);
        if (strncmp(pch, "TX", 2) == 0) {link_l = 0x01;}
        else {link_l = 0x02;}
        printf("Send Out Slot:%u Channel:%u Link:%u \n",slot_s, channel_c, link_l);


        TSCH_S_TASA_ADDED_SLOT(slot_s, channel_c, 1, link_l);

      }
      index++;
      pch = strtok(NULL, "[':] ");
    }
#endif /*WITH_CENTRALIZED_TASA*/
    tsch_schedule_print();
  }
}