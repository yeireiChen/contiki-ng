/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) CoAP Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"

#if PLATFORM_SUPPORTS_BUTTON_HAL
#include "dev/button-hal.h"
#else
#include "dev/button-sensor.h"
#endif

#include "sys/node-id.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"
#include "net/mac/tsch/tsch.h"
#include "net/routing/routing.h"

#define DEBUG DEBUG_PRINT
#include "net/ipv6/uip-debug.h"


/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP
/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern coap_resource_t
  res_hello,
  res_separate,
  res_push,
  res_event,
  res_toggle,
  res_bcollect,
  res_bcollect_2;
  
PROCESS(er_example_server, "Erbium Example Server");
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&er_example_server,&node_process);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  LOG_INFO("Starting Erbium Example Server\n");

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
  coap_activate_resource(&res_hello, "test/hello");
  coap_activate_resource(&res_separate, "test/separate");
  coap_activate_resource(&res_push, "test/push");
  #if PLATFORM_HAS_BUTTON
  coap_activate_resource(&res_event, "sensors/button");
  #endif /* PLATFORM_HAS_BUTTON */
  coap_activate_resource(&res_toggle, "actuators/toggle");
  coap_activate_resource(&res_bcollect, "g/bcollect");
  coap_activate_resource(&res_bcollect_2, "g/bcollect_2");


  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
#if PLATFORM_HAS_BUTTON
#if PLATFORM_SUPPORTS_BUTTON_HAL
    if(ev == button_hal_release_event) {
#else
    if(ev == sensors_event && data == &button_sensor) {
#endif
      LOG_DBG("*******BUTTON*******\n");

      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      res_separate.resume();
    }
#endif /* PLATFORM_HAS_BUTTON */
  }                             /* while (1) */

  PROCESS_END();
}

#if 0
PROCESS_THREAD(node_process, ev, data)
{

  PROCESS_BEGIN();


  NETSTACK_MAC.on();

#if WITH_PERIODIC_ROUTES_PRINT
  {
    static struct etimer et;
    /* Print out routing tables every minute */
    etimer_set(&et, CLOCK_SECOND * 60);
    while(1) {
      /* Used for non-regression testing */
      #if (UIP_MAX_ROUTES != 0)
        PRINTF("Routing entries: %u\n", uip_ds6_route_num_routes());
      #endif
      #if (UIP_SR_LINK_NUM != 0)
        PRINTF("Routing links: %u\n", uip_sr_num_nodes());
      #endif
      PROCESS_YIELD_UNTIL(etimer_expired(&et));
      etimer_reset(&et);
    }
  }
#endif /* WITH_PERIODIC_ROUTES_PRINT */

  PROCESS_END();
}
#endif

PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer etaa;
  

  PROCESS_BEGIN();
  
  //etimer_set(&etaa, CLOCK_SECOND * 60);
  etimer_set(&etaa, CLOCK_SECOND * 5);
  while(1) {
    PROCESS_YIELD_UNTIL(etimer_expired(&etaa));
    etimer_reset(&etaa);
    //print_network_status();
    #if DEBUG
      print_tempAndhumi_status();
    #endif
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
