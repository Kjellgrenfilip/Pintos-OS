#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "../devices/timer.h"
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
   if(start == NULL)
        return false;
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
//Stäng filer i process cleanupif(start == NULL)
        
void syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;    //Stackpekare

  if(esp == NULL)
  {
    process_exit(-1);
    thread_exit();
  }
  if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 4))
      {
        process_exit(-1);
        thread_exit();
       
      }

  struct map* file_table = &(thread_current()->file_table);   //Hämta pekare till file table från tråden
  
  switch ( esp[0] )
  {
    case SYS_READ:    //Argument: fd, buffer, size
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp[2], esp[3]))
      {
        thread_exit();
        break;
      }

      if(esp[1] == STDIN_FILENO) //STDIN_FILENO
      {
        int result = 0;
        for(int i=0; i<esp[3]; i++)
        {
          uint8_t tmp = input_getc();       //Returnerar uint8_t key
          if(tmp == (uint8_t)'\r')
            tmp = (uint8_t)'\n';

          ((char*)esp[2])[i] = tmp;         //Lägg till det hämtade tecknet i buffern
          const char * tmp_buf = (char*)&tmp; //Casta tmp till char*
          putbuf(tmp_buf, 1);                   //Skriv ut det hämtade tecknet på skärmen
          result = i+1;      
        }
          f->eax = result;
      }
      else if(esp[1] == STDOUT_FILENO)    //Får inte läsa från STDOUT_FILENO
          f->eax = -1;

      else          //Vanlig fil
      {
        struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        f->eax = file_read(file, (void*)esp[2], esp[3]); // file_read hanterar så att man inte kan läsa mer än vad som finns i filen.
      }
      break;
    }
    
    case SYS_WRITE:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 16))
        thread_exit();
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp[2], esp[3]))
        thread_exit();


      if(esp[1] == STDOUT_FILENO) 
      {

          putbuf((const char*)esp[2], esp[3]);
         
         f->eax = esp[3];
      }
      else if(esp[1] == STDIN_FILENO)
          f->eax = -1;
      else
      {
        struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
        f->eax = file_write(file, (const void*)esp[2], esp[3]);//file_write hanterar så att man inte kan skriva utanför storleken på filen.
      }

      break;
    }
    
    case SYS_HALT:
    {
      power_off();
      break;
    }
    
    case SYS_EXIT:
    { 
      if(verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
        process_exit((int)(esp[1]));
      else
        process_exit(-1);
      thread_exit();
      break;
    }
    
    case SYS_CREATE:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 12))
      {
        f->eax = false;
        process_exit(-1);
        thread_exit();
      }
      if(!verify_variable_length(thread_current()->pagedir, (void*)esp[1]))
        thread_exit();

      if(esp[2] <= 0)
        f->eax = false;
      
      f->eax = filesys_create((char*)esp[1], esp[2]);   //filesys_create returnerar true om vi lyckade skapa filen. Annars false.
      
      break;
    }
    
    case SYS_OPEN:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        f->eax = -1;
        process_exit(-1);
        thread_exit();
      }
      if(!verify_variable_length(thread_current()->pagedir, (char*)esp[1]))
      {
        f->eax = -1;
        process_exit(-1);
        thread_exit();
      }


      if(map_size(file_table) >= 16) //Möjligtvis låta mappen själv hålla koll? 
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
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
      
      struct file* file = map_remove(file_table, esp[1]); //map_remove returnerar filen om den finns. Annars NULL
      if(file == NULL)
      {
        f->eax = -1; // Kanske inte behövs?
        break;
      }
      file_close(file);
      break;
    }
    
    case SYS_REMOVE:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }

      f->eax = filesys_remove((char*)esp[1]); //Returnerar false om filen inte fanns. True om den fanns och togs bort.
      break;
    }

    case SYS_SEEK:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 12))
      {
        thread_exit();
        break;
      }
      struct file* file = map_find(file_table, esp[1]); 
        if(file == NULL)  //Finns den angivna filen öppen?
          {
           // f->eax = -1;   //seek() har inget returvärde.....
            break;
          }
        if(file_length(file) > esp[2])
        {
            file_seek(file, esp[2]);
        }
        else
          {
          file_seek(file, file_length(file)); //Om det angivna seek-värdet är större än filen, seeka till end of file.
          }

        break;
    }
    
    case SYS_TELL:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
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
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
      struct file* file = map_find(file_table, esp[1]);
        if(file == NULL)
          {
            f->eax = -1;
            break;
          }
      f->eax = file_length(file);
      break;
    }
    
    case SYS_EXEC:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
      if(!verify_variable_length(thread_current()->pagedir, (char*)esp[1]))
      {
        thread_exit();
        break;
      }
      f->eax = process_execute((char*)esp[1]);
      break;
    }
    case SYS_PLIST:
    {
      process_print_list();
      break;
    }

    case SYS_SLEEP:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
      timer_msleep((int)esp[1]);
      break;
    }

    case SYS_WAIT:
    {
      if(!verify_fix_length(thread_current()->pagedir, (void*)esp, 8))
      {
        thread_exit();
        break;
      }
      f->eax = process_wait((int)esp[1]);
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