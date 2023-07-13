#pragma once
//includes
//_Atomic
#include <stdlib.h>
//malloc and free
#include <memory.h>
//if debugging enable add io include
#ifdef _DEBUG_PTR
    #define PTR_LOG 1
    #include <stdio.h>
#endif
//macro defines
#define nullptr NULL
//calc offset of pointer to get whole block
#define offset_ptr(ptr) ((size_t)(ptr) - sizeof (reference_count_ptr))
//shift to start at data type part of block
#define shift_ptr(base,type) ((size_t)base + sizeof(type))
#define PTR_FLAG_63 (1 << 63)
#define PTR_FLAG_SET (n,f) ((n) |= (f))
#define PTR_FLAG_CHECK (n,f) ((n)&(f))
#define PTR_UNIQUE 1
#define PTR_SHARED 0

//enum typedef
typedef enum {unique,shared} pointer_type;
//struct typedef
typedef struct {
    //holds number of references ptr has, Atomic for thread safety
    _Atomic u_int64_t ref;
    //destructor function pointer to handel clean up of advanced types
    void (*dtor)(void*);
    //enum whether type is shared or unique
    pointer_type ptr_type;
    //the actual ptr itself
    void *ptr;
    
}reference_count_ptr;


//this function should allocate the reference_count_ptr struct
//and then this function should return just the ptr of the data
static inline void* allocate_ptr(size_t size, void (*func_ptr)(void*), pointer_type ptr_type_){
    //check function ptr
    if(!func_ptr){
        //no destructor provided
        #if PTR_LOG == 1
            printf("[DEBUG]: Function call of pointer_type: %s and size of: %lu was not provided destructor function pointer\n",(ptr_type_ == unique ? "Unique" : "Shared"), size);
        #endif
    }
    //allocate block for struct and ptr
    reference_count_ptr* ref_ptr = (reference_count_ptr*)malloc(sizeof(reference_count_ptr) + size);

    //if allocation failed return null back to user
    if(!ref_ptr){
        //error allocating memory
        #if PTR_LOG == 1
            printf("[DEBUG]: failed to allocate memory for pointer container\n");
        #endif
        return nullptr;
    }
    #if PTR_LOG == 1
        printf("[DEBUG]: allocated memory for contianer at %p\n",ref_ptr);
    #endif
    //set destructor var in struct
    ref_ptr->dtor = func_ptr;
    //set ptr type unique or shared
    ref_ptr->ptr_type = ptr_type_;
    //align the ptr to the end of the struct block
    ref_ptr->ptr = (void*)shift_ptr(ref_ptr,reference_count_ptr);
    //set reference to one
    ref_ptr->ref = 1;
    //return the ptr address
    return ref_ptr->ptr;
}
//called once someone wants to release an instance of a smart pointer
static inline void release_ptr(void** ptr_){
    //checks for null pointers
    if(!ptr_ || !(*ptr_)){
        #ifdef PTR_LOG
            printf("[DEBUG]: Null Pointer passed to relase_ptr function\n");
        #endif
        return;
    }
    //shift the data pointer to start of the container
    reference_count_ptr* container = (reference_count_ptr*) offset_ptr(*ptr_);
    container->ref--; //decrement reference count of the pointer
    if(container->ref == 0){ //check if reference count is zero
        //delete pointer
        if(container->dtor){ //if dtor is not a null pointer call it
            #ifdef PTR_LOG
                printf("[DEBUG]: Calling ptr destructor at: %p\n",*ptr_);
            #endif
            container->dtor(container->ptr);
        }
        #ifdef PTR_LOG
            printf("[DEBUG]: Freeing struct container of ptr at %p\n", *ptr_);
        #endif
        free(container);//free the whole block including pointer and container
        *ptr_ = nullptr;//prevent dangling pointer and double free
    }
}

//similar to try-with or js using or RAII in c++, only supported in clang and gcc, auto function scope variable only
#if defined(__clang__) || defined(__GNUC__)
__attribute__ ((always_inline))
static inline void auto_release_ptr(void* ptr){
    release_ptr((void**)ptr);
}
#define using_ptr __attribute__((cleanup(auto_release_ptr)))
#endif


// create copy and increase reference only if shared
static inline void* get_ptr(void* ptr_){
    reference_count_ptr* container = (reference_count_ptr*) offset_ptr(ptr_);
    if(container->ptr_type == unique){//prevents giving more instances of a unique ptr
        #ifdef PTR_LOG
                printf("[DEBUG]: Passed Unique pointer in function get_ptr, operation not supported\n");
        #endif
        return nullptr; //should not allow a copy
    }
    container->ref++;//increment count
    return container->ptr; //return pointer
}
//change ownership of ptr
static inline void* move_ptr(void** ptr_){
    void* tptr = *ptr_; //copy value
    *ptr_ = nullptr; //set calling var to NULL
    return tptr; //return the original pointer value, transferring the ownership of the pointer
}

