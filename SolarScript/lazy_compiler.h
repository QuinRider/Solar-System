// Copyright (C) 2021 Quin J. G. Rider (QuinJRider@Gmail.com)
// This is proprietary source code, and as such you have no rights to do 
// anything with it without written permission from the copyright holder 
// stated on the first line.


/* A super lazy compiler from text to MVM bytecode */
#include "defs.h"
#include "vm.h"
#include "state.h"
#include "dllist.h"

typedef struct _mvm_Map_Node_cstr_to_uint32
{
  const char* key;
  uint32_t value;
} mvm_MNode_cstr_to_uint32;

int _mvm_MNode_cstr_to_uint32_comp( void* a, void* b )
{
  mvm_MNode_cstr_to_uint32 *na = (mvm_MNode_cstr_to_uint32*)a;
  mvm_MNode_cstr_to_uint32 *nb = (mvm_MNode_cstr_to_uint32*)b;

  return strcmp( na->key, nb->key );
}

char* mvm_grow_char_array( char* src, uint32_t size, uint32_t new_size )
{
  char* dest = (char*)malloc(sizeof(char)*new_size);

  if ( dest ){
    for ( uint32_t i = 0; i < (size < new_size ? size : new_size); ++i ){
      dest[i] = src[i];
    }
    if ( src ) free( src );
    return dest;
  }
  else{
    return NULL;
  }
}

// Parse a token from text & set token to the tokens text (space separated data)
void _mvm_skip_whitespace( const char* text, uint32_t *i )
{
  if ( text ){
    while ( text[*i] == ' ' ||
            text[*i] == '\n' ||
            text[*i] == '\t' ||
            text[*i] == '\r' ) {
      ++(*i);
    }
  }
}

// Parses a token into a new char* (which you must free)
char* _mvm_parse_token( const char* text, uint32_t *i ){
  uint32_t ss = 8;
  char* s = (char*)malloc(ss);
  uint32_t si = 0;

  _mvm_skip_whitespace( text, i );
  if ( text[*i] ){ // not at EOL
    while ( text[*i] && 
            text[*i] != ' ' &&
            text[*i] != '\n' &&
            text[*i] != '\t' &&
            text[*i] != '\r' ) {
      s[si] = text[*i];
      si += 1;
      (*i) += 1;
      if ( si > (ss - 2) ){
        s = mvm_grow_char_array( s, ss, ss*2 );
        ss = ss*2;
      }
    }

    // shrink ss down to size:
    s = mvm_grow_char_array( s, ss, si+1 );

    s[si] = '\0'; // add null terminator

    return s;
  }
  else{
    if ( s ) free(s);
    return NULL;
  }
}

// Check if a token is a number of the form [.]?[0-9]*[.]?[0-9]*
// 1 = int, 2 = float, 0 = not a number
int mvm_token_is_number( const char* text )
{
  bool saw_dot = false; // Whether a decimal has been seen yet
  uint32_t i = 0;
  while ( text[i] != '\0' ){
    if ( text[i] == '.' ){
      if ( saw_dot ){
        return 0; // too many decimals in number
      }
      saw_dot = true;
    }
    else if ( !(text[i] > ('0'-1) && text[i] < ('9'+1)) ){
      return 0; // Something other than a number was found in the token
    }
    ++i;
  }

  return i ? (saw_dot ? 2 : 1) : 0;
}

// 0 = not boolean, 1 = true, 2 = false
int mvm_token_is_boolean( const char* text )
{
  return !strcmp( text, "true" ) ? 1 : !strcmp( text, "false" ) ? 2 : 0;
}

typedef struct _mvm_Var_Node
{
  const char* name;
  const char type;
} mvm_Var_Node;

int _mvm_Var_Node_comp( void* a, void* b )
{
  if ( a && !b ) return 1;
  if ( b && !a ) return -1;
  if ( !a && !b ) return 0;

  mvm_Var_Node *va = (mvm_Var_Node*)a;
  mvm_Var_Node *vb = (mvm_Var_Node*)b;

  return strcmp(va->name, vb->name);
}


typedef struct _mvm_Scope
{
  mvm_Tree vars;
} mvm_Scope;

// Returns bytecode version of the provided ASCII text code.
// Bytecode layout is:
// [constants|globals|user functions in order of declaration]
const char* mvm_compile( const char* text, char** err )
{
  uint32_t i = 0;

  // Need a list of tokens!!! Linked list time!
  mvm_List l;
  mvm_init_List( &l );



  while ( text[i] ){
    char* token = _mvm_parse_token( text, &i );
    if ( !token ){
      printf( "EOF reached\n\n" );
      break;
    }
    else{
      printf( "Parsed token \"%s\", i = %u\n", token, i );
      mvm_List_append( &l, (void*)token );
    }
  }

  mvm_AATree constants; // list of all constants declared
  mvm_init_AATree( &constants, _mvm_MNode_cstr_to_uint32_comp );

  // go through the tokens and identify constants (unique numbers, strings and booleans)
  mvm_List_Node *c = l.head;
  uint32_t const_loc = 0;

  while ( c ){
    char* str = (char*)c->data;
    if ( mvm_token_is_number(str) ){
      mvm_MNode_cstr_to_uint32 *n = mvm_malloc(mvm_MNode_cstr_to_uint32);
      *n = {str, const_loc++};
      mvm_AATree_insert_overwrite( &constants, n , true );
      printf( "Token '%s' can be converted safely to a number\n", str );
    }
    else if ( mvm_token_is_boolean(str) ){
      mvm_MNode_cstr_to_uint32 *n = mvm_malloc(mvm_MNode_cstr_to_uint32);
      *n = {str, const_loc++};
      mvm_AATree_insert_overwrite( &constants, n , true );
      printf( "Token '%s' can be converted safely to a boolean\n", str );
    }
    c = c->n;
  }

  mvm_AATree globals; // list of all global variables
  mvm_AATree scopes; // list of scopes (head = deepest, tail = global)

  // Convert the list of tokens l into bytecode  

  // cleanup the token list
  mvm_List_shred( &l );

  // Cleanup the constants tree
  mvm_cleanup_AATree( &constants, true );

  return NULL; // Set this to something!
}