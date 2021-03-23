// An implementation of an AA Tree
// (simmilar to a red-black tree, faster search, slower insertion/deletion,
// painless implementation)

// based on code from the following sources:
// https://web.archive.org/web/20160303175254/http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx
// https://gist.github.com/siritori/2760460
// https://en.wikipedia.org/wiki/AA_tree
// http://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/12-AAtrees+Treaps.pdf

// To-do:
//   [ ] Convert delete to iterative
//   [ ] Convert insert to iterative
//   [X] Convert get to iterative
//   [X] Test in valgrind to ensure no memory leaks


#include "defs.h"
#include <stdio.h>

#pragma once

typedef struct _mvm_AANode
{
  struct _mvm_AANode *left, *right;
  void *data;
  uint32_t level;
} mvm_AANode;

mvm_AANode *mvm_new_AANode( void *data )
{
  mvm_AANode *n = mvm_malloc(mvm_AANode);
  if ( n ){
    n->data = data;
    n->left = n->right = NULL;
    n->level = 0;
  }

  return n;
}

void mvm_AANode_decompose( mvm_AANode *self, bool freeData )
{
  if ( self ){
    if ( self->level == 0 ) return;
    mvm_AANode_decompose(self->left, freeData );
    mvm_AANode_decompose(self->right, freeData );
    if ( freeData && self->data ) free( self->data ); 
    free( self );
  }
}

// A simple AA Tree
typedef struct _mvm_AATree
{
  mvm_AANode *root; // root of the tree
  mvm_AANode *nil; // used as leaf-terminator
  mvm_AANode *last, *deleted; // for recursion in deletion
  uint32_t size; // number of nodes in tree
  int (*comp)( void *a, void *b ); // comparison function for sorting (<0 if a<b, >0 if a>b, 0 if a=b)
} mvm_AATree;

void mvm_del_AATree( mvm_AATree *t, bool freeData )
{
  if ( t ){
    if ( t->root ) mvm_AANode_decompose( t->root, freeData );
    t->root = NULL;
    t->size = 0;
    free( t->nil );
    free( t );
  }
}

// To cleanup a tree that's created on the stack, call this!
// NOTE: The last deleted nodes data needs to be manually handled by YOU.
void mvm_cleanup_AATree( mvm_AATree *t, bool freeData )
{
  if ( t ){
    if ( t->root ) mvm_AANode_decompose( t->root, freeData );
    t->root = NULL;
    t->size = 0;
    free( t->nil );
  }
}

// Remove all nodes from the tree, potentially freeing data, but keeping the 
// tree itself around for later use.
// NOTE: The last deleted nodes data needs to be manually handled by YOU,
// __prior__ to calling this function.
void mvm_AATree_decompose( mvm_AATree *self, bool freeData )
{
  if ( self ){
    mvm_AANode_decompose( self->root, freeData );
    self->root = self->nil;
    self->size = 0;
    self->deleted = self->nil;
    self->last = self->nil;
  }
}

mvm_AATree *mvm_new_AATree( int (*comp)(void *a, void* b) )
{
  mvm_AATree *t = mvm_malloc( mvm_AATree );
  t->size = 0;
  t->nil = mvm_new_AANode( NULL );
  t->root = t->nil;
  t->last = t->deleted = t->nil;
  t->nil->left = t->nil->right = t->nil;
  t->comp = comp;
  return t;
}

void mvm_init_AATree( mvm_AATree *t, int (*comp)(void *a, void* b) )
{
  if ( t ){
    t->size = 0;
    t->nil = mvm_new_AANode( NULL );
    t->root = t->nil;
    t->last = t->deleted = t->nil;
    t->nil->left = t->nil->right = t->nil;
    t->comp = comp;
  }
}

void* mvm_AATree_get( mvm_AATree *t, void *d )
{
  if ( t ){
    mvm_AANode* cur = t->root;
    while ( cur != t->nil ){
      if ( t->comp(d,cur->data ) > 0 ) cur = cur->right;
      else if ( t->comp(d,cur->data ) < 0 ) cur = cur->left;
      else return cur->data;
    }
  }

  return NULL;
}

