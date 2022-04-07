#include <stddef.h>
#include "flist.h"



void map_init(struct map* m)
{
    list_init( &(m->content));
    m->next_key = 2;
}

key_t map_insert(struct map* m, value_t v)
{
    struct association* ass = (struct association*)malloc(sizeof(struct association));
    ass->value = v;
    ass->key = m->next_key;
    m->next_key++;
    
    list_insert(&(m->content.tail), &(ass->elem));
    return ass->key;
}

value_t map_find(struct map* m, key_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct association *ass = list_entry(e, struct association, elem);
          if (ass->key == k)
            return ass->value;
        }
    return NULL;
    //Retunera null eller värdet på nyckeln
}

value_t map_remove(struct map* m, key_t k)
{
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct association *ass = list_entry(e, struct association, elem);
          if (ass->key == k)
            {
                value_t return_value = ass->value;
                list_remove(e);      //Peka om pekarna
                
                if(ass != NULL)    
                    free(ass);           //Avallokera minne
                return return_value; //returnera värdet
            }
        }
    return NULL;
}

void map_for_each(struct map* m,
void (*exec)(key_t k, value_t v, int aux),
int aux)
{
    struct list_elem *e;
    for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct association *ass = list_entry(e, struct association, elem);
          exec(ass->key, ass->value, aux);
        }
}

void map_remove_if(struct map* m,
bool (*cond)(key_t k, value_t v, int aux),
int aux)
{
    struct list_elem *e;
    for (e = list_begin(&(m->content)); e != list_end (&(m->content));)
        {
          struct association *ass = list_entry(e, struct association, elem);
          if(cond(ass->key, ass->value, aux))
          {
              
                e = list_remove(e);      //Peka om pekarna
                /*if(sizeof(*(ass->value)) != 0)
                   free(ass->value);*/      
                if(ass != NULL)    
                    free(ass);       //Avallokera minne
          }
          else
            e = list_next (e);
                
        }   
}

value_t pop_front(struct map* m)
{
  struct list_elem *e;

  if(list_empty(&(m->content)))
    return NULL;
  
  e = list_pop_front(&(m->content));
  return list_entry(e, struct association, elem)->value; 
}