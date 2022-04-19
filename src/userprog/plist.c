#include <stddef.h>
#include <stddef.h>
#include "plist.h"


void process_list_init(struct map* m)
{
    list_init( &(m->content));
    m->next_pid = 2;
    m->size = 0;
}

pid_t process_list_insert(struct map* m, value_p v)
{
    struct association* ass = (struct association*)malloc(sizeof(struct association));
    ass->value = v;
    ass->pid = m->next_pid;
    m->next_pid++;
    
    m->size++;

    list_insert(&(m->content.tail), &(ass->elem));

    return ass->pid;
}

value_p process_list_find(struct map* m, pid_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct association *ass = list_entry(e, struct association, elem);
          if (ass->pid == k)
            return ass->value;
        }
    return NULL;
    //Retunera null eller värdet på nyckeln
}

value_p process_list_remove(struct map* m, pid_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct association *ass = list_entry(e, struct association, elem);
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



int process_list_size(struct map* m)
{
  return m->size;
}

