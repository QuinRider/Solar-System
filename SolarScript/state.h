#pragma once

#include "defs.h"
#include "object.h"
#include "aatree.h"

#define MVM_DEFAULT_HEAP_SIZE 8388608 // measured in #objects (~64MB)
#define MVM_DEFAULT_STACK_SIZE 1048576 // measured in # of objects (~8MB)


struct _mvm_State;

// Global state information
struct __MVM__// MVM
{
  struct _mvm_State *state; // current state
  mvm_AATree global_funcs;
} MVM;

// MVM_INIT is in vm.h!

/// Stores state information
typedef struct _mvm_State
{
  mvm_Object *s; // Actual stack data
  uint32_t sp; // stack pointer
  uint32_t ss; // stack size (#objects)
  uint32_t hs; // heap size (#objects)

  uint32_t floating; // number of "floating" objects on the stack.
  // floating objects are those that were pushed to the stack by an operation
  // and have yet to be claimed.
  // Let |floating| = f. All f floating objects must be picked up from the stack
  // in sequence, otherwise f-x objects will be lost (where x is the number of
  // sequential pickup operations). Objects pushed to the stack with type 
  // pointer that are not claimed will cause the MVM to set the error flag
  // to true, as these require manual handling by the user.

  // Performance settings - these can be implemented later!
  uint32_t ops_per_second; // Number of allowed operations in 1 second
  uint32_t ops_in_second; // Number of operations in the last second
  double perf_last_reset; // Last time the vm's perf stats were reset

  int error; // Anything else means there's an error!
  const char* error_message; // Custom message to describe error better
} mvm_State;

mvm_State *mvm_new_State( uint32_t stack_size, uint32_t heap_size,
                          uint32_t cpu_freq )
{
  mvm_State *s = mvm_malloc(mvm_State);
  if ( s ){
    s->ss = stack_size ? stack_size : MVM_DEFAULT_STACK_SIZE;
    s->sp = 0;
    s->hs = heap_size ? heap_size : MVM_DEFAULT_HEAP_SIZE;
    s->ops_per_second = cpu_freq;
    s->ops_in_second = 0;
    s->perf_last_reset = 0.0;
    s->error = MVM_OK;
    s->error_message = "No error";
    s->s = (mvm_Object*)malloc(sizeof(mvm_Object) * s->ss);
  }

  return s;  
}

// Only call this if you KNOW the error message of this state is NULL
// or not heap allocated!
void mvm_del_State( mvm_State *s )
{
  if ( !s ) return;

  if ( s->s ) free( (void*)s->s );
  free( s );
}

// Stack manipulation functions:

// we gonna start simple...
void mvm_push_number( mvmnum n )
{
  if ( MVM.state ){
    if ( MVM.state->sp + 1 < MVM.state->ss ){
      ++MVM.state->sp;
      MVM.state->s[MVM.state->sp].type = MVM_TYPE::number;
      MVM.state->s[MVM.state->sp].data.n = n;
    }
    else{
      MVM.state->error = MVM_ERROR_STACK_OVERFLOW;
    }
  }
}

// Grab a number from the stack at index sp - i
// (index sp is NOT a valid object!).
// On failure worked is set to false and 0.0 is returned.
// (0.0f can be returned when worked == true as well)
mvmnum mvm_get_number( uint32_t i, bool *worked )
{
  mvm_State *s = MVM.state;

  if ( s && i && s->sp - i > 0 && 
       s->s[s->sp - i].type == MVM_TYPE::number ){
    *worked = true;

    return s->s[s->sp - i].data.n;
  }
  else {
    *worked = false;
  }

  return 0.0f;
}

// Set a number from the stack at index sp - i
// (index sp is NOT a valid object!).
// On failure worked is set to false and 0.0 is returned.
// (0.0f can be returned when worked == true as well)
void mvm_set_number( uint32_t i, mvmnum n, bool *worked )
{
  mvm_State *s = MVM.state;

  if ( s && i && s->sp - i > 0 && 
       s->s[s->sp - i].type == MVM_TYPE::number ){
    *worked = true;

    s->s[s->sp - i].data.n = n;
  }
  else {
    *worked = false;
  }
}

// Grab a boolean from the stack at index sp - i
// (index sp is NOT a valid object!).
// On failure worked is set to false
mvmbool mvm_get_bool( uint32_t i, bool *worked )
{
  mvm_State *s = MVM.state;

  if ( s && i && s->sp - i > 0 && 
       s->s[s->sp - i].type == MVM_TYPE::boolean ){
    *worked = true;

    return s->s[s->sp - i].data.b;
  }
  else {
    *worked = false;
  }

  return false;
}

// Set a bool from the stack at index sp - i
// (index sp is NOT a valid object!).
// On failure worked is set to false
void mvm_set_bool( uint32_t i, mvmbool b, bool *worked )
{
  mvm_State *s = MVM.state;

  if ( s && i && s->sp - i > 0 && 
       s->s[s->sp - i].type == MVM_TYPE::boolean ){
    *worked = true;

    s->s[s->sp - i].data.b = b;
  }
  else {
    *worked = false;
  }
}

void mvm_push_bool( mvmbool b )
{
  if ( MVM.state ){
    if ( MVM.state->sp + 1 < MVM.state->ss ){
      ++MVM.state->sp;
      MVM.state->s[MVM.state->sp].type = MVM_TYPE::boolean;
      MVM.state->s[MVM.state->sp].data.b = b;
    }
    else{
      MVM.state->error = MVM_ERROR_STACK_OVERFLOW;
    }
  }
}

// Use this to set the MVM.state->error code to notify the VM of a runtime
// error.
void mvm_set_error( int code )
{
  if ( MVM.state ) MVM.state->error = code;
}

// Use this to set the MVM.state->error_message - you must also set the error
// code in order for the VM to recognize an issue. This is just for additional
// debugging information, or custom errors.
// The message _should_ be a heap allocated char*.
// The message _should_ be freed using mvm_free_error_message.
// (If you use a stack allocated string, then you do not need to deallocate)
void mvm_set_error_message( const char* msg )
{
  if ( MVM.state ) MVM.state->error_message = msg;
}

// Deletes the manualluy set error_message if it's not a nullptr...
// Be careful to not accidentally 
void mvm__error_message( const char* msg )
{
  if ( MVM.state && MVM.state->error_message ) 
    free( (char*)(MVM.state->error_message) );
}

/* void mvm_push_char( mvmnum n );
void mvm_push_string( mvmnum n );
void mvm_push_pointer( mvmnum n );
void mvm_push_pointer( mvmnum n ); */

