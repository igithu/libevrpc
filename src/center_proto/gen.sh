
if [ "$1" == "clean" ]; then
    rm -rf *.pb.cc
    rm -rf *.pb.h
    exit
fi

../../../third-64/protobuf/bin/protoc --proto_path=./ --cpp_out=./ ./*.proto
#../../../third-64/protobuf/bin/protoc --proto_path=./ --cpp_out=./ ./rpc_msg.proto && \
#mv rpc_msg.pb.* ../common
