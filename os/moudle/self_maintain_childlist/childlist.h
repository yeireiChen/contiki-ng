/*Use to maintain child list for projects*/
#include "net/linkaddr.h"

struct child_node_s{
    linkaddr_t address;
    uint16_t slot_offset;
    uint16_t channel_offset;
    struct child_node_s *next;
};

typedef struct child_node_s child_node;

child_node head;


void child_list_ini();
void print_child_list();
child_node *find_child(const linkaddr_t *address);
child_node *child_list_add_child(const linkaddr_t *address);
child_node *find_dupilcate_used_slot();
int child_list_remove_child(child_node *node);
int slot_is_used(uint16_t slot_offset);
int exclude_node_slot_is_used(child_node *exclude,uint16_t slot_offset);
int exclude_lladdr_slot_is_used(const linkaddr_t *exclude,uint16_t slot_offset);
int child_list_set_child_offsets(child_node *node,uint16_t slot_offset,uint16_t channel_offset);