AUTOMAKE_OPTIONS=foreign subdir-objects

top_dir = ${top_srcdir}/..
rpc_rootdir = ${top_srcdir}/src/util/
third_dir = ${top_dir}/third-64

AM_CPPFLAGS = -I${third_dir}/gperftools/include/ \
			  -I${third_dir}/protobuf/include/ \
			  -I${third_dir}/libev/include/

#AM_LDFLAGS = -lz -lrt
AM_LDFLAGS = -lz -lstdc++

LDADD = ${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a \
		${third_dir}/libev/lib/*.a

RPC_SRC = ${rpc_rootdir}/rpc_util.h \
		  ${rpc_rootdir}/rpc_util.cpp \
		  ${rpc_rootdir}/thread.h \
		  ${rpc_rootdir}/pthread_mutex.h \
		  ${rpc_rootdir}/pthread_rwlock.h \
		  ${rpc_rootdir}/disallow_copy_and_assign.h \
		  ${rpc_rootdir}/rpc_communication.h \
		  ${rpc_rootdir}/rpc_communication.cpp


noinst_LIBRARIES = libutil.a

libutil_a_SOURCES = ${RPC_SRC}

#SUBDIRS = test
