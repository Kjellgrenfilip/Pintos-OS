#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:
   
   int sys_read_arg_count = argc[ SYS_READ ];
   
   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended, you may need to change the order of these two (plist, sleep) */
  0, 1
};

//File table i thread
//Stäng filer i process cleanup


static void
syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;    //Stackpekare

  struct map* file_table = &(thread_current()->file_table);   //Hämta pekare till file table från tråden
  
  switch ( esp[0] )
  {
    case SYS_READ:    //Argument: fd, buffer, size
    {
      if(esp[1] == 0) //STDIN_FILENO
      {
        int result = 0;
        for(int i=0; i<esp[3]; i++)
        {
          uint8_t tmp = input_getc();       //Returnerar uint8_t key
          if(tmp == (uint8_t)'\r')
            tmp = (uint8_t)'\n';

          ((char*)esp[2])[i] = tmp;         //Hämta tecken från STDIN till buffer
          const char * tmp_buf = (char *)&tmp; 
          putbuf(tmp_buf, 1);
          result = i+1;      
        }
          f->eax = result;
      }
      else if(esp[1] == 1)    //Får inte läsa från STDOU_FILENO
          f->eax = -1;

      else          //Vanlig fil
      {
        struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        f->eax = file_read(file, (void*)esp[2], esp[3]);
      }
      break;
    }
    case SYS_WRITE:
    {
      if(esp[1] == 1) //STDOUT_FILENO
      {

          putbuf((const char*)esp[2], esp[3]);
         
         f->eax = esp[3];
      }
      else if(esp[1] == 0)
          f->eax = -1;
      else
      {
        struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        f->eax = file_write(file, (const void*)esp[2], esp[3]);
      }

      break;
    }
    case SYS_HALT:
    {
      printf("Testing HALT\n");

      power_off();
      break;
    }
    case SYS_EXIT:
    {
      printf("SYS_EXIT with code ");
      printf("%d\n", esp[1]);     //Print the entered paratmeter for the exit call
      
      thread_exit();
      break;
    }
    case SYS_CREATE:
    {
      f->eax = filesys_create((char*)esp[1], esp[2]);
      /*{
        f->eax = true;
      }
      else
        f->eax = false;*/
      break;
    }
    case SYS_OPEN:
    {
      
      if(map_size(file_table) >= 16)
      {
        f->eax = -1;
        break;
      }

      struct file* open_f = filesys_open((char*)esp[1]);

      if(open_f == NULL)
      {
        f->eax = -1;
        break;
      }
      
      f->eax = map_insert(file_table, open_f);

      break;
    }
    case SYS_CLOSE:
    {
      struct file* file = map_remove(file_table, esp[1]);
      if(file == NULL)
      {
        f->eax = -1;
        break;
      }
      file_close(file);
      break;
    }
    case SYS_REMOVE:
    {
      f->eax = filesys_remove((char*)esp[1]);
      break;
    }

    case SYS_SEEK:
    {
      struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        if(file_length(file) > esp[2])
            file_seek(file, esp[2]);
        else
          f->eax = file_length(file)-1;
          
        break;
    }
    case SYS_TELL:
    {
      struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        f->eax = file_tell(file);
        break;
    }
    case SYS_FILESIZE:
    {
      struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
      f->eax = file_length(file);
      break;
    }

    default:
    {
      printf ("Executed an unknown system call!\n");
      
      printf ("Stack top + 0: %d\n", esp[0]);
      printf ("Stack top + 1: %d\n", esp[1]);
      
      thread_exit ();
    }
    
  }
}