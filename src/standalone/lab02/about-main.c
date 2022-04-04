#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  int x = 0;
  double average = 0;
  int total_chars = 0;
  while(x < argc)
    {

      printf("%-20s" ,argv[x]);
      printf("%2lu\n", strlen(argv[x]));
      total_chars += strlen(argv[x]);
      x++;
    }
  
  
  if(total_chars < 100)
    printf("%-20s", "Total length");
    
    
  else
    printf("%-19s", "Total length");

  printf("%d\n", total_chars);
  
  average = (double)total_chars/(double)argc;
  if(average < 10.0)
    printf("%-18s", "Average length");
  else
    printf("%-17s", "Average length");
    
    
  printf("%.2f\n", average);


  return 0;
}
