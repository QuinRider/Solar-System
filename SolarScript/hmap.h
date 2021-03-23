/* A hashmap, which is used for all non-primative data types in M64Script */
#include "defs.h"
#include "object.h"
#include <stdlib.h>
#include <stdatomic.h>

#pragma once

typedef uint64_t mvm_hash;

// A simple hash-map without buckets (to be changed to use buckets!)
typedef struct _mvm_HMap
{
  mvm_Object *array;
  uint32_t size; // Size of array
  uint32_t count; // Number of objects in array
  mvm_hash (*hash)(void* d); // Hashing algorithm for data
} mvm_HMap;

// Creates & returns a new hmap
mvm_HMap *mvm_new_hmap( mvm_hash (*hash_func)(void*) )
{
  mvm_HMap *m = (mvm_HMap*)malloc(sizeof(mvm_HMap));

  if ( m ){
    m->array = (mvm_Object*)malloc(sizeof(mvm_Object)*8);
    m->size = m->count = 0;
    m->hash = hash_func;
  }

  return m;
}

// djb2 hashing
inline mvm_hash mvm_hash_cstr( unsigned char *str )
{
  unsigned long hash = 5381;
  int c;
  while (c = *str++)  hash = hash * 33 + c;
  return hash;
}

// Same as mvm_hash_cstr, but works with non-null terminated strings
inline mvm_hash mvm_hash_scstr( unsigned char *str, uint32_t size )
{
  mvm_hash hash = 5381;
  int c;
  while ( size ) hash = hash * 33 + str[size--];
  return hash;
}

// It's YOUR job to ensure obj is valid - not this functions!
inline mvm_hash mvm_hash_object( const mvm_Object *obj )
{
  switch ( obj->type ){
    case MVM_TYPE::number:
      break;
    case MVM_TYPE::integer:
      break;
    case MVM_TYPE::boolean:
      break;
    case MVM_TYPE::string:
      break;
  }
}

#define MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( NAME, TYPE ) \
  inline mvm_hash NAME(TYPE t){return mvm_hash_scstr((unsigned char*)&t,\
                                                     sizeof(TYPE));}

//------------------------------------------------------------------------------
// Some default hash functions
// Regular integers:
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_int, int )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_uint, unsigned int )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_longlong, long long int )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_ulonglong, unsigned long long int )
// stdint integers:
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_uint32, uint32_t )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_uint64, uint64_t )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_int32, int32_t )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_int64, int64_t )
// Floating point numbers:
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_float, float )
MVM_DEF_HASH_FUNCTION_FOR_PRIMATIVE( mvm_hash_double, double )