#!/usr/bin/sh
sed -e "s/\(#include <\).*\.\/\([A-Za-z]\)/\1\2/" $1 > $1.tmp1
sed -e "s/\(#include <\)\($2\/\)/\1..\/\2/" $1.tmp1 > $1.tmp2
rm $1
cp $1.tmp2 $1
rm $1.tmp?
