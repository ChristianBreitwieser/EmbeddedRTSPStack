#ifndef RTSP_PARSER_H_
#define RTSP_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rtsp_types.h"

typedef void * rtsp_parser_handle;
extern const rtsp_parser_handle rtsp_parser_invalid_handle;

typedef void (*cbRTSP_RESPONSE_HEADER)(void* arg, rtsp_parser_handle h, sRTSP_RESPONSE resp);
typedef void (*cbRTSP_REQUEST_HEADER)(void * arg, rtsp_parser_handle h, sRTSP_REQUEST resp);
typedef void (*cbRTSP_BODY)(void * arg, rtsp_parser_handle h, uint8_t const * const data, uint32_t size);
/**
 * Retrieves the minimal buffer size required for a rtsp parsers internal data
 * @return minimal size of a buffer usable for create_rtsp_parser()
 */
uint32_t get_rtsp_parser_buffer_size(void);

/**
 * Creates a new rtsp_parser
 * @param[out] handle  if the return value is true this will be a valid handle, otherwise this will be invalid.
 * @param[in] buffer a byte buffer which will hold the internal data for the rtsp parser
 * @param[inout] size in: the size of buffer in bytes, out: regardless if the return value is true or false
 *                    this will be the actual size the rtsp_parser's data needs.
 *                    (if false is returned this value is greater than the input value)
 * @return true if the parser was created, false if the buffer is to small
 */
bool create_rtsp_parser(rtsp_parser_handle * handle, uint8_t * buffer, uint32_t * size);

bool rtsp_parser_register_arg(rtsp_parser_handle handle, void * arg);
bool register_rtsp_response_header_callback(rtsp_parser_handle handle,cbRTSP_RESPONSE_HEADER cb);
bool register_rtsp_response_body_callback(rtsp_parser_handle handle, cbRTSP_BODY cb);
bool register_rtsp_request_header_callback(rtsp_parser_handle handle, cbRTSP_REQUEST_HEADER cb);
bool register_rtsp_request_body_callback(rtsp_parser_handle handle, cbRTSP_BODY cb);

bool reset_rtsp_parser(rtsp_parser_handle handle);

bool parse_rtsp_data(rtsp_parser_handle handle, uint8_t const * const data, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif //RTSP_PARSER_H_