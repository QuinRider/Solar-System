/* Functions for loading/saving data from/to disk in a JSON like format */

#pragma once

#include "defs.h"
#include "aatree.h"

#define map mvm_AATree

typedef struct _mvm_Conf
{
  const char* name;
  map children;
} mvm_Conf;

int mvm_Conf_comp( void *a, void *b )
{
  mvm_Conf *ca = (mvm_Conf*)a;
  mvm_Conf *cb = (mvm_Conf*)b;

  return strcmp(ca->name, cb->name);
}

mvm_Conf *mvm_new_Cond( const char* name )
{
  if ( !name ) return NULL;

  mvm_Conf *c = mvm_malloc(mvm_Conf);
  mvm_Conf *c = mvm_malloc(mvm_Conf);
  
  c->name = (const char*)malloc(strlen(name)+1);
  strcpy( (char*)c->name, name );

  mvm_init_AATree( &c->children, mvm_Conf_comp );

  return c;
}

#undef map mvm_AATree