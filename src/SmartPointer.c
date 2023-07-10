//headers
#include <SmartPointer.h>

//this function should allocate the reference_count_ptr struct
//and then this function should return just the ptr of the data
void* allocate_ptr(size_t size, void (*func_ptr)(void*), pointer_type ptr_type_){
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
        return nullptr;
    }
    //set destructor var in struct
    ref_ptr->dtor = func_ptr;
    //set ptr type unique or shared
    ref_ptr->ptr_type = ptr_type_;
    //align the ptr to the end of the struct block
    ref_ptr->ptr = (void*)shift(ref_ptr,reference_count_ptr);
    //set reference to one
    ref_ptr->ref = 1;
    //return the ptr address
    return ref_ptr->ptr;
}


void release_ptr(void** ptr_) {
    //checks for null pointers
    if(!ptr_ || !(*ptr_)){
        #ifdef PTR_LOG
            printf("[DEBUG]: Null Pointer passed to relase_ptr function\n");
        #endif
        return;
    }
    reference_count_ptr* container = (reference_count_ptr*) offset(*ptr_);
    container->ref--;
    if(container->ref == 0){
        //delete pointer
        if(container->dtor){
             #ifdef PTR_LOG
                printf("[DEBUG]: Calling ptr destructor at: %p\n",*ptr_);
            #endif
            container->dtor(container->ptr);
        }
        #ifdef PTR_LOG
            printf("[DEBUG]: Freeing struct container of ptr at %p\n", *ptr_);
        #endif
        free(container);
        *ptr_ = nullptr;
    }
}

void* get_ptr(void* ptr_){
    reference_count_ptr* container = (reference_count_ptr*) offset(ptr_);
    if(container->ptr_type == unique){
        return nullptr; //should not allow a copy
    }
    container->ref++;
    return container->ptr;
}

void* move_ptr(void** ptr_){ //calling move is the idea of transferring ownership
    void* tptr = *ptr_; //copy value 
    *ptr_ = nullptr; //set calling var to nullptr
    return tptr; //return the orignal pointer value, transferring the ownership of the pointer
}
