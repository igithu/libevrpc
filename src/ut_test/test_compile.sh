#!/bin/bash

g++ -o client client_test.cpp test_def.h ../rpc_communication.h ../rpc_communication.cpp -lglog
g++ -o server server_test.cpp test_def.h ../rpc_communication.h ../rpc_communication.cpp -lglog
