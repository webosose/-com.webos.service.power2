// @@@LICENSE
//
//      Copyright (c) 2017 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#ifndef PMS_ERRORS_H
#define PMS_ERRORS_H

/**
 * @typedef uint32_t PmsErrorCode_t
 * @brief error codes are 32-bit unsigned integers.
 *
 * see #ePmsErrCommon and other error categories.
 */
#include <stdint.h>

typedef uint32_t PmsErrorCode_t; /**< 32 bit error code */

/*
 * @note Each feature implementer should use the feature base error codes to define
 * feature specific errors in their respective header files.
 **/

/**
 * @enum ePmsErrCommon
 * @brief Defines the common error codes used in TIL
 *
 * @note kPmsErrNetwork should be used in conjunction with another field within
 * the PmsMessage response or event so that the actual value can be returned.
 * Base starts from kPmsErrCategoryCommon
 */
enum ePmsErrCommon
{
    kPmsSuccess,                            /**< Pms Success */
    kPmsErrPower2Busy,                      /**< com.webos.service.power2 is busy */
    kPmsErrInvalidJSONFormat,               /**< Invalid JSON format */
    kPmsErrClientNotRegistered,             /**< The client is not registered */
    kPmsErrClientAlreadyRegistered,         /**< The client is already registered */
    kPmsErrResponseTooLate,                 /**< The response is too late */
    kPmsErrTimerNotRunning,                 /**< The timer is not running */
    kPmsErrUnknown,                         /**< Unknown error */
};

/* * Looks up the  string that matches the error code.
 *
 * @param [in] error PmsErrorCode_t value to look up.
 * @return pointer to a fixed string buffer.
 */
const char* GetPmsErrorString(PmsErrorCode_t error);

#endif //PMS_ERRORS_H
