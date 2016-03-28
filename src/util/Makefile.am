AUTOMAKE_OPTIONS=foreign subdir-objects

top_dir = ${top_srcdir}/..
util_dir = ${top_srcdir}/src/rpc/util
third_dir = ${top_dir}/third-64

AM_CPPFLAGS = -I${util_dir}/ \
			  -I${third_dir}/gperftools/include/ \
			  -I${third_dir}/protobuf/include/

AM_LDFLAGS = -lz -lrt

#LDADD = ${third_dir}/gperftools/lib/libtcmalloc_minimal.a \
#		${third_dir}/protobuf/lib/*.a

UTIL_SRC = rpc_util.h \
		   rpc_util.cpp \
		   pthread_rwlock.h \
		   disallow_copy_and_assign.h \
		   rpc_communication.h \
		   rpc_communication.cpp


noinst_LIBRARIES = util.a

util_a_SOURCES = ${UTIL_SRC}

#SUBDIRS = test
