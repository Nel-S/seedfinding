# Thank you to Mzeen for making the original CMakeLists.txt that this was ultimately adapted from.
cmake_minimum_required(VERSION 3.5)

if (TARGET seedfinding)
	return()
endif()

project(seedfinding)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if (WIN32)
	set(CMAKE_C_FLAGS "-D_WIN32")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fwrapv -ffast-math -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG")

# Add static submodules
if (NOT TARGET chunkbiomes)
	add_subdirectory(chunkbiomes)
endif()
if (NOT TARGET utilities)
	add_subdirectory(utilities)
endif()

set(EXECUTABLES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${prog}")

if (backend STREQUAL "MPI")
	set(EXECUTABLES_LIST ${EXECUTABLES_LIST} "${CMAKE_CURRENT_SOURCE_DIR}/core/Backends/(Unfinished) MPI.c")
elseif (backend STREQUAL "Pthreads")
	set(EXECUTABLES_LIST ${EXECUTABLES_LIST} "${CMAKE_CURRENT_SOURCE_DIR}/core/Backends/Pthreads.c")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
else()
	set(EXECUTABLES_LIST ${EXECUTABLES_LIST} "${CMAKE_CURRENT_SOURCE_DIR}/core/Backends/Basic.c")
endif()

add_executable(seedfinding ${EXECUTABLES_LIST})

target_link_libraries(seedfinding INTERFACE core
	PRIVATE chunkbiomes utilities
)
target_include_directories(seedfinding PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/core"
	"${CMAKE_CURRENT_SOURCE_DIR}/Chunkbiomes"
	"${CMAKE_CURRENT_SOURCE_DIR}/utilities"
)

install(TARGETS seedfinding
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)