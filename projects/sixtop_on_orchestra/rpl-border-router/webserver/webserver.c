/*
 * Copyright (c) 2017, RISE SICS
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
 *
 */

 #include "contiki.h"
 #include "net/routing/routing.h"
 #include "net/ipv6/uip-ds6-route.h"
 #include "net/ipv6/uip-sr.h"
 
 #include <stdio.h>
 #include <string.h>
 
 /*---------------------------------------------------------------------------*/
 static const char *TOP = "<html>\n  <head>\n    <title>Contiki-NG</title>\n  </head>\n<body>\n";
 static const char *BOTTOM = "\n</body>\n</html>\n";
/*static const char *SCRIPT_0 ="\n<script>\nvar links=Array.from(document.getElementsByClassName('link'));\nvar tree=[];\nvar nodes=[];\nvar root=new Object;\n";
static const char *SCRIPT_1 ="root.node_addr=document.getElementById('root').innerHTML;\nroot.parent_addr ='';\nroot.child_list=[];\ntree.push(root);\n";
static const char *SCRIPT_2 ="nodes=links.map(link=>{var node = new Object();link_data=link.innerHTML.replace('(','').replace(')','').split(' ');\n";
static const char *SCRIPT_3 ="node.node_addr = link_data[0];node.parent_addr = link_data[2];node.child_list=[];return node;});tree=tree.concat(nodes);\n";
static const char *SCRIPT_4 ="for(i=0;i<tree.length;i++){for(j=0;j<tree.length;j++){if(tree[i].node_addr==tree[j].parent_addr){tree[i].child_list.push(tree[j]);}}}\n";
static const char *SCRIPT_5 ="var stack=[];var space_count=1;var current_node=[];var optStr=['Topology<br><br>'];stack.push(root);while(stack.length!=0){\n";
static const char *SCRIPT_6 ="temp=stack.pop();if(current_node[current_node.length-1]!=temp.parent_addr){current_node.pop();space_count--;}tempStr='';\n";
static const char *SCRIPT_7 ="for(i=0;i<space_count;i++){tempStr+='----';}tempStr+=temp.node_addr+'<br>';optStr.push(tempStr);\n";
static const char *SCRIPT_8 ="if(temp.child_list.length>0){space_count++;current_node.push(temp.node_addr);}for(i=0;i<temp.child_list.length;i++){\n";
static const char *SCRIPT_9 ="stack.push(temp.child_list[i]);}}optStr=optStr.join('');var topo=document.createElement('div');topo.innerHTML=optStr;\n";
static const char *SCRIPT_10 ="document.body.appendChild(topo);console.log(optStr,document.body);\n</script>";*/
 static char buf[256];
 static int blen;
 #define ADD(...) do {                                                   \
     blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
   } while(0)
 #define SEND(s) do { \
   SEND_STRING(s, buf); \
   blen = 0; \
 } while(0);
 
 /* Use simple webserver with only one page for minimum footprint.
  * Multiple connections can result in interleaved tcp segments since
  * a single static buffer is used for all segments.
  */
 #include "httpd-simple.h"
 
 /*---------------------------------------------------------------------------*/
 static void
 ipaddr_add(const uip_ipaddr_t *addr)
 {
   uint16_t a;
   int i, f;
   for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
     a = (addr->u8[i] << 8) + addr->u8[i + 1];
     if(a == 0 && f >= 0) {
       if(f++ == 0) {
         ADD("::");
       }
     } else {
       if(f > 0) {
         f = -1;
       } else if(i > 0) {
         ADD(":");
       }
       ADD("%x", a);
     }
   }
 }
 /*---------------------------------------------------------------------------*/
 static
 PT_THREAD(generate_routes(struct httpd_state *s))
 {
   static uip_ds6_nbr_t *nbr;
  
   PSOCK_BEGIN(&s->sout);
   SEND_STRING(&s->sout, TOP);

   ADD("  Root\n  <ul>\n");
   uip_ipaddr_t hostaddr;
   NETSTACK_ROUTING.get_root_ipaddr(&hostaddr);
   ADD("    <li id='root'>");
   ipaddr_add(&hostaddr);
   ADD("</li>\n");
   SEND(&s->sout);
   ADD("  </ul>\n");
   SEND(&s->sout);
   
   ADD("  Neighbors\n  <ul>\n");
   SEND(&s->sout);
   for(nbr = nbr_table_head(ds6_neighbors);
       nbr != NULL;
       nbr = nbr_table_next(ds6_neighbors, nbr)) {
     ADD("    <li>");
     ipaddr_add(&nbr->ipaddr);
     ADD("</li>\n");
     SEND(&s->sout);
   }
   ADD("  </ul>\n");
   SEND(&s->sout);
 
 #if (UIP_MAX_ROUTES != 0)
   {
     static uip_ds6_route_t *r;
     ADD("  Routes\n  <ul>\n");
     SEND(&s->sout);
     for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {
       ADD("    <li>");
       ipaddr_add(&r->ipaddr);
       ADD("/%u (via ", r->length);
       ipaddr_add(uip_ds6_route_nexthop(r));
       ADD(") %lus", (unsigned long)r->state.lifetime);
       ADD("</li>\n");
       SEND(&s->sout);
     }
     ADD("  </ul>\n");
     SEND(&s->sout);
   }
 #endif /* UIP_MAX_ROUTES != 0 */
 
 #if (UIP_SR_LINK_NUM != 0)
   if(uip_sr_num_nodes() > 0) {
     static uip_sr_node_t *link;
     ADD("  Routing links\n  <ul>\n");
     SEND(&s->sout);
     for(link = uip_sr_node_head(); link != NULL; link = uip_sr_node_next(link)) {
       if(link->parent != NULL) {
         uip_ipaddr_t child_ipaddr;
         uip_ipaddr_t parent_ipaddr;
 
         NETSTACK_ROUTING.get_sr_node_ipaddr(&child_ipaddr, link);
         NETSTACK_ROUTING.get_sr_node_ipaddr(&parent_ipaddr, link->parent);
 
         ADD("    <li class='link'>");
         ipaddr_add(&child_ipaddr);
 
         ADD(" (parent: ");
         ipaddr_add(&parent_ipaddr);
         ADD(") %us", (unsigned int)link->lifetime);
 
         ADD("</li>\n");
         SEND(&s->sout);
       }
     }
     ADD("  </ul>");
     SEND(&s->sout);
   }
 #endif /* UIP_SR_LINK_NUM != 0 */
   /*SEND_STRING(&s->sout, SCRIPT_0);
   SEND_STRING(&s->sout, SCRIPT_1);
   SEND_STRING(&s->sout, SCRIPT_2);
   SEND_STRING(&s->sout, SCRIPT_3);
   SEND_STRING(&s->sout, SCRIPT_4);
   SEND_STRING(&s->sout, SCRIPT_5);
   SEND_STRING(&s->sout, SCRIPT_6);
   SEND_STRING(&s->sout, SCRIPT_7);
   SEND_STRING(&s->sout, SCRIPT_8);
   SEND_STRING(&s->sout, SCRIPT_9);
   SEND_STRING(&s->sout, SCRIPT_10);*/
   SEND_STRING(&s->sout, BOTTOM);
   
   PSOCK_END(&s->sout);
 }
 /*---------------------------------------------------------------------------*/
 PROCESS(webserver_nogui_process, "Web server");
 PROCESS_THREAD(webserver_nogui_process, ev, data)
 {
   PROCESS_BEGIN();
 
   httpd_init();
 
   while(1) {
     PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
     httpd_appcall(data);
   }
 
   PROCESS_END();
 }
 /*---------------------------------------------------------------------------*/
 httpd_simple_script_t
 httpd_simple_get_script(const char *name)
 {
   return generate_routes;
 }
 /*---------------------------------------------------------------------------*/
 