// get the root value of the given tree
void *mvm_AATree_rval( mvm_AATree *t )
{
  if ( t ) return t->root->data;
  return NULL;
}

mvm_AANode* _mvm_AATree_skew( mvm_AANode* n )
{
  if ( n->level != n->left->level ) return n;

  mvm_AANode* l = n->left;
  n->left = l->right;
  l->right = n;
  n = l;

  return n;
}

mvm_AANode* _mvm_AATree_split( mvm_AANode* n )
{
  if ( n->right->right->level != n->level ) return n;

  mvm_AANode* r = n->right;
  n->right = r->left;
  r->left = n;
  n = r;
  ++n->level;

  return n;
}

mvm_AANode* _mvm_AATree_insert_rec( mvm_AATree *t, mvm_AANode* n, void* v, const bool *overwrite )
{
  // Recursively traverse down the tree making rotaions via skew and split
  // as required; because no parent pointer exists in the Node class we use
  // recursion to keep track of 'important' nodes, as well as the new root
  // of the tree after the insertion and rebalancing is complete. This is a
  // very functional programming approach. A non-recursive implementation
  // would be _slightly_ faster due to less function calls and stack ops.
  if ( n == t->nil ) {
    n = mvm_new_AANode( v );
    n->level =1;
    n->left = n->right = t->nil;

    ++t->size;
    return n;
  }
  if ( t->comp(v, n->data) < 0 ){
    n->left = _mvm_AATree_insert_rec( t, n->left, v, overwrite );
  }
  else if ( t->comp(v, n->data) > 0 ){
    n->right = _mvm_AATree_insert_rec( t, n->right, v, overwrite );
  }
  else {
    if ( *overwrite ){
      n->data = v;
      return n;
    }
  }

  n = _mvm_AATree_skew( n );
  n = _mvm_AATree_split( n );
  return n;
}

// Might need to update to keep last && deleted inside of the tree!
mvm_AANode* _mvm_AATree_remove_rec( mvm_AATree *t, mvm_AANode* n, void *v )
{
  if ( n == t->nil ) return n;

  t->last = n;

  if ( t->comp(v, n->data) < 0 ){
    n->left = _mvm_AATree_remove_rec( t, n->left, v );
  }
  else{
    t->deleted = n;
    n->right = _mvm_AATree_remove_rec( t, n->right, v );
  }

  if ( n == t->last && t->deleted != t->nil && !t->comp(v, t->deleted->data) ){
    t->nil->data = t->deleted->data;
    t->deleted->data = n->data;
    t->deleted = t->nil;
    n = n->right;
    free( t->last );
    --t->size;
  }
  else if ( n->left->level < n->level-1 || n->right->level < n->level-1 ){
    --n->level;
    if ( n->right->level > n->level ){
        n->right->level = n->level;
    }
    n = _mvm_AATree_skew( n );
    n->right = _mvm_AATree_skew( n->right );
    n->right->right = _mvm_AATree_skew( n->right->right );
    n = _mvm_AATree_split( n );
    n->right = _mvm_AATree_split( n->right );
  }

  return n;
}

void* mvm_AATree_last_deleted( mvm_AATree *t )
{
  void *d = t->nil->data;
  t->nil->data = NULL;
  return d;
}

void mvm_AATree_insert( mvm_AATree *t, void* d )
{
  const static bool overwrite = false;
  t->root = _mvm_AATree_insert_rec( t, t->root, d, &overwrite );
}

void mvm_AATree_insert_overwrite( mvm_AATree *t, void* d, bool overwrite )
{
  t->root = _mvm_AATree_insert_rec( t, t->root, d, &overwrite );
}

bool mvm_AATree_remove( mvm_AATree *t, void* d )
{
  return (t->root = _mvm_AATree_remove_rec( t, t->root, d )) ? true : false;
}


// macros:
#define mvm_tinsert mvm_AATree_insert
#define mvm_tremove mvm_AATree_remove
#define mvm_tget mvm_AATree_get
#define mvm_tlastdel mvm_AATree_last_deleted
#define mvm_Tree mvm_AATree