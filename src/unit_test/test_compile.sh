#!/bin/bash

g++ -o client client_test.cpp test_def.h ../util/rpc_communication.* ../util/rpc_util.*
g++ -o server server_test.cpp test_def.h ../util/rpc_communication.* ../util/rpc_util.*
