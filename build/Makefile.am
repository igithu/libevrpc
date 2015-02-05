
#SUBDIRS=src
#DIST_SUBDIRS=src

COMM_LIB =../cs_sample/proto/simple_rpc.pb.h \
		  ../cs_sample/proto/simple_rpc.pb.cc \
		  ../rpc/rpc_msg.pb.h \
		  ../rpc/rpc_msg.pb.cc \
		  ../rpc/rpc_util.h \
		  ../rpc/rpc_util.cpp

COMMON_CF = -I../../third-64/protobuf/include/

COMMON_LD = -lpthread -lz -ldl -lrt \
			../../../third-64/protobuf/lib/*.a



bin_PROGRAMS = rpc_client rpc_service

rpc_client_SOURCES = \
		      ../cs_sample/libev_client.cc \
			  ../rpc/connection_manager.h \
			  ../rpc/connection_manager.cpp \
			  ../rpc/rpc_channel.h \
			  ../rpc/rpc_channel.cpp \
			  $(COMM_LIB)

rpc_client_CPPFLAGS = $(COMMON_CF)
rpc_client_LDFLAGS =-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -fpermissive 
rpc_client_LDADD =  $(COMMON_LD)

rpc_service_SOURCES = \
		      ../cs_sample/libev_server.cc \
			  ../rpc/connection_manager.h \
			  ../rpc/connection_manager.cpp \
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
