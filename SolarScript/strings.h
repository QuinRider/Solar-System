// Copyright 2021 Quin J. G. Rider (QuinJRider@Gmail.com)
//
// This is unpublished proprietary code and can not be used in any way without
// written permission from the copyright holder named above.
//


#include "defs.h"
#include "aatree.h"

typedef struct _mvm_String
{
  const char* str;
  unsigned int len; // length of used characters (excluding null terminator)
} mvm_String;

mvm_String *mvm_new_String( const char* str )
{
  mvm_String *s = mvm_malloc(mvm_String);
  if ( s ){
    s->len = strlen(str);
    s->str = (const char*)malloc(s->len * sizeof(char) + 1);
    strcpy((char*)s->str, str);
  }

  return s;
}

void mvm_del_String( mvm_String *s )
{
  if ( s ){
    if ( s->str ) free((void*)s->str);
    free( s );
  }
}

// Change s to s2
mvm_String *mvm_String_set_cstr( mvm_String *s, const char* s2 )
{
  if ( s && s2 ){
    if ( s->str ) free((void*)s->str);
    s->len = strlen(s2);
    s->str = (const char*)malloc(sizeof(char)*(s->len+1));
    strcpy((char*)s->str,s2);
  }
  return s;
}

// Append s2 to s1
mvm_String *mvm_String_append_cstr( mvm_String *s, const char* s2 )
{
  if ( s && s2 ){
    if ( s->str ) free((void*)s->str);
    s->len = s->len + strlen(s2);
    s->str = (const char*)malloc(s->len + 1);
    strcat( (char*)s->str, s2 );
  }
  return s;
}

// Change s to s2
mvm_String *mvm_String_set( mvm_String *s, mvm_String *s2 )
{
  if ( s && s2 ){
    if ( s->str ) free((void*)s->str);
    s->len = s2->len;
    s->str = (const char*)malloc(s->len+1);
    strcpy((char*)s->str,(char*)s2->str);
  }
  return s;
}

// Append s2 to s1
mvm_String *mvm_String_append( mvm_String *s, mvm_String *s2 )
{
  if ( s && s2 ){
    if ( s->str ) free((void*)s->str);
    s->len = s->len + s2->len;
    s->str = (const char*)malloc(s->len + 1);
    strcat( (char*)s->str, (char*)s2->str );
  }
  return s;
}