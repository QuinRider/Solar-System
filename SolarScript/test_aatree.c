/* Testing out the AAMap */

#include "aatree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int comp( void* _a, void *_b )
{
  int a = *((int*)_a);
  int b = *((int*)_b);
  if ( a < b ) return -1;
  if ( a > b ) return 1;
  else return 0;
}

#define mvm_insert mvm_AATree_insert
#define mvm_remove mvm_AATree_remove
#define mvm_get mvm_AATree_get

int main( int argc, const char* argv[] )
{
  time_t raw_time;
  struct tm *time_info;
  time( &raw_time );
  time_info = localtime( &raw_time );

  int seconds = time_info->tm_sec;
  int minutes = time_info->tm_min;
  int hours = time_info->tm_hour;

  int seed = seconds*minutes*hours;

  srand(seed);

  mvm_AATree *t = mvm_new_AATree( comp );

  int *key = mvm_new_int(0);

  int num_items = 50000;

  for ( int i = 0; i < num_items; ++i ){
    *key = i;
    mvm_insert( t, mvm_new_int(*key) ); // clone the key & insert it
  }

  for ( int i = 0; i < num_items; ++i ){
    *key = rand()%100 + 1;
    if ( *key > 50 ){
      *key = i;
      /* int * v = (int*)mvm_get( t, key ); */
      mvm_remove( t, key );
      int *v = (int*)mvm_AATree_last_deleted(t);
      free( v );
    }
    else{
      *key = i;
      int * v = (int*)mvm_get( t, key );
    }
  }

  printf( "\nSize of tree is: %u\n", t->size );

  // Cleanup:
  mvm_del_AATree( t, true ); // del tree and all data in it
  free( key ); key = NULL;

  printf( "A-OK\n" );

  return 0;
}