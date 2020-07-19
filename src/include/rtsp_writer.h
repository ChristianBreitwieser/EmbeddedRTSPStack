//
// Created by christianb on 19.07.20.
//

#ifndef RTSP_PARSER_RTSP_WRITER_H
#define RTSP_PARSER_RTSP_WRITER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "rtsp_types.h"

typedef void *rtsp_writer_handle;
extern const rtsp_writer_handle rtsp_writer_invalid_handle;

typedef void (*cbRTSP_WRITER_DATA)(void * arg, rtsp_writer_handle h, uint8_t const * const data, uint16_t size, bool eof);

bool rtsp_writer_create(rtsp_writer_handle * handle, uint8_t * buffer, uint32_t * size);

bool rtsp_writer_begin_write_response(rtsp_writer_handle  handle, sRTSP_RESPONSE const * rsp);

bool rtsp_writer_begin_write_request(rtsp_writer_handle  handle, sRTSP_REQUEST const * req);

bool rtsp_writer_write_body_data(rtsp_writer_handle handle, uint8_t const * const data, uint16_t size);

bool rtsp_writer_write_eof(rtsp_writer_handle handle);

bool rtsp_writer_register_arg(rtsp_writer_handle handle, void * arg);

bool rtsp_writer_register_data_cb(rtsp_writer_handle handle, cbRTSP_WRITER_DATA cb);

void rtsp_writer_consume(uint16_t length);

void rtsp_writer_poll();


bool rtsp_writer_reset(rtsp_writer_handle handle);

#ifdef __cplusplus
}
#endif
#endif //RTSP_PARSER_RTSP_WRITER_H
