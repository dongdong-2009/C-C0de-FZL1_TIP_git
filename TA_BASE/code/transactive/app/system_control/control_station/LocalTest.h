#pragma once
#include "core/utilities/src/RunParams.h"
#define IF_LOCAL_TEST if (TA_Base_Core::RunParams::getInstance().isSet("LocalTest"))
#define IF_NOT_LOCAL_TEST if (!TA_Base_Core::RunParams::getInstance().isSet("LocalTest"))


struct LocalTest
{
    static void test_duty();
    static void test_task_manager();
};
