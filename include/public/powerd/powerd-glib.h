// @@@LICENSE
//
//      Copyright (c) 2015 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#ifndef _LIBPOWERD_GLIB_H_
#define _LIBPOWERD_GLIB_H_

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

void PowerdGmainAttach(GMainLoop *mainLoop);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //  _LIBPOWERD_GLIB_H_
