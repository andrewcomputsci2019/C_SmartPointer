cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
message("running smart_ptr cmake file")
#make clangd happy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#project name declaration
project(SmartPointer VERSION 1.0.1 LANGUAGES C)
#install artifacts
include(GNUInstallDirs)
#make target
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
#include dirs 
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
#set c_standard
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
set(PTR_LOGGING OFF CACHE BOOL "ENABLES DEBUG INFORMATION DURING RUN TIME")
if(${PTR_LOGGING})
    message("debug messaging enabled for smart pointers")
    target_compile_definitions(${PROJECT_NAME} INTERFACE _DEBUG_PTR)
endif()

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION include)

#[[add_executable(DEMO_EXE)
target_sources(DEMO_EXE PUBLIC ${PROJECT_SOURCE_DIR}/demo/main.c)
target_link_libraries(DEMO_EXE PUBLIC SmartPointer)]]

message("finished running smart_pointer lib file")
