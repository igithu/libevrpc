AUTOMAKE_OPTIONS=foreign subdir-objects

top_dir = ${top_srcdir}/..
util_dir = ${top_srcdir}/src/util
client_rootdir = ${top_srcdir}/src/rpc_client
third_dir = ${top_dir}/third-64

AM_CPPFLAGS = -I${third_dir}/gperftools/include/ \
			  -I${third_dir}/protobuf/include/

#AM_LDFLAGS = -lz -lrt
AM_LDFLAGS = -lz -lstdc++

LDADD = ${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a

RPC_SRC = ${client_rootdir}/rpc_client.h \
		  ${client_rootdir}/rpc_client.cpp \
		  ${client_rootdir}/rpc_channel.h \
		  ${client_rootdir}/rpc_channel.cpp \
		  ${util_dir}/rpc_util.h \
		  ${util_dir}/rpc_util.cpp \
		  ${util_dir}/thread.h \
		  ${util_dir}/pthread_mutex.h \
		  ${util_dir}/pthread_rwlock.h \
		  ${util_dir}/disallow_copy_and_assign.h \
		  ${util_dir}/rpc_communication.h \
		  ${util_dir}/rpc_communication.cpp


noinst_LIBRARIES = librpc_client.a

librpc_client_a_SOURCES = ${RPC_SRC}

#SUBDIRS = test
