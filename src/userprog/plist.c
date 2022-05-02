#include <stddef.h>
#include "plist.h"

static struct lock p_lock;

void process_list_init(struct p_map* m)
{
    list_init( &(m->content));
    m->next_pid = 2;
    m->size = 0;
    lock_init(&p_lock);
}

pid_t process_list_insert(struct p_map* m, value_p v, pid_t pid)
{
    lock_acquire(&p_lock);
    struct p_association* ass = (struct p_association*)malloc(sizeof(struct p_association));
    sema_init(&(v->sema), 0);
    ass->value = v;
    ass->pid = pid;
    m->size++;

    list_insert(&(m->content.tail), &(ass->elem));
    lock_release(&p_lock);
    return ass->pid;
}

value_p process_list_find(struct p_map* m, pid_t k)
{
    lock_acquire(&p_lock);
    struct list_elem *e;

      for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
        {
          struct p_association *ass = list_entry(e, struct p_association, elem);
          if (ass->pid == k)
          {
            lock_release(&p_lock);
            return ass->value;
          }
            
        }
    lock_release(&p_lock);
    return NULL;
    //Retunera null eller värdet på nyckeln
}

value_p process_list_remove(struct p_map* m, pid_t k)
{
    lock_acquire(&p_lock);
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
          lock_release(&p_lock);
          return return_value; //returnera värdet
      }
    }
    lock_release(&p_lock);
    return NULL;
}

int process_list_size(struct p_map* m)
{
  return m->size;
}

void process_list_print(struct p_map* m)
{
  struct list_elem *e;
  printf("PID   P_PID    STATUS    ALIVE   P_ALIVE:\n");
  for (e = list_begin(&(m->content)); e != list_end (&(m->content)); e = list_next (e))
  {
    struct p_association *ass = list_entry(e, struct p_association, elem);
    printf("%-3d   %-5d    %-6d    %-5d   %d \n", ass->pid, ass->value->parent_id, ass->value->status_code, ass->value->alive, ass->value->parent_alive);
  }
}

void set_dead_and_clean(struct p_map* m, pid_t curr)
{
  lock_acquire(&p_lock);    //Tillåt inte flera processer här samtidigt
  struct list_elem* e;
  for (e = list_begin(&(m->content)); e != list_end (&(m->content));)
  {
    struct p_association *ass = list_entry(e, struct p_association, elem);
    if (ass->value->parent_id == curr)
    {
      ass->value->parent_alive = false;
      
    }
    if(ass->value->alive == false && ass->value->parent_alive == false)
    {
      e = list_remove(e);
      free(ass->value);
      free(ass);        
      continue;
    }
    e = list_next(e);
  }
  lock_release(&p_lock);
}
