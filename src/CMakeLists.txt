cmake_minimum_required(VERSION 3.15)
project(EmbeddedRTSPStack)

set(CMAKE_CXX_STANDARD 14)

add_library(eRTSPStack
rtsp_parser.c
rtsp_writer.c 
rtsp_debug.c 
rtsp_internal.c)

target_include_directories(eRTSPStack PRIVATE ${CMAKE_CURRENT_LIST_DIR}/internal_include)
target_include_directories(eRTSPStack PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

