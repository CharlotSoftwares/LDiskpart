cmake_minimum_required(VERSION 3.10)
project(diskpart C)

# Use C99 and enable warnings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

# Find libparted
find_package(PkgConfig REQUIRED)
pkg_check_modules(PARTED REQUIRED libparted)

# Source files
set(SOURCES
    active.c
    add.c
    assign.c
    attach.c
    attributes.c
    automount.c
    break.c
    clean.c
    compact.c
    convert.c
    create.c
    delete.c
    detach.c
    detail.c
    diskpart.c
    dump.c
    expand.c
    extend.c
    filesystems.c
    format.c
    gpt.c
    help.c
    import.c
    inactive.c
    interpreter.c
    list.c
    merge.c
    misc.c
    offline.c
    online.c
    partlist.c
    recover.c
    remove.c
    repair.c
    rescan.c
    retain.c
    san.c
    select.c
    setid.c
    shrink.c
    uniqueid.c
)

# Includes
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${PARTED_INCLUDE_DIRS}
)

# Executable
add_executable(ldiskpart ${SOURCES})

# Link libparted
target_link_libraries(ldiskpart ${PARTED_LIBRARIES})

# Optional: Show libparted include and lib paths (debug)
message(STATUS "libparted include dirs: ${PARTED_INCLUDE_DIRS}")
message(STATUS "libparted libraries: ${PARTED_LIBRARIES}")
