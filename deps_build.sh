#!/bin/sh

DEPS_PRFIX=https://github.com/igithu
BUILD_ROOT=$PWD/..
INSTALL_ROOT_DIR=$BUILD_ROOT/third-64

function check_directory() {
    if [ ! -f $BUILD_ROOT/$1/mark_done ]
    then
        if [ -d $BUILD_ROOT/$1 ]
        then
            rm -rf $BUILD_ROOT/$1
        fi
        mkdir -p $BUILD_ROOT/$1
        return 0
    fi
    return 1
}

function get_sources_deps() {
    if [ ! -f $BUILD_ROOT/$1/mark_done ]
    then
        if [ -d $BUILD_ROOT/$1 ]
        then
            rm -rf $BUILD_ROOT/$1
        fi
        git clone $DEPS_PRFIX/$1.git
        if [ $? -eq 0 ]
        then
            touch $1/mark_done
        fi
    fi

    if [ ! -f $BUILD_ROOT/$1/mark_done ]
    then
        echo "Get $1 file failed!"
        exit -1
    else
        echo "Get $1 files done"
    fi
}

# $1:with version
function install_deps() {
    if [ -f $INSTALL_ROOT_DIR/$2/mark_done ]
    then
        exit 0
    fi

    if [ -d $BUILD_ROOT/deps/$1 ]
    then
        rm -rf $BUILD_ROOT/deps/$1
    fi

    cd $BUILD_ROOT/deps && tar -zxvf $1.tar.gz
    if [ $? -ne 0 ]
    then
        echo "Get $1 dir failed!"
        exit -1
    fi
    cd $1
    mkdir -p $INSTALL_ROOT_DIR/$2
    echo "the install dir is $INSTALL_ROOT_DIR/$2"
    ./configure --prefix=$INSTALL_ROOT_DIR/$2 && make && make install
    if [ $? -ne 0 ]
    then
        echo "Build the $1 failed!"
        exit -1
    fi
    touch $INSTALL_ROOT_DIR/$2/mark_done
    touch $BUILD_ROOT/deps/mark_done
    echo $INSTALL_ROOT_DIR/$2/mark_done
    echo $BUILD_ROOT/deps/$1
    exit 0
}


cd $BUILD_ROOT
get_sources_deps  deps

check_directory third-64

install_deps libev-4.19 libev && install_deps protobuf-2.6.0 protobuf


#cd $BUILD_ROOT/deps
#
#tar -zxvf libev-4.19.tar.gz
#cd libev-4.19
#make && make check &

#if [ ! -f $BUILD_ROOT/deps/mark_done ]
#then
#    if [ -d $BUILD_ROOT/deps ]
#    then
#        rm -rf $BUILD_ROOT/deps
#    fi
#    git clone $DEPS_PRFIX/deps.git
#    if [ $? -e 0 ]
#    then
#        touch deps/mark_done
#    fi
#fi
#
#if [ ! -f $BUILD_ROOT/deps/mark_done ]
#    echo "Get deps file failed!"
#    exit -1
#fi







