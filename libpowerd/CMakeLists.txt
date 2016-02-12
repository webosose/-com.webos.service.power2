# @@@LICENSE
#
#      Copyright (c) 2015 LG Electronics, Inc.
#
# Confidential computer software. Valid license from LG required for
# possession, use or copying. Consistent with FAR 12.211 and 12.212,
# Commercial Computer Software, Computer Software Documentation, and
# Technical Data for Commercial Items are licensed to the U.S. Government
# under vendor's standard commercial license.
#
# LICENSE@@@

#
# powerd/libpowerd/CMakeLists.txt
#

# Build the libpowerd shared library

webos_build_pkgconfig(${CMAKE_SOURCE_DIR}/files/pkgconfig/powerd)

webos_add_compiler_flags(ALL -fPIC -DSTACK_GROWS_DOWN)

add_library(libpowerd SHARED clock.c commands.c init.c wait.c)
target_link_libraries(libpowerd ${GLIB2_LDFLAGS} ${LS2_LDFLAGS} ${JSON_LDFLAGS} pthread rt)
webos_build_library(NAME libpowerd)
