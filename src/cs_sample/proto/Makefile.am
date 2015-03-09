top_dir = ${top_srcdir}/..
pb_rootdir = ${top_srcdir}/src/cs_sample/proto
third_dir = ${top_dir}/third-64

AM_CPPFLAGS = -I${third_dir}/protobuf/include/

AM_LDFLAGS = -lz -lrt

LDADD = ${third_dir}/protobuf/lib/*.a

include_HEADERS = ${pb_rootdir}/rpc_sample.pb.h

PB_SRC = ${include_HEADERS} ${pb_rootdir}/rpc_sample.pb.cc

noinst_LIBRARIES = libproto.a

libproto_a_SOURCES = ${PB_SRC}

