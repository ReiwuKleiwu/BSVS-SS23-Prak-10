cmake_minimum_required(VERSION 3.7.2)
project(BSVS-SS23-Prak)

set(CMAKE_C_STANDARD 99)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(BSVS-SS23-Prak main.c hashtable.c hashtable.h validate_user_input.h validate_user_input.c handle_requests.c handle_requests.h handle_methods.c handle_methods.h socket_server.c socket_server.h sub_store.c sub_store.h request.h request.c sub_message_queue.c sub_message_queue.h sub_message_queue.h sub_message_queue.c)