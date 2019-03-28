/*Use to maintain child list for projects*/
#include "net/linkaddr.h"

struct child_node_s{
    linkaddr_t address;
    uint16_t slot_offset;
};

typedef struct child_node_s child_node;




void child_list_ini();
child_node *find_child(linkaddr_t *address);
child_node *child_list_add_child(const linkaddr_t *address);
int child_list_remove_child(child_node *node);
int slot_is_used(uint16_t slot_offset);
int child_list_set_child_slot_offset(child_node *node,uint16_t slot_offset);