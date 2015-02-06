
#SUBDIRS=src
#DIST_SUBDIRS=src

COMM_LIB =../cs_sample/proto/simple_rpc.pb.h \
		  ../cs_sample/proto/simple_rpc.pb.cc \
		  ../rpc/socket_util.h \
		  ../rpc/socket_util.cpp \
		  ../rpc/rpc_msg.pb.h \
		  ../rpc/rpc_msg.pb.cc \
		  ../rpc/rpc_util.h \
		  ../rpc/rpc_util.cpp \
		  ../log/libevrpc_log.h \
		  ../log/libevrpc_log.cpp

COMMON_CF = -I../../third-64/protobuf/include/ \
			-I../../third-64/glog/include/ \
			-I../../third-64/libev/include/

COMMON_LD = -lpthread -lz -ldl -lrt \
			../../third-64/protobuf/lib/*.a \
			../../third-64/glog/lib/*.a \
		    ../../third-64/libev/lib/*.a	



bin_PROGRAMS = rpc_client rpc_service

rpc_client_SOURCES = \
		      ../cs_sample/libev_client.cc \
			  ../rpc/rpc_channel.h \
			  ../rpc/rpc_channel.cpp \
			  $(COMM_LIB)

rpc_client_CPPFLAGS = $(COMMON_CF)
rpc_client_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_client_LDADD =  $(COMMON_LD)

rpc_service_SOURCES = \
		      ../cs_sample/libev_server.cc \
			  ../rpc/libev_connector.h \
			  ../rpc/libev_connector.cpp \
			  ../rpc/rpc_server.h \
			  ../rpc/rpc_server.cpp \
			  ../rpc/io_thread.h \
			  ../rpc/io_thread.cpp \
			  ../rpc/thread_pool.h \
			  ../rpc/thread_pool.cpp \
			  $(COMM_LIB)

rpc_service_CPPFLAGS = -I../../third-64/boost/include/ \
					   -I../../util/thread/include/ \
					   -I../../util/tools/include/ \
					   $(COMMON_CF)
rpc_service_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_service_LDADD = $(COMMON_LD) \
					../../third-64/boost/lib/*.a
#
