AUTOMAKE_OPTIONS=foreign subdir-objects
#SUBDIRS=rpc_client rpc_server cs_sample

top_dir = ${top_srcdir}/..
src_dir = ${top_srcdir}/src
config_dir = ${src_dir}/config_parser
client_dir = ${src_dir}/rpc_client
server_dir = ${src_dir}/rpc_server
third_dir = ${top_dir}/third-64


CLIENT_INCLUDES =                               \
		rpc_client/rpc_client.h                 \
		rpc_client/rpc_channel.h                \
		rpc_client/client_rpc_controller.h      \
		rpc_client/rpc_heartbeat_client.h

CLIENT_SORCS =                                  \
		rpc_client/rpc_client.cpp               \
		rpc_client/rpc_channel.cpp              \
		rpc_client/client_rpc_controller.cpp    \
		rpc_client/rpc_heartbeat_client.cpp

SERVER_INCLUDES =                               \
		rpc_server/rpc_server.h                 \
		rpc_server/dispatch_thread.h            \
		rpc_server/libev_thread_pool.h          \
		rpc_server/server_rpc_controller.h      \
		rpc_server/connection_timer_manager.h   \
		rpc_server/rpc_heartbeat_server.h

SERVER_SRCS =                                   \
		rpc_server/rpc_server.cpp               \
		rpc_server/dispatch_thread.cpp          \
		rpc_server/libev_thread_pool.cpp        \
		rpc_server/server_rpc_controller.cpp    \
		rpc_server/connection_timer_manager.cpp \
		rpc_server/rpc_heartbeat_server.cpp


CENTER_INCLUDES =                           \
		rpc_center/rpc_center.h             \
		rpc_center/center_server_thread.h   \
		rpc_center/election_thread.h        \
		rpc_center/load_balancer.h          \
		rpc_center/reporter_thread.h        \
		center_proto/centers.pb.h           \
		center_proto/center_client.pb.h     \
		center_proto/center_cluster.pb.h    \
		${COMMON_INCLUDES}


CENTER_SRCS =                               \
		rpc_center/rpc_center.cpp           \
		rpc_center/center_server_thread.cpp \
		rpc_center/election_thread.cpp      \
		rpc_center/load_balancer.cpp        \
		rpc_center/reporter_thread.cpp      \
		center_proto/centers.pb.cc          \
		center_proto/center_client.pb.cc    \
		center_proto/center_cluster.pb.cc   \
		${COMMON_SRCS}


COMMON_INCLUDES =                        \
		config_parser/config_parser.h    \
		config_parser/dictionary.h       \
		config_parser/iniparser.h        \
		util/rpc_util.h                  \
		util/thread.h                    \
		util/disallow_copy_and_assign.h  \
		util/rpc_communication.h         \
		util/pthread_mutex.h             \
		util/pthread_rwlock.h

COMMON_SRCS =                            \
		config_parser/config_parser.cpp  \
		config_parser/dictionary.cpp     \
		config_parser/iniparser.cpp      \
		util/rpc_util.cpp                \
		util/rpc_communication.cpp


nobase_include_HEADERS =                 \
		${CLIENT_INCLUDES}               \
		${SERVER_INCLUDES}               \
		${COMMON_INCLUDES}


lib_LTLIBRARIES = librpc_client.la librpc_server.la

librpc_client_la_CPPFLAGS =                                 \
		-I${top_srcdir}/src                                 \
		-I${third_dir}/gperftools/include/                  \
		-I${third_dir}/protobuf/include/
librpc_client_la_LIBADD =                                   \
		#${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a
librpc_client_la_LDFLAGS = -lz -std=c++11
librpc_client_la_SOURCES =       \
		${CLIENT_SORCS}          \
		${COMMON_SRCS}


librpc_server_la_CPPFLAGS =                \
		-I${top_srcdir}/src                \
		-I${third_dir}/gperftools/include/ \
		-I${third_dir}/protobuf/include/   \
		-I${third_dir}/libev/include/
librpc_server_la_LIBADD =                                    \
		#${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a                        \
		${third_dir}/libev/lib/*.a
librpc_server_la_LDFLAGS = -lz -std=c++11
librpc_server_la_SOURCES =   \
		${SERVER_SRCS}       \
		${COMMON_SRCS}


#bin_PROGRAMS = rpc_center_server
#rpc_center_server_SOURCES = rpc_center/rpc_center_server.cc ${CENTER_INCLUDES} ${CENTER_SRCS}
#rpc_center_server_LDADD = ${third_dir}/protobuf/lib/*.a  ${third_dir}/libev/lib/*.a
#INCLUDES = -I${top_srcdir}/src                \
#		   -I${third_dir}/gperftools/include/ \
#		   -I${third_dir}/protobuf/include/   \
#		   -I${third_dir}/libev/include/
#
