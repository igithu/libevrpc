AUTOMAKE_OPTIONS=foreign subdir-objects

top_dir = ${top_srcdir}/..
cs_rootdir = ${top_srcdir}/src/cs_sample
third_dir = ${top_dir}/third-64
pub_dir = ${top_dir}/pub_util

SUBDIRS = proto

AM_CPPFLAGS = -I${top_srcdir}/src \
			  -I${third_dir}/protobuf/include/ \
			  -I${third_dir}/glog/include/ \
			  -I${third_dir}/boost/include/ \
			  -I${third_dir}/libev/include/ \
			  -I${pub_dir}


#AM_LDFLAGS = -rdynamic -lpthread -lz -lm -lrt
AM_LDFLAGS = -lpthread -lz -lm


LDADD = $(top_builddir)/src/rpc/librpc.a \
		$(top_builddir)/src/cs_sample/proto/libproto.a \
		${third_dir}/glog/lib/*.a \
		${third_dir}/protobuf/lib/*.a \
		${third_dir}/boost/lib/*.a \
		${third_dir}/libev/lib/*.a

bin_PROGRAMS = rpc_server rpc_client

SERVER_SRC = ${cs_rootdir}/libev_server.cc
rpc_server_SOURCES = ${SERVER_SRC}

CLIENT_SRC = ${cs_rootdir}/libev_client.cc
rpc_client_SOURCES = ${CLIENT_SRC}
