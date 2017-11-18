AUTOMAKE_OPTIONS=foreign subdir-objects

top_dir = ${top_srcdir}/../..
cs_rootdir = ${top_srcdir}
third_dir = ${top_dir}/third-64

#SUBDIRS = proto

INCLUDES = -I${third_dir}/protobuf/include/              \
           -I${third_dir}/libevrpc/include/              \
	   -I${third_dir}/libev/include/                 \
	   -I${cs_rootdir}/


#AM_LDFLAGS = -rdynamic -lpthread -lz -lm -lrt
rpc_client_LDFLAGS = -lpthread -lz -lm

#$(top_builddir)/src/rpc_server/librpc_server.a


LDADD = ${third_dir}/libevrpc/lib/*.a \
		${third_dir}/protobuf/lib/*.a \
		${third_dir}/libev/lib/*.a

bin_PROGRAMS = rpc_server rpc_client

SERVER_SRC = ${cs_rootdir}/src/libev_server.cc    \
			 ${cs_rootdir}/proto/rpc_sample.pb.h  \
			 ${cs_rootdir}/proto/rpc_sample.pb.cc
rpc_server_SOURCES = ${SERVER_SRC}

CLIENT_SRC = ${cs_rootdir}/src/libev_client.cc    \
			 ${cs_rootdir}/proto/rpc_sample.pb.h  \
			 ${cs_rootdir}/proto/rpc_sample.pb.cc
rpc_client_SOURCES = ${CLIENT_SRC}

# rpc_client_LDADD = $(top_builddir)/src/rpc_client/librpc_client.a \
# 		$(top_builddir)/src/cs_sample/proto/libproto.a \
# 		${third_dir}/protobuf/lib/*.a \
# 		${third_dir}/libev/lib/*.a
#
# bin_PROGRAMS = rpc_client
#
# CLIENT_SRC = ${cs_rootdir}/libev_client.cc
# rpc_client_SOURCES = ${CLIENT_SRC}
