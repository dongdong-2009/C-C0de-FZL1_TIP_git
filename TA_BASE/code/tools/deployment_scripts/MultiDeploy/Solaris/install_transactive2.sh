#!/bin/bash

if [ "$1" = "" ] || [ "$2" = "" ] || [ "$3" = "" ] || [ "$4" = "" ] || [ "$5" = "" ] || [ "$6" = "" ]; then
    echo "usage <version> <Debug|Release> <tag> <v_file> <kill> <slink> <db schema - optional> <db user - optional>"
    echo "eg. install_transactive 00.12.01 Debug CODE_4669_001201 1.0 y y TRA_INT IT154_3"
    echo "/usr/sbin/mount and /usr/sbin/umount must be suid root"
    exit 1
fi

VERSION=$1
MODE=$2
TAG=$3
V_FILE=$4
KILL_FLAG=$5
SYM_LINK=$6
DB_SCHEMA=$7
DB_USER=$8

if [ "$KILL_FLAG" = "Y" -o "$KILL_FLAG" = "y" ]; then
    watson kill
fi

mkdir $VERSION
mkdir tmp_mnt
/usr/sbin/mount 10.205.83.31:/u06/home/tabuild/$MODE/staging/$TAG/Solaris ~/tmp_mnt/

cd $VERSION
mkdir log

gunzip -c ~/tmp_mnt/Solaris.tar.gz | tar xvf -
#cp ~/tmp_mnt/lib/libomnithread.so .
cd bin
cp ~/bin/watson .
cp ~/bin/versions .
cp ~/bin/transactive .
cp ~/bin/transactive.station .
chmod 755 *

(echo -e "root"
sleep 3
echo -e "oldhats"
sleep 5
echo -e "cd /u01/transactive/$VERSION/bin;chown root:other SystemStatusAgent;chown root:other SystemController;chmod +s SystemStatusAgent;chmod +s SystemController"
sleep 4) | telnet localhost

cd

/usr/sbin/umount ~/tmp_mnt
rmdir tmp_mnt

# Allow update of symbolic links to be optional

if [ "$SYM_LINK" = "Y" -o "$SYM_LINK" = "y" ]; then
    rm bin lib log
    ln -s $VERSION/bin
    ln -s $VERSION/lib
    ln -s $VERSION/log
fi

cd ~/bin

# Update versions file (Major and minor version numbers)
temp=${VERSION:0:5}
sed "s/Version:.*/Version:$temp/" versions | sed "s/:.*:/:$V_FILE:/g" > new_v
mv new_v versions

# Find out if server is station or OCC
if [ -e transactive ]; then
    TRAN="transactive"
fi

if [ -e transactive.station ]; then
    TRAN="transactive.station"
fi

# If neither transActive script is in ~/bin, exit
if [ ! -e transactive -a ! -e transactive.station ]; then
    echo "MISSING TRANSACTIVE !!!!!"
    exit
fi

# Given that transActive script exists, change db connection string
if [ "$DB_SCHEMA" != "" -a "$DB_USER" != "" ]; then
    sed "s/db-connection=[^ --]* /db-connection=$DB_SCHEMA:$DB_USER:$DB_USER /" $TRAN > output 

    mv -f output $TRAN
fi

# Check that version file and db conn string is correct (for user)
cat versions
grep -i "nohup" $TRAN

