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

static void
syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;
  
  switch ( esp[0] )
  {
    case SYS_READ:    //Argument: fd, buffer, size
    {
      if(esp[1] == 0) //STDIN_FILENO
      {
        for(int i=0; i<esp[3]; i++)
        {
          uint8_t tmp = input_getc();       //Returnerar uint8_t key
          if(tmp == (uint8_t)'\r')
            tmp = (uint8_t)'\n';

          ((char*)esp[2])[i] = tmp;
          const char * tmp_buf = (const char*)&tmp;
          putbuf(tmp_buf, 1);
        }
          f->eax = 1;
      }
      else if(esp[1] == 1)
          f->eax = -1;
      break;
    }
    case SYS_WRITE:
    {
      if(esp[1] == 1) //STDOUT_FILENO
      {

          putbuf((const char*)esp[2], esp[3]);
         
         f->eax = 1;
      }
      else if(esp[1] == 0)
          f->eax = -1;
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
      printf("Testing SYS_EXIT\n");
      printf("%d\n", esp[1]); //Print the entered paratmeter for the exit call
      thread_exit();
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
