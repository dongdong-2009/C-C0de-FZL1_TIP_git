kill -f ControlStation.
kill -f Gwx32.exe
kill -f drwtsn32.exe
kill -f AlarmManager.ex
kill -f ArchiveManager.
kill -f banner.exe
kill -f ConfigurationEd
kill -f DisplayManager.
kill -f DutyManager.exe
kill -f ECSManager.exe
kill -f EquipmentStatus
kill -f EventViewer.exe
kill -f InspectorPanel.
kill -f PAManager.exe
kill -f PlanManager.exe
kill -f RadioManager.ex
kill -f ReportManager.e
kill -f SoundManager.ex
kill -f STISManager.exe
kill -f SystemManager.e
kill -f TisLogViewer.ex
kill -f TrendViewer.exe
kill -f TTISManager.exe
kill -f SoundM~1.exe
net stop alarmstore
net stop "OPC Bridge"
cd c:\transactive\

del /F /S /Q bin lib config help sounds integration_tools

rmdir /S /Q bin lib config help sounds integration_tools