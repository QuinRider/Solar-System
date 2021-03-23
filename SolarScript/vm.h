#pragma once

#include "defs.h"
#include "object.h"
#include "state.h"
#include "ops.h"

#define MVM_SAFE

int MVM_INIT()
{
  MVM.state = NULL;
  mvm_init_AATree( &MVM.global_funcs, mvm_Operation_comp_id );
  
  // go through standard operations and add them to global_funcs
  {
    mvm_Operation *o;

  // Make quick work of prep by using this macro
#define prep( NAME )\
    o = _mvm_genop( #NAME, _mvm_op_exec_##NAME );\
    if ( !o ) return MVM_ERROR;\
    mvm_AATree_insert( &MVM.global_funcs, o );

    prep(not)
    prep(and)
    prep(or)
    prep(nand)
    prep(nor)
    prep(xor)
    prep(nxor)
    prep(add)
    prep(sub)
    prep(mul)
    prep(div)
    prep(pow)
    prep(abs)
    prep(ipadd)
    prep(ipsub)
    prep(ipmul)
    prep(ipdiv)
    prep(ipabs)
    prep(ippow)

#undef prep
  }

  return MVM_OK;  
}

// Call this to cleanup memory used by the MVM
void MVM_CLEANUP()
{
  printf( "Cleanup!\n" ); fflush(stdout);
  printf( "Deleting global function objects\n" );
  mvm_cleanup_AATree( &MVM.global_funcs, true );
}

/// Set the state to use for the following operations
void mvm_set_state( mvm_State *s );

/// Execute a pre-compiled opcode sequence
/// Use mvm_compile( const char* text, const char* ops, unsigned int *num )
/// to compile text into bytecode.
int mvm_exec( const char *ops, unsigned int num );

////////////////////////////////////////////////////////////////////////////////
// Implementation:

void mvm_set_state( mvm_State *s )
{
  MVM.state = s;
}

int mvm_exec( const char *ops, unsigned int num )
{
  int diff = 0; // stack difference (total cumulative over all ops)

#ifdef MVM_SAFE
  // Only check for a state if we're being super safe (one less check if not!)
  if ( MVM.state ){
#endif

  for ( uint32_t i = 0; i < num && MVM.state->error == MVM_OK; ++i ){
    // Get a function pointer to the function corrisponding to the provided
    // operation:
    mvm_Operation *fp = (mvm_Operation*)mvm_AATree_get( &MVM.global_funcs, 
                                                        (void*)&ops[i] );
    
    if ( !fp ){
      MVM.state->error = MVM_ERROR_INVALID_OP;
    }

    MVM.state->sp += (diff += fp->exec());
  }

#ifdef MVM_SAFE
  }
#endif

  return diff;
}