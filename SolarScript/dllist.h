/* A basic doubly-linked list, which only uses void* data handles. */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "../M64Script/defs.h"

typedef struct _mvm_List_Node
{
  struct _mvm_List_Node *p, *n; // p=prev, n=next
  void *data;
} mvm_List_Node;

/// A container for mvm_List stuff - call all mvm_List_ functions on one of these.
typedef struct _mvm_List
{
  mvm_List_Node *head; // First item in list
  mvm_List_Node *tail; // Last item in list
  uint32_t size; // Size of the list
} mvm_List;

/// Allocate memory for a new mvm_List_Node
mvm_List_Node *mvm_List_Node_new( void* data )
{
  mvm_List_Node *n = (mvm_List_Node*)malloc(sizeof(mvm_List_Node));
  if ( !n ) return NULL;

  n->data = data;
  n->p = n->n = NULL;

  return n;
}

/// Allocate memory for a new mvm_List
mvm_List* mvm_new_List()
{
  mvm_List *l = (mvm_List*)malloc(sizeof(mvm_List));
  if ( !l ) return NULL;

  l->head = l->tail = NULL;
  l->size = 0;

  return l;
}

void mvm_init_List( mvm_List *l )
{
  if ( !l ) return;

  l->head = l->tail = NULL;
  l->size = 0;
}

/// Assumes the HELL out of l being valid.
/// Doesn't delete data in nodes - do this manually!
/// Deletes l and all of it's nodes.
void mvm_List_delete( mvm_List *l )
{
  while ( l->head ){
    mvm_List_Node *n = l->head;
    l->head = l->head->n;
    free( n );
  }
  free( l );
}

/// Same as mvm_List_delete(), but frees data in nodes too.
/// Assumes each node has a validly malloc'd data value - an invalid free WILL
/// occur if this is not the case!
void mvm_List_obliterate( mvm_List *l )
{
  while ( l->head ){
    mvm_List_Node *n = l->head;
    l->head = l->head->n;
    free( n->data );
    free( n );
  }
  free( l );
}

// Same as obliterate, but doesn't delete l!
void mvm_List_shred( mvm_List *l )
{
  while ( l->head ){
    mvm_List_Node *n = l->head;
    l->head = l->head->n;
    free( n->data );
    free( n );
  }
  l->head = l->tail = NULL;
  l->size = 0;
}


/// Appends data to the end of l.
/// Assumes the hell out of l being valid. BE SURE IT'S VALID.
bool mvm_List_append( mvm_List *l, void* data )
{
  mvm_List_Node *n = mvm_List_Node_new( data );
  if ( !n ) return false;

  if ( !l->head ){
    l->head = l->tail = n;
  }
  else{
    l->tail->n = n;
    n->p = l->tail;
    l->tail = n;
  }

  l->size += 1u;

  return true;
}

/// Pushes data to the front of l.
/// Assumes the hell out of l being valid. BE SURE IT'S VALID.
bool mvm_List_push( mvm_List *l, void* data )
{
  mvm_List_Node *n = mvm_List_Node_new( data );
  if ( !n ) return false;

  if ( !l->head ){
    l->head = l->tail = n;
  }
  else{
    n->n = l->head;
    l->head->p = n;
    l->head = n;
  }

  ++l->size;

  return true;
}

/// Returns data at index i in l, or NULL if out of bounds
bool mvm_List_insert( mvm_List *l, uint32_t i, void* data )
{
  if ( i == l->size ) return mvm_List_append( l, data );
  if ( !i ) return mvm_List_push( l, data );
  if ( i > l->size ) return false;
  
  mvm_List_Node *c = l->head;
  mvm_List_Node *p = NULL;

  while ( i > 0 && c->n ){
    p = c;
    c = c->n;
    --i;
  }

  mvm_List_Node *n = mvm_List_Node_new( data );
  n->p = p;
  p->n = n;
  n->n = c;
  c->p = n;

  ++l->size;

  // If i isn't 0 then it was out of bounds and c is NULL anyways.
  // If the list was empty, c is NULL.
  return true;
}

/// Returns data at index i in l, or NULL if out of bounds
mvm_List_Node* mvm_List_get_node( mvm_List *l, uint32_t i )
{
  if ( i == l->size - 1 ) return l->tail;
  if ( i >= l->size ) return NULL;
  
  mvm_List_Node *c = l->head;

  while ( i > 1 && c->n ){
    c = c->n;
    --i;
  }

  // If i isn't 0 then it was out of bounds and c is NULL anyways.
  // If the list was empty, c is NULL.
  return c;
}

