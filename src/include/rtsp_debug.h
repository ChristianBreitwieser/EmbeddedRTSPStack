//
// Created by christianb on 19.07.20.
//

#ifndef RTSP_PARSER_RTSP_DEBUG_H
#define RTSP_PARSER_RTSP_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtsp_types.h"

    char const * rtsp_debug_method_to_string(eRTSP_METHOD m);
    char const * rtsp_debug_statuscode_to_string(eRTSP_STATUSCODE m);
    char const * rtsp_debug_header_to_string(eRTSP_HEADER h);

#ifdef __cplusplus
}
#endif

#endif //RTSP_PARSER_RTSP_DEBUG_H
