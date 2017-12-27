; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "planviewer.h"
LastPage=0

ClassCount=34
Class1=CBlankForm
Class2=CMainFrame
Class3=CPlanCatForm
Class4=CPlanForm
Class5=CPlanStepForm
Class6=CPlansTreeView
Class7=CPlanViewerApp
Class8=CPlanViewerDlg
Class9=CPlanViewerDoc
Class10=CSplitStepView
Class11=CStepsListComboBox
Class12=CStepsListEdit
Class13=CStepsListView
Class14=CStepsSplitWnd

ResourceCount=23
Resource1=IDD_PLANCAT_FORM
Resource2=IDR_CATEDIT
Resource3=IDD_PLANVIEWER_DIALOG (English (U.S.))
Resource4=IDR_PLANCTRL
Resource5=IDD_BLANK_FORM
Resource6=IDD_ABOUTBOX (English (U.S.))
Resource7=IDD_PLAN_FORM
Resource8=IDD_PLANSTEP_FORM
Resource9=IDR_STEP_TOOLBAR (English (U.S.))
Resource10=IDD_PLAN_DETAILS_DIALOG
Class15=CPlanExecControlDlg
Resource11=IDR_PLANEDIT
Class16=CYesNoDlg
Resource12=IDD_STEPLIST_DIALOG
Class17=CDelayDlg
Class18=CStepCustomDlg
Resource13=IDR_PLAN_TOOLBAR (English (U.S.))
Class19=CStepParameterDlg
Resource14=IDD_ALLPLANS_FORM
LastClass=CPlanForm
Class20=CAllPlansForm
Resource15=IDD_ACTIVEPLANS_FORM
Class21=CActivePlansForm
Resource16=IDD_PLANFLOW_DIALOG
Class22=CUnapprovedPlansForm
Resource17=IDR_PLANCONTROL
Class23=CPlanDetailsDlg
Resource18=IDD_STEPPARAM_DIALOG
Resource19=IDR_CATEGORY_TOOLBAR (English (U.S.))
Class24=CPlanFlowDlg
Class25=CStepListDlg
Class26=CNotes
Class27=CPlanStepEditDoc
Class28=CPlanStepEditMainFrame
Class29=CEditableListCtrl
Class30=CListCellCheckBox
Resource20=IDD_UNAPPROVEDPLANS_FORM
Class31=CPlanCatEditMainFrame
Class32=CPlanCatEditDoc
Resource21=IDR_STEPEDIT
Class33=CPlanEditMainFrame
Class34=CApprovedPlansForm
Resource22=IDR_MAINFRAME (English (U.S.))
Resource23=IDD_APPROVEDPLANS_FORM

[CLS:CBlankForm]
Type=0
BaseClass=CFormView
HeaderFile=BlankForm.h
ImplementationFile=BlankForm.cpp
LastObject=CBlankForm
Filter=D

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame
Filter=T
VirtualFilter=fWC

[CLS:CPlanCatForm]
Type=0
BaseClass=CFormView
HeaderFile=PlanCatForm.h
ImplementationFile=PlanCatForm.cpp
LastObject=IDC_NOTES_PICT
Filter=D
VirtualFilter=VWC

[CLS:CPlanForm]
Type=0
BaseClass=CFormView
HeaderFile=PlanForm.h
ImplementationFile=PlanForm.cpp
LastObject=CPlanForm
Filter=D
VirtualFilter=VWC

[CLS:CPlanStepForm]
Type=0
BaseClass=CFormView
HeaderFile=PlanStepForm.h
ImplementationFile=PlanStepForm.cpp
LastObject=IDC_STEPPARAM_LIST
Filter=D
VirtualFilter=VWC

[CLS:CPlansTreeView]
Type=0
BaseClass=CTreeView
HeaderFile=PlansTreeView.h
ImplementationFile=PlansTreeView.cpp
Filter=C
VirtualFilter=VWC
LastObject=ID_PLAN_CONTROL

[CLS:CPlanViewerApp]
Type=0
BaseClass=CWinApp
HeaderFile=PlanViewer.h
ImplementationFile=PlanViewer.cpp

[CLS:CPlanViewerDlg]
Type=0
BaseClass=CDialog
HeaderFile=PlanViewerDlg.h
ImplementationFile=PlanViewerDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CPlanViewerDlg

