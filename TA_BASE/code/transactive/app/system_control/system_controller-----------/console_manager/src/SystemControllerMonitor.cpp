#include "SystemControllerMonitor.h"


SystemControllerMonitor::SystemControllerMonitor()
{
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerMonitor");
    activateServant();
}

