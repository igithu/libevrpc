#!/bin/sh
#!/bin/bash

#build the data_server
#must have the bootstrap.sh file or be failed for sure
#must install automake

BUILD_ROOT=$PWD

SRC_ROOT=$BUILD_ROOT/src

SERVER_PATH=$BUILD_ROOT/libevrpc

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

echo ""
echo "*******************************************"
echo "build the ut successfully!!"
echo "*******************************************"
echo ""

#make distclean && ./bootstrap.sh clean


