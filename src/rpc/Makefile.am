top_dir = ${top_srcdir}/..
rpc_rootdir = ${top_srcdir}/src/rpc
third_dir = ${top_dir}/third-64
pub_dir = ${top_dir}/pub_util

AM_CPPFLAGS = -I${third_dir}/protobuf/include/ \
			  -I${third_dir}/libev/include/ \
			  -I${third_dir}/boost/include/ \
			  -I${third_dir}/glog/include/ \
			  -I${pub_dir}

AM_LDFLAGS = -lz -lrt

LDADD = ${third_dir}/protobuf/lib/*.a \
		${third_dir}/libev/lib/*.a \
		${third_dir}/boost/lib/*.a \
		${third_dir}/glog/lib/*.a

RPC_SRC = ${rpc_rootdir}/rpc_server.h \
		  ${rpc_rootdir}/rpc_server.cpp \
		  ${rpc_rootdir}/libev_connector.h \
		  ${rpc_rootdir}/libev_connector.cpp \
		  ${rpc_rootdir}/io_thread.h \
		  ${rpc_rootdir}/io_thread.cpp \
		  ${rpc_rootdir}/thread_pool.h \
		  ${rpc_rootdir}/thread_pool.cpp \
		  ${rpc_rootdir}/rpc_channel.h \
		  ${rpc_rootdir}/rpc_channel.cpp \
		  ${rpc_rootdir}/rpc_util.h \
		  ${rpc_rootdir}/rpc_util.cpp \
		  ${rpc_rootdir}/socket_util.h \
		  ${rpc_rootdir}/socket_util.cpp \
		  ${rpc_rootdir}/rpc_msg.pb.h \
		  ${rpc_rootdir}/rpc_msg.pb.cc \
		  ${rpc_rootdir}/logger.h \
		  ${rpc_rootdir}/logger.cpp


noinst_LIBRARIES = librpc.a

librpc_a_SOURCES = ${RPC_SRC}

#SUBDIRS = test
