#pragma once

#include "defs.h"
#include "aatree.h"
#include <math.h>

// Notes:
// 1. Strings are immutable (re-created when changed, innefficient)
// 2. Numbers are all 32bit floats
// 3. Tables are AATrees of other objects

/// An object to be used by the MVM
typedef struct _mvm_Object
{
  char type; // Type of object
  union {
    int (*f)(); // A function
    void* p; // pointer
    mvmnum n; // number (float)
    mvmbool b; // boolean (unsigned char)
    const char* s; // duh, it's a c-string
  } data; // Data in object
} mvm_Object;

mvm_Object *mvm_new_Object_number( mvmnum n )
{
  mvm_Object *o = mvm_malloc(mvm_Object);

  if ( o ){
    o->type = MVM_TYPE::number;
    o->data.n = n;
  }

  return o;
}

// Creates an mvm_Object with type string & copies s into o->data.s
mvm_Object *mvm_new_Object_string( const char* s )
{
  mvm_Object *o = mvm_malloc(mvm_Object);

  if ( o ){
    o->type = MVM_TYPE::string;
    o->data.s = (const char*)malloc(strlen(s)*sizeof(char) + 1);
    strcpy((char*)(o->data.s), s);
  }

  return o;
}

// Creates an mvm_Object with type pointer and sets o->data.p to p
mvm_Object *mvm_new_Object_table( mvm_AATree* t )
{
  mvm_Object *o = mvm_malloc(mvm_Object);

  if ( o ){
    o->type = MVM_TYPE::pointer;
    o->data.p = (void*)t;
  }

  return o;
}

// Creates an mvm_Object with type pointer and sets o->data.p to p
mvm_Object *mvm_new_Object_pointer( void* p )
{
  mvm_Object *o = mvm_malloc(mvm_Object);

  if ( o ){
    o->type = MVM_TYPE::pointer;
    o->data.p = p;
  }

  return o;
}