/* halt.c

   Simple program to test whether running a user program works.
 	
   Just invokes a system call that shuts down the OS. */

#include <syscall.h>
#include <threads/init.h>

int
main (void)
{
  int status = 12;
  //halt ();
  exit(status);
  /* not reached */
}
