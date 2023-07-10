#pragma once
//includes
//_Atomic
#include <stdlib.h>
//malloc and free
#include <memory.h>
//if debugging enable add io include
#ifdef _DEBUG
#define PTR_LOG 1
#include <stdio.h>
#endif
//macro defines
#define nullptr NULL
//calc offset of pointer to get whole block
#define offset(ptr) ((size_t)(ptr) - sizeof (reference_count_ptr))
//shift to start at data type part of block
#define shift(base,type) ((size_t)base + sizeof(type))

//enum typedef
typedef enum {unique,shared} pointer_type;
//struct typedef
typedef struct {
    //holds number of references ptr has, Atomic for thread safety
    _Atomic int ref;
    //destructor function pointer to handel clean up of advanced types
    void (*dtor)(void*);
    //enum whether type is shared or unique
    pointer_type ptr_type;
    //the actual ptr itself
    void *ptr;
    
}reference_count_ptr;


//create the pointer and init reference
void* allocate_ptr(size_t size, void (*func_ptr)(void*), pointer_type ptr_type_);

//release the pointer and set the calling variable to null
void release_ptr(void** ptr_);

//create a copy of the pointer and increment
void* get_ptr(void* ptr_);

//transfer ownership of the pointer and set calling var to null
void* move_ptr(void** ptr_);











