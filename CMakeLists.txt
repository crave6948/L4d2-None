cmake_minimum_required(VERSION 3.9)
project(l4d2_base)  # Replace l4d2_base with the actual project name

# Disable Warning while building
set(CMAKE_PROJECT_WARNING_LEVEL "OFF")
set(CMAKE_WARN_DEPRECATED FALSE)
add_compile_options(-w)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1)
set(CMAKE_SUPPRESS_REGENERATION true)
# if(COMMAND cmake_policy)
#   cmake_policy(SET CMP0126 NEW)
# endif()
# Add /EHsc flag for MSVC globally
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# Source files
# Set the source files directory
set(SOURCE_FILES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Use GLOB_RECURSE to find all source files in subdirectories
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_FILES_DIR}/*.cpp)

# Use GLOB_RECURSE to find all source files in subdirectories
file(GLOB_RECURSE HEADER_FILES ${SOURCE_FILES_DIR}/*.h)
file(GLOB_RECURSE SOURCE_FILES1 ${SOURCE_FILES_DIR}/*.hpp)
file(GLOB_RECURSE SOURCE_FILES2 ${SOURCE_FILES_DIR}/*.c)

# Add the library target
add_library(l4d2_base SHARED ${SOURCE_FILES} ${HEADER_FILES} ${SOURCE_FILES1} ${SOURCE_FILES2})

# additional libraries

# Set C++ standard
set_target_properties(l4d2_base PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "pol-crave-git" # Replace pol with the actual library name
)

# Add custom command to reconfigure and rebuild project
add_custom_target(reconfigure
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Reconfiguring and rebuilding project"
)