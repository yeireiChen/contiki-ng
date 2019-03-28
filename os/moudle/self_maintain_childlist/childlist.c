/*Use to maintain child list for projects*/

#include "childlist.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "net/linkaddr.h"
#include "net/nbr-table.h"
#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "IPv6"
#define LOG_LEVEL LOG_LEVEL_IPV6

MEMB(child,child_node,NBR_TABLE_MAX_NEIGHBORS);
LIST(child_list);

void child_list_ini(){
    memb_init(&child);
    list_init(child_list);
}

child_node *find_child(const linkaddr_t *address){
    child_node *node;
    for(node=list_head(child_list);node!=NULL;node=list_item_next(node)){
        if(linkaddr_cmp(address,&node->address)){
            return node;
        }
    }
    return NULL;
}

child_node *child_list_add_child(const linkaddr_t *address){
    child_node *node;
    node = find_child(address);
    if(node==NULL){
        node = memb_alloc(&child);
        if(node!=NULL){
            memset(node, 0, sizeof(child_node));
             LOG_INFO_("child add 0: ");
            LOG_INFO_LLADDR(address);
            LOG_INFO_(" ");
            LOG_INFO_LLADDR(&node->address);
            LOG_INFO_("\n");
            linkaddr_copy(&node->address,address);
             LOG_INFO_("child add 1: ");
            LOG_INFO_LLADDR(address);
            LOG_INFO_(" ");
            LOG_INFO_LLADDR(&node->address);
            LOG_INFO_("\n");
            list_push(child_list,node);
            return node;
        }
    }
    return NULL;
}

int child_list_remove_child(child_node *node){
    list_remove(child_list,node);
     return memb_free(&child,node);
}

int slot_is_used(uint16_t slot_offset){
    child_node *node;
    for(node=list_head(child_list);node!=NULL;node=list_item_next(node)){
        if(node->slot_offset == slot_offset){
            return 1;
        }
    }
    return 0;
}

int child_list_set_child_slot_offset(child_node *node,uint16_t slot_offset)
{
    if(node != NULL){
        node -> slot_offset = slot_offset;
        return 1;
    }
    return 0;
}