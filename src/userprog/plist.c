#include <stddef.h>
#include <stddef.h>
#include "plist.h"


void process_list_init(struct p_map* m)
{
    list_init( &(m->content));
    m->next_pid = 2;
    m->size = 0;
}

pid_t process_list_insert(struct p_map* m, value_p v)
{
    struct p_association* ass = (struct p_association*)malloc(sizeof(struct p_association));
    ass->value = v;
    ass->pid = m->next_pid;
    m->next_pid++;
    
    m->size++;

    list_insert(&(m->content.tail), &(ass->elem));

    return ass->pid;
}

value_p process_list_find(struct p_map* m, pid_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct p_association *ass = list_entry(e, struct p_association, elem);
          if (ass->pid == k)
            return ass->value;
        }
    return NULL;
    //Retunera null eller värdet på nyckeln
}

value_p process_list_remove(struct p_map* m, pid_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct p_association *ass = list_entry(e, struct p_association, elem);
          if (ass->pid == k)
            {
                value_p return_value = ass->value;
                list_remove(e);      //Peka om pekarna

                m->size--;

                if(ass != NULL)    
                    free(ass);           //Avallokera minne
                return return_value; //returnera värdet
            }
        }
    return NULL;
}



int process_list_size(struct p_map* m)
{
  return m->size;
}

