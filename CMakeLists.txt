cmake_minimum_required(VERSION 3.28)
project(App LANGUAGES C)
set(EXECUTABLE_NAME App)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer" CACHE STRING "" FORCE)
#set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native")

add_executable(${EXECUTABLE_NAME}
    src/main.c
    src/cmdline.c
    src/error.c
    src/log.c
    src/json_parser.c
    src/model_counter.c
    src/arena_alloc.c
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(YAJL REQUIRED yajl)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${YAJL_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${YAJL_LIB})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${GLIB_LIBRARIES})
target_compile_options(${EXECUTABLE_NAME} PRIVATE ${GLIB_CFLAGS_OTHER})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        -fsanitize=address,undefined
    )
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")