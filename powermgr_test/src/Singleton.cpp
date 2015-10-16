/* @@@LICENSE
 *
 * Copyright (c) 2014-2015 LG Electronics, Inc.
 *
 * Confidential computer software. Valid license from LG required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and
 * Technical Data for Commercial Items are licensed to the U.S. Government
 * under vendor's standard commercial license.
 *
 * LICENSE@@@
 */

#include "Singleton.hpp"

namespace SingletonNS {
std::list<Tracker *> _list;
bool _atexit_registered = false;

void destroyAll() {
    Tracker *pTracker = NULL;

    while (!_list.empty()) {
        pTracker = _list.back();
        _list.pop_back();
        delete pTracker;
    }
}
}
