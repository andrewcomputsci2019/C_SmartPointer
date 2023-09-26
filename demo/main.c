#include <stdio.h>
#include <string.h>
#include <SmartPointer.h>
//Destructor of a string pointer
void dtor_String(void* ptr){
    printf("Final Value of the pointer is %s\n", (char*)ptr);
}
void dtor_File(void* fp){
    //in this case, we have wrapped our data in two pointers, so we convert
    //the given pointer into a double pointer and grab the single FILE pointer which we did not allocate
    FILE* fd = *(FILE**)fp;
    //could put a closing message in a file once its closes
    fputs("Closing File!\n",fd);
    fclose(fd);
    printf("closed file\n");
}

int main(){
    //basic operations
    int* ptr = allocate_ptr(sizeof(int),nullptr,PTR_UNIQUE); //using_ptr makes sure that this pointer is freed out of scope
    *ptr = 5;
    int* ptr2 = get_ptr(ptr);//this should not work
    printf("Verify that copy failed: %p\n", ptr2);
    printf("Pointer value is %d\n",*ptr);
    ptr2 = move_ptr((void**)&ptr); //this will transfer ownership and make ptr2 carry the value
    printf("Verify that move succeed: %p\n", ptr2);
    printf("Verifying that pointer value is still 5: %d\n",*ptr2);
    printf("Verify that pointer is null %p\n",ptr);
    release_ptr((void**)&ptr2);
    printf("Verify pointer is null: %p",ptr2);
    { //highlight how using_ptr tag works
        using_ptr char *str = allocate_ptr(sizeof(char) * 35, dtor_String, PTR_SHARED);
        str = strcpy(str, "Smart String\n");
        printf("This is a string %s", str);
    }//once reached here *str is released and then freed

    //advanced operations and ideas of using these smart pointers
    {
        using_ptr FILE **filePtr = allocate_ptr(sizeof(FILE *), dtor_File, PTR_UNIQUE);
        *filePtr = fopen("./test.txt", "w");
        fputs("This is an operation performed by a smart pointer file pointer!\n", *filePtr);
    }
    //above example but manually decrementing the pointer
    FILE ** pFile = allocate_ptr(sizeof(FILE*),dtor_File,PTR_UNIQUE);
    *pFile = fopen("./test1.txt","w");
    fputs("This is a second file using the smart pointers\n",*pFile);
    printf("Closing file manually by releasing smart pointer reference\n");
    release_ptr((void**)&pFile);
}