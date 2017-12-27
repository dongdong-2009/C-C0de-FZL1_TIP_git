#!/bin/sh
rm -rf CM*
rm -rf Make* 
rm -rf cm*
rm -rf sqlgen*
rm -rf bin*
cd ..
rm -f ./bin/sqlgen


make  clean
make DEBUG=1
