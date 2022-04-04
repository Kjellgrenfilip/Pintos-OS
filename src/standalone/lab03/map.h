/* do not forget the guard against multiple includes */
#pragma once
#include "list.h"

typedef char* value_t;
typedef int key_t;

struct association
{
    key_t key;
    value_t value;
    struct list_elem elem;  //Previous and next pointers
};



struct map
{
    struct list content;    //Head and tail
    int next_key;
};

void map_init(struct map* m);

key_t map_insert(struct map* m, value_t v);
value_t map_find(struct map*, key_t k);
value_t map_remove(struct map* m, key_t k);
void map_for_each(struct map* m, void (*exec)(key_t k, value_t v, int aux), int aux);
void map_remove_if(struct map* m, bool (*cond)(key_t k, value_t v, int aux), int aux);