cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#make clangd happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#project name decloration
project(SmartPointer VERSION 0.1 LANGUAGES C)
#make target
add_executable(${PROJECT_NAME})
#set c_standard
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
set(PTR_LOGGING OFF CACHE BOOL "ENABLES DEBUG INFORMATION DURING RUN TIME")
if(${PTR_LOGGING})
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
endif()
#add src dir and include dir
set(SRC src/main.c src/SmartPointer.c) #add src here
target_sources(${PROJECT_NAME} PUBLIC ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
