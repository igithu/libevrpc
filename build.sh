#!/bin/sh
#!/bin/bash

#build the data_server
#must have the bootstrap.sh file or be failed for sure
#must install automake

BUILD_ROOT=$PWD

SRC_ROOT=$BUILD_ROOT/src
CLIENT_SRC_PATH=$SRC_ROOT/cs_sample
SERVER_SRC_PATH=$SRC_ROOT/cs_sample

START_FILE=$BUILD_ROOT/libevrpc.sh
SERVER_PATH=$BUILD_ROOT/libevrpc
SERVER_BIN_PATH=$SERVER_PATH/bin
CONF_PATH=$BUILD_ROOT/../../conf
CONF_FILE=$CONF_PATH/ds.ini

echo $BUILD_ROOT

# check the bootstrap exist.
if [ ! -f $BUILD_ROOT/bootstrap.sh ]
then
    echo "the bootstrap.sh does not exist!"
    exit -1
fi

# clean the .o file in order to make the proj
if [ -d $SERVER_PATH ] 
then
    rm -rf $SERVER_PATH
fi
make distclean && ./bootstrap.sh clean
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



#start to collect the project file 

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


# make sure the path SERVER_BIN_PATH exist

if [ ! -d $SERVER_BIN_PATH ]
then
    mkdir -p $SERVER_BIN_PATH
fi 


mv $CLIENT_SRC_PATH/rpc_client $SERVER_SRC_PATH/rpc_server $SERVER_BIN_PATH && \
#cp -r $CONF_PATH $SERVER_PATH && \
#cp $START_FILE $SERVER_BIN_PATH

if [ $? -ne 0 ]
then
    echo "build the clint bin failed"
    exit -1
fi

echo ""
echo "*******************************************"
echo "build the libevrpc successfully!!"
echo "*******************************************"
echo ""

make distclean && ./bootstrap.sh clean


