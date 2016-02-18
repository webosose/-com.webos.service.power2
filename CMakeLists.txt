# @@@LICENSE
#
#   Copyright (c) 2015-2016 LG Electronics Inc. All Rights Reserved
#
# LICENSE@@@

cmake_minimum_required(VERSION 2.8.7)
project(powermgr)

############################################################
############################################################
function(webos_add_test target_name app_dep)
    get_property(SOURCES TARGET ${target_name} PROPERTY "SOURCES")
    set(TST_SRC)
    foreach(SRC ${SOURCES})
        if(NOT ${SRC} MATCHES "main.cpp")
            set(TST_SRC ${TST_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
        endif()
    endforeach()

    set (TST_APP_NAME ${target_name}_test)
    set (TST_DEP ${app_dep} ${ARGN} ${GTEST_LIBRARIES} )
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_subdirectory(${target_name}_test)
endfunction()
############################################################
############################################################

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC4)
webos_component(1 0 0)

include(FindPkgConfig)

pkg_check_modules(LS2 REQUIRED luna-service2)
include_directories(${LS2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LS2_CFLAGS_OTHER})

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS_OTHER})

pkg_check_modules(JSON REQUIRED json-c)
include_directories(${JSON_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${JSON_CFLAGS_OTHER})

pkg_check_modules(PMLOGLIB REQUIRED PmLogLib)
include_directories(${PMLOGLIB_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PMLOGLIB_CFLAGS_OTHER})

pkg_check_modules(NYXLIB REQUIRED nyx)
include_directories(${NYXLIB_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${NYXLIB_CFLAGS_OTHER})

pkg_check_modules(PBNJSON_CPP REQUIRED pbnjson_cpp)
include_directories(${PBNJSON_CPP_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_CPP_CFLAGS_OTHER})

pkg_check_modules(PMSSUPPORTAPI REQUIRED pms-support-api)
include_directories(${PMSSUPPORTAPI_INCLUDE_DIRS})

include_directories(include/private/pmscore)
include_directories(include/private/pmslogging)
include_directories(include/private/shutdown)
include_directories(include/private/util)
include_directories(include/private/common)
include_directories(include/private/sleepd)
include_directories(include/public/powerd)
include_directories(include/internal)

add_definitions(-DSLEEPD_BACKWARD_COMPATIBILITY)
add_definitions(-DENABLE_WAKELOCK_FOR_SLEEP_STATE)
add_definitions(-DENABLE_POWEROFF_REBOOT_SIGNAL)

if(TV_BUILD)
    add_definitions(-DTV_BUILD_TRUE)
endif()

file(GLOB SOURCE_FILES src/*/*.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -Werror")

add_executable(powermgr ${SOURCE_FILES}
                          src/main.cpp
                          )

set(LIBS ${LS2_LDFLAGS}
         ${GLIB2_LDFLAGS}
         ${PMLOGLIB_LDFLAGS}
         ${PBNJSON_CPP_LDFLAGS}
         ${NYXLIB_LDFLAGS}
         luna-service2++
         pmssupport
         pthread)

target_link_libraries(powermgr ${LIBS})

install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION sbin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE)


install(PROGRAMS scripts/public/suspend_action DESTINATION ${WEBOS_INSTALL_SBINDIR})
webos_build_configured_file(files/conf/pmlog/powermgr.conf SYSCONFDIR pmlog.d)

# OYK_2013_07_09 : Add MSM suspend settings
install(PROGRAMS scripts/public/msm_suspend DESTINATION ${WEBOS_INSTALL_SBINDIR})
# OYK_2013_08_12 : Add cpufreq governor settings
install(PROGRAMS scripts/public/set_cpufreq_ondemand_governor DESTINATION ${WEBOS_INSTALL_SBINDIR})
# OYK_2013_08_28 : Add alarm.xml
install(PROGRAMS scripts/public/alarms.xml DESTINATION ${WEBOS_INSTALL_LOCALSTATEDIR})
# OYK_2014_02_25 : Add cpufreq governor parameters
install(PROGRAMS scripts/public/cpufreq_governor_parameters DESTINATION ${WEBOS_INSTALL_SBINDIR})
install(PROGRAMS scripts/public/set_cpu_online DESTINATION ${WEBOS_INSTALL_SBINDIR})
install(PROGRAMS scripts/public/set_cpu_offline DESTINATION ${WEBOS_INSTALL_SBINDIR})
install(PROGRAMS scripts/public/update_timestamp DESTINATION ${WEBOS_INSTALL_SBINDIR})

webos_build_daemon()
webos_build_system_bus_files()

if(!TV_BUILD)
    add_subdirectory(libpowerd)
endif()

#unit test for power manager service(powermgr)
#for gtest framework
#webos_use_gtest()
#webos_add_test(powermgr ${LIBS})