[CLS:CPlanViewerDoc]
Type=0
BaseClass=CDocument
HeaderFile=PlanViewerDoc.h
ImplementationFile=PlanViewerDoc.cpp

[CLS:CSplitStepView]
Type=0
BaseClass=CView
HeaderFile=SplitStepView.h
ImplementationFile=SplitStepView.cpp

[CLS:CStepsListComboBox]
Type=0
BaseClass=CComboBox
HeaderFile=StepsListComboBox.h
ImplementationFile=StepsListComboBox.cpp

[CLS:CStepsListEdit]
Type=0
BaseClass=CEdit
HeaderFile=StepsListEdit.h
ImplementationFile=StepsListEdit.cpp

[CLS:CStepsListView]
Type=0
BaseClass=CListView
HeaderFile=StepsListView.h
ImplementationFile=StepsListView.cpp

[CLS:CStepsSplitWnd]
Type=0
BaseClass=CFrameWnd
HeaderFile=StepsSplitWnd.h
ImplementationFile=StepsSplitWnd.cpp

[DLG:IDD_BLANK_FORM]
Type=1
Class=CBlankForm
ControlCount=0

[DLG:IDD_PLANCAT_FORM]
Type=1
Class=CPlanCatForm
ControlCount=6
Control1=IDC_CATEGORY_HEADER,static,1342308864
Control2=IDC_NAME_LABEL,static,1342308352
Control3=IDC_CATEGORY_NAME_EDIT,edit,1350633600
Control4=IDC_DESC_LABEL,static,1342308352
Control5=IDC_CATEGORY_DESCRIPTION_EDIT,edit,1350633540
Control6=IDC_NOTES_PICT,static,1342308611

[DLG:IDD_PLAN_FORM]
Type=1
Class=CPlanForm
ControlCount=1
Control1=IDC_PLANEDIT_TAB,SysTabControl32,1342177280

[DLG:IDD_PLANSTEP_FORM]
Type=1
Class=CPlanStepForm
ControlCount=22
Control1=IDC_NAME_LABEL,static,1342308352
Control2=IDC_STEP_NAME_EDIT,edit,1350633600
Control3=IDC_STEPDELAY_LABEL,static,1342308352
Control4=IDC_MINUTE_EDIT,edit,1350641664
Control5=IDC_MINUTE_SPIN,msctls_updown32,1476395063
Control6=IDC_SECOND_EDIT,edit,1350641664
Control7=IDC_SECOND_SPIN,msctls_updown32,1476395063
Control8=IDC_DESC_LABEL,static,1342308352
Control9=IDC_STEP_DESCRIPTION_EDIT,edit,1350633540
Control10=IDC_STEPPARAM_LIST,SysListView32,1350631429
Control11=IDC_STEPTYPE_LABEL,static,1342308352
Control12=IDC_STEPDETAILS_BOX,button,1342177287
Control13=IDC_STEPTYPE_EDIT,edit,1350568064
Control14=IDC_STEPPARAM_BOX,button,1342177287
Control15=IDC_STEPEXEC_BOX,button,1342177287
Control16=IDC_STEPSKIPPABLE_CHECK,button,1476460547
Control17=IDC_STEPSKIP_CHECK,button,1476460547
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308864
Control21=IDC_STATIC,static,1342308864
Control22=IDC_NOTES_PICT,static,1073742083

[DLG:IDD_PLANVIEWER_DIALOG]
Type=1
Class=CPlanViewerDlg