void* mvm_List_get( mvm_List *l, uint32_t i ){
  mvm_List_Node* n = mvm_List_get_node(l,i);
  return n ? n->data : NULL;
}

void* mvm_List_remove( mvm_List *l, uint32_t i )
{
  void* d = NULL;

  mvm_List_Node *n = mvm_List_get_node(l,i);
  if ( !n ){
    printf( "get_node doesn't work! (Size = %u, i = %u)\n", l->size, i );
    exit(-1);
    return NULL;
  }
  else{
    d = n->data;
  }

  if ( n->p && n->n ){
    n->p->n = n->n;
    n->n->p = n->p;
  }
  else if ( n->p && !n->n ){
    /* tail removal */
    n->p->n = NULL;
    l->tail = n->p;
  }
  else if ( n->n && !n->p ){
    /* head removal */
    n->n->p = NULL;
    l->head = n->n;
  }
  else{
    /* List empty now */
    l->head = l->tail = NULL;
  }

  --l->size;

  free(n);

  return d;
}

// Code for testing this list code

// int comp( void* _a, void *_b )
// {
//   int a = *((int*)_a);
//   int b = *((int*)_b);
//   if ( a < b ) return -1;
//   if ( a > b ) return 1;
//   else return 0;
// }

// #define mvm_insert mvm_List_insert
// #define mvm_remove mvm_List_remove
// #define mvm_get mvm_List_get


// int main( int argc, const char* argv[] )
// {
//   time_t raw_time;
//   struct tm *time_info;
//   time( &raw_time );
//   time_info = localtime( &raw_time );

//   int seconds = time_info->tm_sec;
//   int minutes = time_info->tm_min;
//   int hours = time_info->tm_hour;

//   int seed = seconds*minutes*hours;

//   srand(seed);

//   mvm_List *l = mvm_List_new();

//   uint32_t *key = mvm_new_int(0);

//   uint32_t num_items = 10000;

//   for ( uint32_t i = 0; i < num_items; ++i ){
//     *key = i;
//     bool ok = mvm_insert( l, i, mvm_new_uint32_t(*key) ); // clone the key & insert it
//     if ( !ok ) printf( "\nFailed to insert\n" );
//   }

//   printf( "Size of list is %d\n", l->size );

//   for ( uint32_t i = 0; i < num_items; ++i ){
//     *key = (uint32_t)rand()%100 + 1;
//     if ( *key > 90 ){
//       int *v = (int*)mvm_remove( l, l->size - 1 );
//       if ( v ) {
//         free( v );
//       }
//       else printf( "Failed to find!\n" );
//     } 
//     else if ( *key > 80 ){
//       *key = (uint32_t)rand()%l->size;
//       mvm_insert( l, *key, mvm_new_uint32_t(i) );
//     }
//     else if ( *key > 70 ){
//       int *v = (int*)mvm_remove( l, 0 );
//       if ( v ){
//         free(v);
//       }
//       else if ( l->size ){
//         printf( "!!! Attempted pop, didn't work, list isn't empty !!!\n" );
//       }
//     }
//     else if ( *key > 60){
//       int *v = (int*)mvm_remove( l, l->size - 1 );
//       if ( v ){
//         free(v);
//       }
//       else if ( l->size ){
//         printf( "!!! Attempted pop, didn't work, list isn't empty !!!\n" );
//       }
//     }
//     else if ( *key > 50 ){
//       mvm_insert( l, 0, mvm_new_uint32_t(0) );
//     }
//     else if ( *key > 40 ){
//       mvm_insert( l, l->size-1, mvm_new_uint32_t(1) );
//     }
//     else{
//       *key = (uint32_t)rand()%l->size;
//       int *v = (int*)mvm_get( l, *key );
//       if ( !v ){
//         printf( "Expected to find something, did not!\n" );
//       }
//     }
//   }

//   printf( "\nSize of list is: %u\n", l->size );

//   // Cleanup:
//   mvm_List_obliterate( l ); // del list and all data in it
//   free( key ); key = NULL;

//   printf( "A-OK\n" );

//   return 0;
// }