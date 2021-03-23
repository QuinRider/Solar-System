// Compounds are Objects in which contain other objects (or compounds) - they're
// basiclly structs.

#pragma once

#include "aatree.h"
#include "object.h"

#define insert mvm_AATree_insert
#define remove mvm_AATree_remove
#define get mvm_AATree_get
#define map mvm_AATree

typedef struct _mvm_Compound
{
  const char* name;
  map children; // Any kind of object - including other compounds 
                // (wrapped in an object)
} mvm_Compound;


void mvm_del_Object_compound( mvm_Compound *o );

#undef insert
#undef remove
#undef get
#undef map