[DLG:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_PLANVIEWER_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=0

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_CAT_NEW
Command2=ID_SEP_CAT
Command3=ID_SEP_PLAN1
Command4=ID_SEP_PLAN2
Command5=ID_SEP_PLAN3
Command6=ID_SEP_STEP
CommandCount=6

[TB:IDR_CATEGORY_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_CAT_NEW
Command2=ID_CAT_SAVE
Command3=ID_CAT_DELETE
Command4=ID_PLAN_NEW
CommandCount=4

[TB:IDR_PLAN_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_PLAN_NEW
Command2=ID_PLAN_SAVE
Command3=ID_PLAN_DELETE
Command4=ID_PLAN_MOVE
Command5=ID_PLAN_COPY
Command6=ID_PLAN_APPROVE
Command7=ID_STEP_NEW
Command8=ID_STEP_PASTE
CommandCount=8

[TB:IDR_STEP_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_STEP_NEW
Command2=ID_STEP_SAVE
Command3=ID_STEP_DELETE
Command4=ID_STEP_MOVE_UP
Command5=ID_STEP_MOVE_DOWN
Command6=ID_STEP_COPY
Command7=ID_STEP_PASTE
CommandCount=7

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_VIEW_TOOLBAR
Command3=ID_VIEW_STATUS_BAR
Command4=ID_VIEW_ALLPLANS
Command5=ID_VIEW_ACTIVEPLANS
Command6=ID_VIEW_APPROVEDPLANS
Command7=ID_VIEW_UNAPPROVEDPLANS
Command8=ID_CAT_NEW
Command9=ID_CAT_DELETE
Command10=ID_CAT_EDIT
Command11=ID_PLAN_NEW
Command12=ID_PLAN_DELETE
Command13=ID_PLAN_CUT
Command14=ID_PLAN_COPY
Command15=ID_PLAN_PASTE
Command16=ID_PLAN_EDIT
Command17=ID_PLAN_SCHEDULE
Command18=ID_PLAN_CONTROL
Command19=ID_HELP_ABOUTPLANMANAGER
CommandCount=19

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CStepCustomDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_PARAM_SELECT_COMBO,combobox,1075904771
Control6=IDC_PARAM_EDIT,edit,1082196096

[CLS:CPlanExecControlDlg]
Type=0
HeaderFile=PlanExecControlDlg.h
ImplementationFile=PlanExecControlDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CPlanExecControlDlg
VirtualFilter=dWC

[CLS:CYesNoDlg]
Type=0
HeaderFile=YesNoDlg.h
ImplementationFile=YesNoDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CYesNoDlg

[CLS:CDelayDlg]
Type=0
HeaderFile=DelayDlg.h
ImplementationFile=DelayDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CDelayDlg

[CLS:CStepCustomDlg]
Type=0
HeaderFile=StepCustomDlg.h
ImplementationFile=StepCustomDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CStepCustomDlg
VirtualFilter=dWC

[DLG:IDD_STEPPARAM_DIALOG]
Type=1
Class=CStepParameterDlg
ControlCount=7
Control1=IDC_STATIC,static,1342308352
Control2=IDC_PARAMNAME_EDIT,edit,1350567936
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PARAMVALUE_EDIT,edit,1350631492
Control5=IDC_PARAMVALUE_COMBO,combobox,1344339971
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816

[CLS:CStepParameterDlg]
Type=0
HeaderFile=StepParameterDlg.h
ImplementationFile=StepParameterDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CStepParameterDlg
VirtualFilter=dWC

[DLG:IDD_ALLPLANS_FORM]
Type=1
Class=CAllPlansForm
ControlCount=1
Control1=IDC_NOTES_PICT,static,1342177539

[CLS:CAllPlansForm]
Type=0
HeaderFile=AllPlansForm.h
ImplementationFile=AllPlansForm.cpp
BaseClass=CFormView
Filter=D
LastObject=CAllPlansForm
VirtualFilter=VWC

[DLG:IDD_ACTIVEPLANS_FORM]
Type=1
Class=CActivePlansForm
ControlCount=2
Control1=IDC_NOTES_PICT,static,1342177539
Control2=IDC_ALLACTIVEPLANINST_LIST,SysListView32,1350631425

[CLS:CActivePlansForm]
Type=0
HeaderFile=ActivePlansForm.h
ImplementationFile=ActivePlansForm.cpp
BaseClass=CFormView
Filter=D
LastObject=IDC_ALLACTIVEPLANINST_LIST
VirtualFilter=VWC

[DLG:IDD_UNAPPROVEDPLANS_FORM]
Type=1
Class=CUnapprovedPlansForm
ControlCount=1
Control1=IDC_NOTES_PICT,static,1342177539

[CLS:CUnapprovedPlansForm]
Type=0
HeaderFile=UnapprovedPlansForm.h
ImplementationFile=UnapprovedPlansForm.cpp
BaseClass=CFormView
Filter=D
LastObject=CUnapprovedPlansForm
VirtualFilter=VWC

[DLG:IDD_PLAN_DETAILS_DIALOG]
Type=1
Class=CPlanDetailsDlg
ControlCount=13
Control1=IDC_NAME_LABEL,static,1342308352
Control2=IDC_PLAN_NAME_EDIT,edit,1350633600
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PLANEXECMODE_EDIT,edit,1350568064
Control5=IDC_STATIC,static,1342308352
Control6=IDC_PLANAPPSTATUS_EDIT,edit,1350568064
Control7=IDC_DESC_LABEL,static,1342308352
Control8=IDC_PLAN_DESCRIPTION_EDIT,edit,1350633540
Control9=IDC_ACTIVEPLAN_LIST,SysListView32,1350631433
Control10=IDC_PLANDETAILS_BOX,button,1342177287
Control11=IDC_ACTIVEPLANS_BOX,button,1342177287
Control12=IDC_NOTES_PICT,static,1342177539
Control13=IDC_SPLITBAR,static,1342177280

[CLS:CPlanDetailsDlg]
Type=0
HeaderFile=PlanDetailsDlg.h
ImplementationFile=PlanDetailsDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CPlanDetailsDlg
VirtualFilter=dWC

[DLG:IDD_STEPLIST_DIALOG]
Type=1
Class=CStepListDlg
ControlCount=15
Control1=IDC_PLANSTATUS_BOX,button,1342177287
Control2=IDC_PLANINST_LABEL,static,1342308864
Control3=IDC_PLANINST_COMBO,combobox,1344339971
Control4=IDC_PLANOWNER_LABEL,static,1342308864
Control5=IDC_PLANOWNER_EDIT,edit,1350633472
Control6=IDC_PLANEXECSTATE_LABEL,static,1342308864
Control7=IDC_PLANEXECSTATE_EDIT,edit,1350633600
Control8=IDC_PLANEXECMODE_LABEL,static,1342308864
Control9=IDC_PLANEXECMODE_EDIT,edit,1350633600
Control10=IDC_PLANSTEPS_BOX,button,1342177287
Control11=IDC_PLANSTEP_LIST,SysListView32,1350631437
Control12=IDC_SPLITBAR,static,1342177280
Control13=IDC_STEPPARAM_BOX,button,1342177287
Control14=IDC_STEPPARAM_LIST,SysListView32,1350631429
Control15=IDC_NOTES_PICT,static,1342177539

[DLG:IDD_PLANFLOW_DIALOG]
Type=1
Class=CPlanFlowDlg
ControlCount=15
Control1=IDC_FLOW_PICT,static,1342177284
Control2=IDC_NOTES_PICT,static,1342177539
Control3=IDC_FLOW_HOR_SBAR,scrollbar,1073741824
Control4=IDC_FLOW_VER_SBAR,scrollbar,1073741825
Control5=IDC_PLANSTATUS_BOX,button,1342177287
Control6=IDC_PLANINST_LABEL,static,1342308864
Control7=IDC_PLANINST_EDIT,edit,1350633600
Control8=IDC_PLANINST_COMBO,combobox,1344339971
Control9=IDC_PLANOWNER_LABEL,static,1342308864
Control10=IDC_PLANOWNER_EDIT,edit,1350633472
Control11=IDC_PLANEXECSTATE_LABEL,static,1342308864
Control12=IDC_PLANEXECSTATE_EDIT,edit,1350633600
Control13=IDC_PLANEXECMODE_LABEL,static,1342308864
Control14=IDC_PLANEXECMODE_EDIT,edit,1350633600
Control15=IDC_PLANFLOW_BOX,button,1342177287

[CLS:CPlanFlowDlg]
Type=0
HeaderFile=PlanFlowDlg.h
ImplementationFile=PlanFlowDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CPlanFlowDlg

[CLS:CStepListDlg]
Type=0
HeaderFile=StepListDlg.h
ImplementationFile=StepListDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CStepListDlg
VirtualFilter=dWC

[CLS:CNotes]
Type=0
HeaderFile=Notes.h
ImplementationFile=Notes.cpp
BaseClass=CStatic
Filter=W
LastObject=CNotes
VirtualFilter=WC


[CLS:CPlanStepEditDoc]
Type=0
HeaderFile=PlanStepEditDoc.h
ImplementationFile=PlanStepEditDoc.cpp
BaseClass=CDocument
Filter=N

[CLS:CPlanStepEditMainFrame]
Type=0
HeaderFile=PlanStepEditMainFrm.h
ImplementationFile=PlanStepEditMainFrm.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=CPlanStepEditMainFrame

[MNU:IDR_PLANEDIT]
Type=1
Class=?
Command1=ID_PLAN_EXIT
Command2=ID_PLAN_SAVE
Command3=ID_PLAN_APPROVE
Command4=ID_PLAN_UNAPPROVE
Command5=ID_STEP_NEW
Command6=ID_STEP_DELETE
Command7=ID_STEP_CUT
Command8=ID_STEP_COPY
Command9=ID_STEP_PASTE
Command10=ID_STEP_EDIT
CommandCount=10

[CLS:CEditableListCtrl]
Type=0
HeaderFile=EditableListCtrl.h
ImplementationFile=EditableListCtrl.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CEditableListCtrl

[CLS:CListCellCheckBox]
Type=0
HeaderFile=ListCellCheckBox.h
ImplementationFile=ListCellCheckBox.cpp
BaseClass=CButton
Filter=W
VirtualFilter=BWC
LastObject=CListCellCheckBox

[MNU:IDR_STEPEDIT]
Type=1
Class=?
Command1=ID_STEP_EXIT
Command2=ID_STEP_SAVE
Command3=ID_STEP_EDITPARAM
CommandCount=3

[CLS:CPlanCatEditMainFrame]
Type=0
HeaderFile=PlanCatEditMainFrm.h
ImplementationFile=PlanCatEditMainFrm.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=CPlanCatEditMainFrame

[CLS:CPlanCatEditDoc]
Type=0
HeaderFile=PlanCatEditDoc.h
ImplementationFile=PlanCatEditDoc.cpp
BaseClass=CDocument
Filter=N

[CLS:CPlanEditMainFrame]
Type=0
HeaderFile=PlanEditMainFrm.h
ImplementationFile=PlanEditMainFrm.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=ID_PLAN_EXECUTE

[MNU:IDR_CATEDIT]
Type=1
Class=?
Command1=ID_CAT_EXIT
Command2=ID_CAT_SAVE
CommandCount=2

[TB:IDR_PLANCONTROL]
Type=1
Class=?
Command1=ID_PLAN_EXECUTE
Command2=ID_PLAN_STOP
Command3=ID_PLAN_PAUSE
Command4=ID_PLAN_RESUME
Command5=ID_PLAN_TAKECONTROL
Command6=ID_PLAN_RELEASECONTROL
CommandCount=6

[TB:IDR_PLANEDIT]
Type=1
Class=?
Command1=ID_PLAN_SAVE
Command2=ID_PLAN_APPROVE
Command3=ID_PLAN_UNAPPROVE
Command4=ID_STEP_NEW
Command5=ID_STEP_DELETE
Command6=ID_STEP_CUT
Command7=ID_STEP_COPY
Command8=ID_STEP_PASTE
Command9=ID_STEP_EDIT
CommandCount=9

[TB:IDR_STEPEDIT]
Type=1
Class=?
Command1=ID_STEP_SAVE
Command2=ID_STEP_EDITPARAM
Command3=ID_STEP_CUSTOMOK
CommandCount=3

[TB:IDR_CATEDIT]
Type=1
Class=?
Command1=ID_CAT_SAVE
CommandCount=1

[DLG:IDD_APPROVEDPLANS_FORM]
Type=1
Class=CApprovedPlansForm
ControlCount=0

[CLS:CApprovedPlansForm]
Type=0
HeaderFile=ApprovedPlansForm.h
ImplementationFile=ApprovedPlansForm.cpp
BaseClass=CFormView
Filter=D
LastObject=CApprovedPlansForm

[TB:IDR_PLANCTRL]
Type=1
Class=?
Command1=ID_PLAN_EXECUTE
Command2=ID_PLAN_STOP
Command3=ID_PLAN_PAUSE
Command4=ID_PLAN_RESUME
Command5=ID_PLAN_TAKECONTROL
Command6=ID_PLAN_RELEASECONTROL
Command7=ID_PLAN_SCHEDULE
Command8=ID_STEP_CUSTOMISE
CommandCount=8

[MNU:IDR_PLANCTRL]
Type=1
Class=?
Command1=ID_PLAN_EXIT
Command2=ID_PLAN_EXECUTE
Command3=ID_PLAN_EXECUTE_CUSTOM
Command4=ID_PLAN_STOP
Command5=ID_PLAN_PAUSE
Command6=ID_PLAN_RESUME
Command7=ID_PLAN_TAKECONTROL
Command8=ID_PLAN_RELEASECONTROL
Command9=ID_PLAN_SCHEDULE
Command10=ID_STEP_CUSTOMISE
CommandCount=10

