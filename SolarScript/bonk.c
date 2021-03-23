#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void modifyPointer( int **p )
{
  *p = (int*)malloc(sizeof(int));
  **p = 123;
}

int main( int argc, const char** argv )
{
  int* i = NULL;
  modifyPointer( &i );

  printf( "i = %d\n", *i );

  free(i);
  return 0;
}