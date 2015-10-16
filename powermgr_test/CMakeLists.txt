# @@@LICENSE
#
#   Copyright (c) 2014-2015 LG Electronics Inc. All Rights Reserved
#
# LICENSE@@@


cmake_minimum_required(VERSION 2.8.7)
project(${TST_APP_NAME} C CXX)

include_directories(../include/private/common)
include_directories(../include/private/pmscore)
include_directories(../include/private/pmslogging)
include_directories(../include/private/shutdown)
include_directories(../include/private/util)
include_directories(../include/public)


file(GLOB SOURCE_FILES ./src/*.cpp)
file(GLOB RESPONDER_SOURCE_FILES ../src/*/*.cpp)

add_executable(${TST_APP_NAME} ${SOURCE_FILES} ${RESPONDER_SOURCE_FILES})

target_link_libraries(${TST_APP_NAME} ${TST_DEP} -lpthread ${WEBOS_GTEST_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${WEBOS_INSTALL_LIBEXEC}/tests/
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
