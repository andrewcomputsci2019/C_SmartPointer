# C_SmartPointer
___
It Is a library that strives to provide a simple way of creating a memory safe environment in C by providing reference counting 
pointers. 
## Core Features

* `PTR_UNIQUE` and `PTR_SHARED` pointer types
*  Destructor support for complex pointer types
*  `using_ptr` Auto scope cleaning pointers similar to try with resource and RAII 
*  Cross-platform support --> linux kernel 6, macos ventura

___
## How to install
Either use the cmake build script provided or just copy the header into your project
### Using cmake
add the cmake script as a subdirectory of your project and use `target_link_libraries(your_target PUBLIC SmartPointer)`
after adding the subdirectory in your own cmake build script\
You can also build a provided demo by adding the cmake argument `-DPTR_DEMO:BOOL=ON`

___

## Usage
All code run below has debug flag enabled, `-D _DEBUG_PTR` or cmake `-DPTR_LOGGING:BOOL=ON`

* Most basic way of using the smart pointers
```c
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
```
output of above code
```shell
[DEBUG]: Function call of pointer_type: Unique and size of: 4 was not provided destructor function pointer
[DEBUG]: allocated memory for container at 0x23f76b0
[DEBUG]: Passed Unique pointer in function get_ptr, operation not supported
Verify that copy failed: (nil)
Pointer value is 5
Verify that move succeed: 0x23f76c0
Verifying that pointer value is still 5: 5
Verify that pointer is null (nil)
[DEBUG]: Freeing struct container of ptr at 0x23f76c0
Verify pointer is null: (nil)
```
* using the auto release version of the smart pointer
```c
    //Destructor of a string pointer
    void dtor_String(void* ptr){
    printf("Final Value of the pointer is %s\n", (char*)ptr);
    }
    ...
    { //highlight how using_ptr tag works by creating a scope
            using_ptr char *str = allocate_ptr(sizeof(char) * 35, dtor_String, PTR_SHARED);
            str = strcpy(str, "Smart String\n");
            printf("This is a string %s", str);
    }//once reached here *str is released automatically and then freed
```
output of above code
```shell
[DEBUG]: allocated memory for container at 0x23f76d0
This is a string Smart String
[DEBUG]: Calling ptr destructor at: 0x23f76e0
Final Value of the pointer is Smart String
```
* advance usage of smart pointers and destructors
```c
void dtor_File(void* fp){
    //in this case, we have wrapped our data in two pointers, so we convert
    //the given pointer into a double pointer and grab the single FILE pointer which we did not allocate
    FILE* fd = *(FILE**)fp;
    //could put a closing message in a file once its closes
    fputs("Closing File!\n",fd);
    fclose(fd);
    printf("closed file\n");
}
...
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
````
output of above code
```shell
[DEBUG]: Freeing struct container of ptr at 0x23f76e0
[DEBUG]: allocated memory for container at 0x23f76b0
[DEBUG]: Calling ptr destructor at: 0x23f76c0
closed file
[DEBUG]: Freeing struct container of ptr at 0x23f76c0
[DEBUG]: allocated memory for container at 0x23f76b0
Closing file manually by releasing smart pointer reference
[DEBUG]: Calling ptr destructor at: 0x23f76c0
closed file
[DEBUG]: Freeing struct container of ptr at 0x23f76c0
```
test.txt
```text
This is an operation performed by a smart pointer file pointer!
Closing File!
```
test1.txt
```text
This is a second file using the smart pointers
Closing File!
```
___
### limitations
as of current, the library does not support custom memory allocators.
    