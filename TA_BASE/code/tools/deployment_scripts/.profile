TA_HOME=/u01/transactive
export TA_HOME

PATH=$PATH:$TA_HOME/bin:.
export PATH

ORACLE_HOME=/u01/app/oracle/OraHome920
export ORACLE_HOME

LD_LIBRARY_PATH=$TA_HOME/lib:$ORACLE_HOME/lib:/usr/local/lib
export LD_LIBRARY_PATH

coreadm -p $HOME/core_files/core.%n.%f $$

if [ -r ~/.bashrc ] ; then
    . ~/.bashrc
fi
