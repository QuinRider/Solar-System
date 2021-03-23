#include "vm.h"
#include "lazy_compiler.h"

int main( int argc, const char* argv[] )
{
  int result = MVM_OK;
  if ( (result = MVM_INIT()) != MVM_OK ){
    printf( "Failed to initialize the MVM!\n" );
    return result;
  }

  // The CPU speed doesn't do anything yet!
  mvm_State *s = mvm_new_State( 0, 0, 0 );
  if ( !s ){
    printf( "Failed to create MVM state!\n" );
    return MVM_ERROR;
  }

  printf( "Initialized MVM and State correctly.\n" );  

  mvm_set_state( s );

  // Test the compiler:
  const char* code =
    "n1 = 1.23\n"
    "n2 = 23.4\n"
    "t = true\n"
    "f = false\t";

  char* error = NULL;
  const char* bytes = mvm_compile( code, &error );
  if ( error ){
    printf( "Result (error code): %s\n", error );
    free(error);
  }

  mvm_exec( NULL, 0 );

  MVM_CLEANUP();

  mvm_del_State( s );

  return result == MVM_OK ? 0 : result;
}