// Copyright (C) 2021 Quin J. G. Rider (QuinJRider@Gmail.com)
// This is proprietary source code, and as such you have no rights to do 
// anything with it without written permission from the copyright holder 
// stated on the first line.

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int mvmop;
typedef unsigned char mvmbyte;
typedef float mvmnum;
typedef int32_t mvmint;
typedef unsigned char mvmbool;

#ifndef __cplusplus
  #ifndef bool
    #define bool unsigned char
    #define true 1
    #define false 0
  #endif
#endif

enum MVM_TYPE{
  number, // 32 or 64bit float
  integer, // 32 or 64bit integer
  string, // const char*
  boolean, // 8bit boolean (unsigned char)
  pointer, // 32 or 64bit void* pointer
  compound // 32 or 64bit void* pointer to an mvm_Compound
};

// Error codes
#define MVM_OK 1
#define MVM_NOT_OK 0
#define MVM_ERROR -1
#define MVM_WARNING -2
#define MVM_NOT_FOUND -3
#define MVM_BAD_ARG_0 -4
#define MVM_BAD_ARG_1 -5
#define MVM_BAD_ARG_2 -6
#define MVM_BAD_ARG_3 -7
#define MVM_BAD_ARG_4 -8
#define MVM_BAD_ARG_5 -9
#define MVM_BAD_ARG_6 -10
#define MVM_BAD_ARG_7 -11
#define MVM_BAD_ARG -12
#define MVM_ERROR_INVALID_OP -200
#define MVM_ERROR_OUT_OF_BOUNDS -300
#define MVM_ERROR_ABOVE_BOUNDS -301
#define MVM_ERROR_BELOW_BOUNDS -302
#define MVM_ERROR_STACK_OVERFLOW -400 // attempted to put too much in stack
#define MVM_ERROR_STACK_UNDERFLOW -401 // attempted remove from empty stack

// This is the maximum allowed depth of a scope parsed by the compiler - it
// should be plenty enough! This is NOT the maximum recursion depth, which is
// limited by the current mvm_State stack size and variables such as how many
// objects are returned from or passed to functions.
#define MVM_COMPILER_MAX_SCOPE_DEPTH 128

// Allocate memory of with size of sizeof(T)
#define mvm_malloc( T ) (T*)malloc(sizeof(T))

// Allocate memory with size of sizeof(typeof(D)) and set it to the value D
#define mvm_copy( D ) \
  (typeof(D)*)memcpy((void*)mvm_malloc(typeof(D)), (void*)(&D), sizeof(typeof(D)))
  
#define MVM_GEN_NEW( NAME, T ) \
  T* mvm_new_##T( T d ){ T *x = mvm_malloc(T); *x = d; return x; }

MVM_GEN_NEW(int, int)
MVM_GEN_NEW(uint, uint32_t)
MVM_GEN_NEW(int64, int64_t)
MVM_GEN_NEW(uint64, uint64_t)
MVM_GEN_NEW(double, double)
MVM_GEN_NEW(float, float)
MVM_GEN_NEW(bool, bool)