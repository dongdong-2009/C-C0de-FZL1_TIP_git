#!/bin/sh
rm -rf CM*
rm -rf Make* 
rm -rf cm*
rm -rf sqlgen*
rm -rf bin*
rm -rf ../bin

cmake ..
make
