/* @@@LICENSE
 *
 * Copyright (c) 2017 LG Electronics, Inc.
 *
 * Confidential computer software. Valid license from LG required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and
 * Technical Data for Commercial Items are licensed to the U.S. Government
 * under vendor's standard commercial license.
 *
 * LICENSE@@@
 */

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <algorithm>

namespace SingletonNS {
//! Base class for manage singleton instances
class Tracker {
public:
    virtual ~Tracker() {
    }
};

extern std::list<Tracker *> _list;
extern bool _atexit_registered;

extern void destroyAll();
}

//! Singleton template class
template<typename TYPE>
class Singleton {
public:
    /*! get singleton instance
     instance will be destroy when program ends automatically
     */
    static TYPE &instance() {
        if (!_instance) {
            _instance = new TYPE;
            Singleton<TYPE>::track();
        }

        return *_instance;
    }

    //! destroy singleton instance
    static void destroy() {
        if (!_instance) {
            return;
        }

        Singleton<TYPE>::untrack();
    }

    /*! replace singleton instance
     it not recommanded call this function if not for test
     */
    template<typename REPLACE_TYPE>
    static void replace() {
        destroy();

        _instance = new REPLACE_TYPE;
        Singleton<TYPE>::track();
    }

private:

    //! Implement class for manage singleton instances
    template<typename T>
    class TrackerImpl: public SingletonNS::Tracker {
    public:
        TrackerImpl(T *_p) :
                m_p(_p) {
        }
        ~TrackerImpl() {
            delete m_p;
            Singleton<T>::_instance = 0;
        }
        T *m_p;
    };

    //! Helper class for find singleton track from list
    template<typename T>
    class TrackerFinder {
    public:
        TrackerFinder(T *_p) :
                m_p(_p) {
        }
        bool operator()(SingletonNS::Tracker *p) {
            TrackerImpl<T> *pImpl = static_cast<TrackerImpl<T>*>(p);

            if (!pImpl) {
                return false;
            }

            return (pImpl->m_p == m_p);
        }

        T *m_p;
    };

protected:
    friend class TrackerImpl<TYPE> ;

    //! Constructor
    Singleton() {
    }

    //! Destructor
    virtual ~Singleton() {
    }

private:
    //! Track this instance
    static void track() {
        SingletonNS::Tracker *pTracker = new TrackerImpl<TYPE>(
                Singleton<TYPE>::_instance);

        if (pTracker) {
            SingletonNS::_list.push_back(pTracker);

            if (!SingletonNS::_atexit_registered) {
                SingletonNS::_atexit_registered = true;
                atexit(SingletonNS::destroyAll);
            }
        }
    }

    //! Untrack this instance
    static void untrack() {
        auto it = std::find_if(SingletonNS::_list.begin(),
                SingletonNS::_list.end(),
                TrackerFinder<TYPE>(Singleton<TYPE>::_instance));

        if (it != SingletonNS::_list.end()) {
            delete *it;
            SingletonNS::_list.erase(it);
        }
    }

    static TYPE *_instance;
};

template<typename TYPE>
TYPE *Singleton<TYPE>::_instance = NULL;

#endif
