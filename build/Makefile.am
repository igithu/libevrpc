
#SUBDIRS=src
#DIST_SUBDIRS=src

ROOT_DIR=${top_srcdir}/../src

INCLUDES=-I${ROOT_DIR}/

COMM_LIB =../src/cs_sample/proto/simple_rpc.pb.h \
		  ../src/cs_sample/proto/simple_rpc.pb.cc \
		  ../src/rpc/socket_util.h \
		  ../src/rpc/socket_util.cpp \
		  ../src/rpc/rpc_msg.pb.h \
		  ../src/rpc/rpc_msg.pb.cc \
		  ../src/rpc/rpc_util.h \
		  ../src/rpc/rpc_util.cpp \
		  ../src/log/libevrpc_log.h \
		  ../src/log/libevrpc_log.cpp

COMMON_CF = -I../../third-64/protobuf/include/ \
			-I../../third-64/glog/include/ \
			-I../../third-64/libev/include/

COMMON_LD = -lpthread -lz -ldl -lrt \
			../../third-64/protobuf/lib/*.a \
			../../third-64/glog/lib/*.a \
		    ../../third-64/libev/lib/*.a	



bin_PROGRAMS = rpc_client rpc_server

rpc_client_SOURCES = \
		      ../src/cs_sample/libev_client.cc \
			  ../src/rpc/rpc_channel.h \
			  ../src/rpc/rpc_channel.cpp \
			  $(COMM_LIB)

rpc_client_CPPFLAGS = $(COMMON_CF)
rpc_client_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_client_LDADD =  $(COMMON_LD)

rpc_server_SOURCES = \
		      ../src/cs_sample/libev_server.cc \
			  ../src/rpc/libev_connector.h \
			  ../src/rpc/libev_connector.cpp \
			  ../src/rpc/rpc_server.h \
			  ../src/rpc/rpc_server.cpp \
			  ../src/rpc/io_thread.h \
			  ../src/rpc/io_thread.cpp \
			  ../src/rpc/thread_pool.h \
			  ../src/rpc/thread_pool.cpp \
			  $(COMM_LIB)

rpc_server_CPPFLAGS = -I../../third-64/boost/include/ \
					   -I../../pub_util/thread/include/ \
					   -I../../pub_util/tools/include/ \
					   $(COMMON_CF)
rpc_server_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_server_LDADD = $(COMMON_LD) \
					../../third-64/boost/lib/*.a
#
