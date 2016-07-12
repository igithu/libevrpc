#!/bin/bash

BUILD_ROOT=$PWD
CLIENT_SRC_PATH=$BUILD_ROOT
SERVER_SRC_PATH=$BUILD_ROOT

START_FILE=$BUILD_ROOT/libevrpc.sh
SAMPLE_PATH=$BUILD_ROOT/rpc_sample
CONF_PATH=$BUILD_ROOT
CONF_FILE=$CONF_PATH/*.ini

# check the bootstrap exist.
if [ ! -f $BUILD_ROOT/bootstrap.sh ]
then
    echo "the bootstrap.sh does not exist!"
    exit -1
fi

make distclean
./bootstrap.sh clean
./bootstrap.sh

# check the configure file
if [ ! -f $BUILD_ROOT/configure ]
then
    echo "the configure does not exist!"
    exit -1
fi

# make the project
./configure --disable-dependency-tracking && make
if [ $? -ne 0 ]
then
    echo "make the project failed!"
    exit -1
fi

## make sure the config file exist
#if [ ! -f $CONF_FILE ]
#then
#    echo "the config file $CONF_FILE does not exist"
#    exit -1
#fi

if [ ! -f $START_FILE ]
then
    echo "the $START_FILE file does not exist"
    exit -1
fi


if [ -d $SAMPLE_PATH ]
then
    rm -rf $SAMPLE_PATH
fi
mkdir -p $SAMPLE_PATH


mv $CLIENT_SRC_PATH/rpc_client $SERVER_SRC_PATH/rpc_server $SAMPLE_PATH && \
cp $START_FILE $SAMPLE_PATH
cp -r $CONF_FILE $SAMPLE_PATH && \

if [ $? -ne 0 ]
then
    echo "build the clint bin failed"
    exit -1
fi


make distclean
./bootstrap.sh clean
