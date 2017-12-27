#!/bin/ksh
##
###############################################################################
#
# Startup and Shutdown script for Oracle 9i database.  Should also shutdown
# and startup the listener. This should be run as user oracle.
# Created: Matt Chalwell
# Date: 25th July 2002
# 
###############################################################################
##
#
ORACLE_BASE=/u01/app/oracle; export ORACLE_BASE
ORACLE_HOME=/u01/app/oracle/OraHome920; export ORACLE_HOME
ORACLE_SID=tra_stat; export ORACLE_SID

# Start the oracle database:
$ORACLE_HOME/bin/sqlplus /nolog << EOF
conn / as sysdba
shutdown immediate
exit
EOF

lsnrctl stop

