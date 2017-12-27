setlocal
start "PlanBannerSimulator" PlanBannerSimulator.exe  --location-key=1 --notify-hosts=Occ-scada-1:6667,Occ-scada-1:6668 --db-connection-file=c:\transActive\config\database\OCC_ScadaConnectionStrings.csv --mgr-port=6666 --corba-timeout=9999 --DataBase_TRANSACT=DataBaseGood --debug-level=INFO --debug-file=C:\log\PlanBannerSimulator.log
endlocal
