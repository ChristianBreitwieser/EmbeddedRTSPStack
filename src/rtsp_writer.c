#include <rtsp_writer.h>
#include <rtsp_internal.h>


const rtsp_writer_handle rtsp_writer_invalid_handle = NULL;

typedef struct {


}sRTSPWriterState;

bool rtsp_writer_create(rtsp_writer_handle * handle, uint8_t * buffer, uint32_t * size){

}

bool rtsp_writer_begin_write_response(rtsp_writer_handle  handle, sRTSP_RESPONSE const * rsp){

}

bool rtsp_writer_begin_write_request(rtsp_writer_handle  handle, sRTSP_REQUEST const * req){

}

bool rtsp_writer_write_body_data(rtsp_writer_handle handle, uint8_t const * const data, uint16_t size){

}

bool rtsp_writer_write_eof(rtsp_writer_handle handle){

}

bool rtsp_writer_register_arg(rtsp_writer_handle handle, void * arg){

}

bool rtsp_writer_register_data_cb(rtsp_writer_handle handle, cbRTSP_WRITER_DATA cb){

}

void rtsp_writer_consume(uint16_t length){

}

void rtsp_writer_poll(){

}
