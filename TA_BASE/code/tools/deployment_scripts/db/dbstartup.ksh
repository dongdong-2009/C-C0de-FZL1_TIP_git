#!/bin/ksh
# Startup file for Oracle instance and also for Oracle listener
# Created Matt Chalwell Wed 20th 2001
# Edits:
#  RobA 08/04/2002 - Added check for existing tcms database on secondary
#  M.C. 08/10/2002 - Modified to be Manila specific (transact)

ORACLE_HOME=/u01/app/oracle/OraHome920
ORA_OWNER=oracle
ORACLE_SID=tra_stat

# SITE SPECIFIC  - Change me!
SECONDARY_HOST=192.168.1.2

 
    if [ ! -f $ORACLE_HOME/bin/lsnrctl ]
    then
	echo "Oracle Listener startup: Cannot start listener"
        exit
    fi

    case "$1" in
        'start')

                 # Start the oracle databases:
		 su - oracle -c ~transactive/bin/db/dbora.ksh
           ;;

        'stop')

             	# Stop the oracle databases:
             	su - oracle -c ~transactive/bin/db/dbshut.ksh
             ;;

        *)
            echo "usage: dbstartup.ksh {start|stop}"
    esac

