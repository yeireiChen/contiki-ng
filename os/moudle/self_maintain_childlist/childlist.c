/*Use to maintain child list for projects*/

#include "childlist.h"
//#include "lib/list.h"
#include "lib/memb.h"
#include "net/linkaddr.h"
#include "net/nbr-table.h"
#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "IPv6"
#define LOG_LEVEL LOG_LEVEL_IPV6

MEMB(child,child_node,NBR_TABLE_MAX_NEIGHBORS);
//LIST(child_list);

void child_list_ini(){
    memb_init(&child);
    head.next=NULL;
}
/*------------------------------------------------------------------------*/
/*---------------Link list------------------------------------------------*/
/*------------------------------------------------------------------------*/
child_node *child_list_head(){
    return head.next;
}
child_node *child_list_next(child_node *node){
    return node->next;
}
void child_list_push(child_node *node){
    child_node* current_head = head.next;
    head.next = node;
    node->next = current_head;
}
 void child_list_remove(child_node *node){
    child_node *current_node;
    child_node *privious_node = &head;

    for(current_node = child_list_head();current_node!= NULL;current_node=child_list_next(current_node)){
        if(current_node == node){
             privious_node->next = node->next;
             break;
        }
        privious_node = current_node;
    }
}

void print_child_list(){
    child_node *current_node;
    LOG_INFO_("\nchild list: ");
    for(current_node = child_list_head();current_node!= NULL;current_node=child_list_next(current_node)){
        LOG_INFO_LLADDR(&current_node->address);
        LOG_INFO_(" ");
    }
}
/*------------------------------------------------------------------------*/
child_node *find_child(const linkaddr_t *address){
    child_node *node;
    for(node=child_list_head();node!=NULL;node=child_list_next(node)){
        if(linkaddr_cmp(address,&node->address)){
            return node;
        }
    }
    return NULL;
}

child_node *child_list_add_child(const linkaddr_t *address){
    child_node *node;
    node = find_child(address);
    print_child_list();
    if( node && child_list_remove_child(node)){
        node = NULL;
    }
    print_child_list();
    if(node==NULL){
        node = memb_alloc(&child);
        LOG_INFO_("\nallocate! ");
        if(node!=NULL){
           
            LOG_INFO_("\nchild add 0: ");
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
            child_list_push(node);
            LOG_INFO_("child add 2: ");
            LOG_INFO_LLADDR(address);
            LOG_INFO_(" ");
            LOG_INFO_LLADDR(&node->address);
            LOG_INFO_("\n");
            return node;
        }
    }
    return NULL;
}

int child_list_remove_child(child_node *node){
    child_list_remove(node);
    return memb_free(&child,node)==0?1:0;
}

int slot_is_used(uint16_t slot_offset){
    child_node *node;
    for(node=child_list_head();node!=NULL;node=child_list_next(node)){
        if(node->slot_offset == slot_offset){
            return 1;
        }
    }
    return 0;
}

int child_list_set_child_offsets(child_node *node,uint16_t slot_offset,uint16_t channel_offset)
{
    if(node != NULL){
        node -> slot_offset = slot_offset;
        node -> channel_offset = channel_offset;
        return 1;
    }
    return 0;
}