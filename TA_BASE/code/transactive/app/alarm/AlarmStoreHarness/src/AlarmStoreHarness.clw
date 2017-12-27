; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConfigDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "AlarmStoreHarness.h"

ClassCount=4
Class1=CAlarmStoreHarnessApp
Class2=CAlarmStoreHarnessDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_ALARMSTOREHARNESS_DIALOG
Resource4=IDD_CONFIG_DIALOG (English (U.S.))
Resource5=IDD_ALARMSTOREHARNESS_DIALOG (English (U.S.))
Class4=CConfigDlg
Resource6=IDD_ABOUTBOX (English (U.S.))

[CLS:CAlarmStoreHarnessApp]
Type=0
HeaderFile=AlarmStoreHarness.h
ImplementationFile=AlarmStoreHarness.cpp
Filter=N

[CLS:CAlarmStoreHarnessDlg]
Type=0
HeaderFile=AlarmStoreHarnessDlg.h
ImplementationFile=AlarmStoreHarnessDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAlarmStoreHarnessDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=AlarmStoreHarnessDlg.h
ImplementationFile=AlarmStoreHarnessDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_ALARMSTOREHARNESS_DIALOG]
Type=1
Class=CAlarmStoreHarnessDlg
ControlCount=18
Control1=IDC_BTN_CFG,button,1342242816
Control2=IDC_BTN_CFGFILE,button,1342242816
Control3=IDC_EDIT_CFGFILE,edit,1350633600
Control4=IDC_EDIT_ACTION_INFO,edit,1353779332
Control5=IDC_BTN_START,button,1342242816
Control6=IDC_BTN_RELOAD,button,1342242816
Control7=IDC_EDIT_MESSAGING_INTERVAL,edit,1350631552
Control8=IDC_EDIT_DIRECT_INTERVAL,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_BTN_APPLY,button,1342242816
Control12=IDC_STATIC,button,1342177287
Control13=IDC_BTN_GETALARM,button,1342242816
Control14=IDC_EDIT_ALARM_INFO,edit,1350633604
Control15=IDC_BTN_NEWALARM,button,1073807360
Control16=IDC_BTN_ACKALARM,button,1073807360
Control17=IDC_BTN_CLOSEALARM,button,1073807360
Control18=IDC_BTN_CLOSEALL,button,1342242816

[DLG:IDD_ALARMSTOREHARNESS_DIALOG (English (U.S.))]
Type=1
Class=CAlarmStoreHarnessDlg
ControlCount=40
Control1=IDC_BTN_CFG,button,1342242816
Control2=IDC_BTN_CFGFILE,button,1342242816
Control3=IDC_EDIT_CFGFILE,edit,1350633600
Control4=IDC_BTN_START,button,1342242816
Control5=IDC_BTN_RELOAD,button,1342242816
Control6=IDC_EDIT_MESSAGING_INTERVAL,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BTN_CLOSEALL,button,1342242816
Control9=IDC_BTN_OUTPUT,button,1073807360
Control10=IDC_BTN_OUTPUT2,button,1073807360
Control11=IDC_COMBO_SORT,combobox,1344339971
Control12=IDC_COMBO_FILTER,combobox,1344339971
Control13=IDC_STATIC,static,1342308865
Control14=IDC_STATIC,static,1342308865
Control15=IDC_ALARM_LIST,SysListView32,1342249097
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_TOTAL_ALARMS,edit,1350633600
Control19=IDC_NOT_ACK_ALARMS,edit,1350633600
Control20=IDC_BTN_SORT,button,1342242816
Control21=IDC_BTN_FILTER,button,1342242816
Control22=IDC_BTN_REBUILD_ALARM,button,1342242816
Control23=IDC_STATIC,button,1342177287
Control24=IDC_EDIT_SORT_INTERVAL,edit,1350631552
Control25=IDC_EDIT_FILTER_INTERVAL,edit,1350631552
Control26=IDC_EDIT_REBUILD_INTERVAL,edit,1350631552
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,button,1342177287
Control31=IDC_BTN_CONFIG,button,1342242816
Control32=IDC_BTN_APPLY,button,1073807360
Control33=IDC_BTN_CHECK_SM,button,1342242816
Control34=IDC_CHK_ALARM,button,1476460547
Control35=IDC_CHK_SORT,button,1342242819
Control36=IDC_CHK_FILTER,button,1342242819
Control37=IDC_CHK_REBUILD,button,1342242819
Control38=IDC_COMBO_LOCATION,combobox,1344339971
Control39=IDC_STATIC,static,1342308352
Control40=IDC_BTN_REMOVE_LA,button,1342242816

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CConfigDlg]
Type=0
HeaderFile=configdlg.h
ImplementationFile=configdlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHK_1
VirtualFilter=dWC

[DLG:IDD_CONFIG_DIALOG (English (U.S.))]
Type=1
Class=CConfigDlg
ControlCount=10
Control1=IDC_BTN_OK,button,1342242816
Control2=IDC_BTN_CANCEL,button,1342242816
Control3=IDC_EDIT_SEVERITY,edit,1350631552
Control4=IDC_EDIT_LOCATION,edit,1350631552
Control5=IDC_EDIT_ASSET,edit,1350631552
Control6=IDC_EDIT_DESCRIPTION,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

