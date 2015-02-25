
#SUBDIRS=src
#DIST_SUBDIRS=src

root_dir=${top_srcdir}/../src
third_dir=../../third-64
pub_dir=../../pub_util

INCLUDES=-I${root_dir}/ -I${pub_dir}

COMMON_SRC=${root_dir}/cs_sample/proto/simple_rpc.pb.h \
		   ${root_dir}/cs_sample/proto/simple_rpc.pb.cc \
		   ${root_dir}/rpc/socket_util.h \
		   ${root_dir}/rpc/socket_util.cpp \
		   ${root_dir}/rpc/rpc_msg.pb.h \
		   ${root_dir}/rpc/rpc_msg.pb.cc \
		   ${root_dir}/rpc/rpc_util.h \
		   ${root_dir}/rpc/rpc_util.cpp \
		   ${root_dir}/log/libevrpc_log.h \
		   ${root_dir}/log/libevrpc_log.cpp

COMMON_CF = -I${third_dir}/protobuf/include/ \
			-I${third_dir}/glog/include/ \
			-I${third_dir}/libev/include/

COMMON_LD = -lpthread -lz -ldl -lrt \
			${third_dir}/protobuf/lib/*.a \
			${third_dir}/glog/lib/*.a \
		    ${third_dir}/libev/lib/*.a	



bin_PROGRAMS = rpc_client rpc_server

rpc_client_SOURCES = \
		      ${root_dir}/cs_sample/libev_client.cc \
			  ${root_dir}/rpc/rpc_channel.h \
			  ${root_dir}/rpc/rpc_channel.cpp \
			  $(COMMON_SRC)

rpc_client_CPPFLAGS = $(COMMON_CF)
rpc_client_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_client_LDADD =  $(COMMON_LD)

rpc_server_SOURCES = \
		      ${root_dir}/cs_sample/libev_server.cc \
			  ${root_dir}/rpc/libev_connector.h \
			  ${root_dir}/rpc/libev_connector.cpp \
			  ${root_dir}/rpc/rpc_server.h \
			  ${root_dir}/rpc/rpc_server.cpp \
			  ${root_dir}/rpc/io_thread.h \
			  ${root_dir}/rpc/io_thread.cpp \
			  ${root_dir}/rpc/thread_pool.h \
			  ${root_dir}/rpc/thread_pool.cpp \
			  $(COMMON_SRC)

rpc_server_CPPFLAGS = -I${third_dir}/boost/include/ \
					   $(COMMON_CF)
rpc_server_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_server_LDADD = $(COMMON_LD) \
					${third_dir}/boost/lib/*.a
#
