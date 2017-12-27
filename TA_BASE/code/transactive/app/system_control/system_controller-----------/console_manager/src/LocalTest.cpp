#include "StdAfx.h"
#include "LocalTest.h"
#include "ControlStationRootDialog.h"
#include "DutyManager.h"


void LocalTest::test_duty()
{
    DutyManager::SubsystemDetail subsystem_detail;
    subsystem_detail.regionName = "Region";
    subsystem_detail.subsystemName.push_back("Subsystem");

    DutyManager::DutyDetail duty_detail;
    duty_detail.operatorName = "Operator";
    duty_detail.profileName = "Profile";
    duty_detail.locationName = "Location";
    duty_detail.subsystem.push_back(subsystem_detail);

    DutyManager::DutyNotificationDetail notification_detail;
    notification_detail.gained.push_back(duty_detail);
    notification_detail.denied.push_back(duty_detail);
    notification_detail.lost.push_back(duty_detail);
    notification_detail.changed.push_back(duty_detail);

    DutyManager::DutyRequestDetail request_detail;
    request_detail.duty = duty_detail;
    request_detail.uniqueId = 1;

    ControlStationRootDialog::getInstance().PostMessage(WM_DUTY_CHANGE,  reinterpret_cast<WPARAM>(new DutyManager::DutyNotificationDetail(notification_detail)), 1);
    ControlStationRootDialog::getInstance().PostMessage(WM_DUTY_REQUEST,  reinterpret_cast<WPARAM>(new DutyManager::DutyRequestDetail(request_detail)), 1);
}


void LocalTest::test_task_manager()
{
    ControlStationRootDialog::getInstance().PostMessage(WM_SHOW_TASKMANAGER);
}
