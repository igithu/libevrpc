#!/bin/bash

#build the data_server
#must have the bootstrap.sh file or be failed for sure
#must install automake


if [ "$1" == "clean" ]; then
    make distclean && ./bootstrap.sh clean
    exit
fi

BUILD_ROOT=$PWD
INSTALL_PATH="/usr/local/"

PREFIX='--prefix'
for arg in "$@"
do
    #arr=$(echo $arg | tr "=" '=')
    arr=(${arg//=/ })
    ps=(${arr[0]})
    content=(${arr[1]})
    if [ "$ps" = "$PREFIX" ];
    then
        INSTALL_PATH=$content/libevrpc
        echo $INSTALL_PATH
    fi
done

# create new path to install
if [ -d $INSTALL_PATH ]
then
    rm -rf $INSTALL_PATH
fi
mkdir -p $INSTALL_PATH

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
./configure --disable-dependency-tracking --prefix=${INSTALL_PATH} && make
if [ $? -ne 0 ]
then
    echo "make the project failed!"
    exit -1
fi



#test bin:start to collect the project file 

echo ""
echo "*******************************************"
echo "build the libevrpc successfully!!"
echo "*******************************************"
echo ""



