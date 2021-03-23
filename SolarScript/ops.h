#pragma once

#ifndef MVM_INCLUDE_OPS
#define MVM_INCLUDE_OPS

#include "defs.h"
#include "state.h"

#include <math.h>

typedef struct _mvm_Operation
{
  const char* name; // name used during compilation
  uint32_t id; // id used during execution

  /// use mvm_argc() to get the number of args, and mvm_get_<type>() functions
  /// to get objects off the stack. DO NOT MANUALLY INTERFACE WITH THE STATE.
  int (*exec)();
} mvm_Operation;

int mvm_Operation_comp( void *a, void *b )
{
  /* Compare based on names, not ids */
  mvm_Operation *oa = (mvm_Operation*)a;
  mvm_Operation *ob = (mvm_Operation*)b;

  return strcmp(oa->name, ob->name);
}

int mvm_Operation_comp_id( void *a, void *b )
{
  /* Compare based on ids, not names */
  mvm_Operation *oa = (mvm_Operation*)a;
  mvm_Operation *ob = (mvm_Operation*)b;

  return oa->id < ob->id ? -1 : oa->id > ob->id ? 1 : 0;
}

// Generate an operation from a name and exec function pointer
mvm_Operation *_mvm_genop( const char* name, int (*exec)() )
{
  mvm_Operation *o = mvm_malloc( mvm_Operation );
  if ( o ){
    static uint32_t id = 0;
    o->exec = exec;
    o->name = "not";
    o->id = id++;
  }
  return o;
}

// Builtin operations for arithmatic & logic

// Perform !a (not a) op and push result to stack
int _mvm_op_exec_not()
{
  bool worked = false; // whether a get() op was valid:

  mvmbool a = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvm_push_bool( !a );

  return 1;
}

// perform !a, but store result in a
int _mvm_op_exec_ipnot()
{
  bool worked = false; // whether a get() op was valid:

  mvmbool a = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvm_set_bool( 1, !a, &worked ); // we know it'll work!

  return 0;
}

int _mvm_op_exec_and()
{
  bool worked = false; // whether a get() op was valid:

  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( a && b );

  return 1;
}

int _mvm_op_exec_or()
{
  bool worked = false; // whether a get() op was valid:
  
  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( a || b );

  return 1;
}

int _mvm_op_exec_xor()
{
  bool worked = false; // whether a get() op was valid:
  
  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( !(a && b) && (a || b) );

  return 1;
}

int _mvm_op_exec_nxor()
{
  bool worked = false; // whether a get() op was valid:
  
  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( !(!(a && b) && (a || b)) );

  return 1;
}

int _mvm_op_exec_nand()
{
  bool worked = false; // whether a get() op was valid:
  
  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( !(a && b) );

  return 1;
}

int _mvm_op_exec_nor()
{
  bool worked = false; // whether a get() op was valid:
  
  mvmbool a = mvm_get_bool( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmbool b = mvm_get_bool( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_bool( !(a || b) );

  return 1;
}

// arithmatic:
int _mvm_op_exec_add() // push a + b to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_number( a + b );

  return 1;
}

int _mvm_op_exec_sub() // push a - b to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_number( a - b );

  return 1;
}

int _mvm_op_exec_mul() // push a * b to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_number( a * b );

  return 1;
}

int _mvm_op_exec_div() // push a / b to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_push_number( a / b );

  return 1;
}

int _mvm_op_exec_pow() // push a ^ b to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }
  mvm_push_number( powf32(a,b) );

  return 1;
}

int _mvm_op_exec_abs() // push |a| to the stack
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvm_push_number( fabsf(a) );

  return 1;
}

// In-place operations (result stored in a, not pushed to stack)
int _mvm_op_exec_ipadd() // a += b
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_set_number( 2, a + b, &worked ); // a is an mvmnum, so this works

  return 0;
}

int _mvm_op_exec_ipsub() // a -= b
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_set_number( 2, a - b, &worked ); // a is an mvmnum, so this works

  return 0;
}

int _mvm_op_exec_ipmul() // a *= b
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_set_number( 2, a*b, &worked ); // a is an mvmnum, so this works

  return 0;
}

int _mvm_op_exec_ipdiv() // a /= b
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_set_number( 2, a/b, &worked ); // a is an mvmnum, so this works

  return 0;
}

int _mvm_op_exec_ippow() // performs a = a ^ b
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvmnum b = mvm_get_number( 2, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_2 );
    return 0;
  }

  mvm_set_number( 2, powf32(a,b), &worked ); // a is an mvmnum, so this works

  return 0;
}

int _mvm_op_exec_ipabs() // performs a = |a|
{
  bool worked = false; // whether a get() op was valid:
  
  mvmnum a = mvm_get_number( 1, &worked );
  if ( !worked ) {
    mvm_set_error( MVM_BAD_ARG_1 );
    return 0;
  }

  mvm_set_number( 1, fabsf(a), &worked ); // a is an mvmnum, so this works

  return 0;
}

#endif // MVM_INCLUDE_OPS