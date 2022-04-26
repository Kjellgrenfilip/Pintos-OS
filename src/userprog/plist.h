#ifndef _PLIST_H_
#define _PLIST_H_

#include "../lib/kernel/list.h"
#include <stdlib.h>
#include "../threads/synch.h"

/* Place functions to handle a running process here (process list).
   
   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process inforamtion (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.
     
   - A function that print the entire content of the list in a nice,
     clean, readable format.
     
 */
typedef struct process_info* value_p;
typedef int pid_t;


struct process_info
{
  pid_t parent_id;
  int status_code;
  bool alive;
  bool parent_alive;
  struct semaphore sema;
};

struct p_association
{
    pid_t pid;
    value_p value;
    struct list_elem elem;  //Previous and next pointers
};

struct p_map
{
    int size;
    struct list content;    //Head and tail
    int next_pid;
};

void process_list_init(struct p_map* m);

pid_t process_list_insert(struct p_map* m, value_p v, pid_t pid);
value_p process_list_find(struct p_map*, pid_t k);
value_p process_list_remove(struct p_map* m, pid_t k);
void process_list_print(struct p_map*);
void set_parent_dead(struct p_map* m, pid_t curr);

int process_list_size(struct p_map*);

#endif
