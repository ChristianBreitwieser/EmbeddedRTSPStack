cmake_minimum_required(VERSION 3.15)
project(rtsp_server)

set(CMAKE_CXX_STANDARD 14)



add_subdirectory(../../src eRTSPStack)
add_subdirectory(../../tools/LwipApiMock/src lwip_api_fmock)

add_executable(rtsp_server main.cpp rtsp_server.c)
target_link_libraries(rtsp_server PUBLIC lwip_api_fmock eRTSPStack)
