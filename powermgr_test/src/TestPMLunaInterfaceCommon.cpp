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

#include <gtest/gtest.h>
#include <pbnjson.hpp>

#include "PMSCommon.h"
#include "NyxUtil.h"
#include "TestUtil.h"
#include "LunaInterfaceCommon.h"

class TestPMLunaInterfaceCommon: public ::testing::Test {
    public:
        TestPMLunaInterfaceCommon()
        {
            printf("TestPMLunaInterfaceCommon constructor entered\n");
            printf("TestPMLunaInterfaceCommon constructor exit\n");
        }

        ~TestPMLunaInterfaceCommon() {
            printf("====TestPMLunaInterfaceCommon destructor entered ===\n");
            printf("====TestPMLunaInterfaceCommon destructor exit ===\n");
        }
};

TEST_F(TestPMLunaInterfaceCommon, LunaInterfaceCommon_AddLunaCategoryContext)
{
    printf("------------------ BEGIN LunaInterfaceCommon_AddLunaCategoryContext ----------------------------\n");
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    pCxt->AddLunaCategoryContext(kPmsMsgCategoryRoot, (unsigned int)this);
    printf("------------------ END LunaInterfaceCommon_AddLunaCategoryContext----------------------------\n");
}

TEST_F(TestPMLunaInterfaceCommon, LunaInterfaceCommon_GetLunaCategoryContext)
{
    printf("------------------ BEGIN LunaInterfaceCommon_GetLunaCategoryContext ----------------------------\n");
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot);
    printf("------------------ END LunaInterfaceCommon_GetLunaCategoryContext----------------------------\n");
}