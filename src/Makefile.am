#AUTOMAKE_OPTIONS=foreign subdir-objects
#SUBDIRS=rpc_client rpc_server cs_sample

top_dir = ${top_srcdir}/..
src_dir = ${top_srcdir}/src
config_dir = ${src_dir}/config_parser
client_dir = ${src_dir}/rpc_client
server_dir = ${src_dir}/rpc_server


CLIENT_INCLUDES =  \
		rpc_client/rpc_client.h \
		rpc_client/rpc_channel.h \
		rpc_client/client_rpc_controller.h \
		rpc_client/rpc_heartbeat_client.h \
		rpc_client/config_parser.h \
		rpc_client/dictionary.h \
		rpc_client/iniparser.h \
		rpc_client/rpc_util.h \
		rpc_client/disallow_copy_and_assign.h \
		rpc_client/rpc_communication.h \
		rpc_client/pthread_rwlock.h
CLIENT_SORCS = \
SERVER_INCLUDES =  \
SERVER_SRCS =  \


nobase_include_HEADERS =  \
		${CLIENT_INCLUDES} \
		${SERVER_INCLUDES}




lib_LTLIBRARIES = librpc_client.a

rpc_client_CPPFLAGS = \
		-I${top_srcdir}/src \
		-I${third_dir}/gperftools/include/ \
		-I${third_dir}/protobuf/include/
rpc_client_LIBADD = \
		${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a
rpc_client_a_LDFLAGS =
rpc_client_la_SOURCES = \
		${CLIENT_SORCS}




rpc_server_CPPFLAGS = \
		-I${top_srcdir}/src \
		-I${third_dir}/gperftools/include/ \
		-I${third_dir}/protobuf/include/ \
		-I${third_dir}/libev/include/
rpc_server_LIBADD = \
		${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
		${third_dir}/protobuf/lib/*.a \
		${third_dir}/libev/lib/*.a
rpc_server_a_LDFLAGS =
rpc_server_la_SOURCES =


