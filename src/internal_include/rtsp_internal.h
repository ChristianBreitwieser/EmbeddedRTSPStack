//
// Created by christianb on 19.07.20.
//

#ifndef RTSP_PARSER_RTSP_INTERNAL_H
#define RTSP_PARSER_RTSP_INTERNAL_H

#include "rtsp_types.h"

typedef enum {
    RTSP_TYPE_UNKNOWN,
    RTSP_TYPE_REQUEST,
    RTSP_TYPE_RESPONSE
} eRTSP_TYPE;

typedef enum {
    RTSP_VERSION_UNKNOWN,
    RTSP_VERSION_UNSUPPORTED,
    RTSP_VERSION_1_0
} eRTSP_VERSION;

const char * rtsp_internal_header_to_string(eRTSP_HEADER h);
const char * rtsp_internal_method_to_string(eRTSP_METHOD m);

//Version strings
extern const char * cRTSP_INTERNAL_VERSION_1_0;
extern const char * cRTSP_INTERNAL_VERSION_2_0;

//Methods
extern const char * cRTSP_INTERNAL_METHOD_DESCRIBE;
extern const char * cRTSP_INTERNAL_METHOD_ANNOUNCE;
extern const char * cRTSP_INTERNAL_METHOD_GET_PARAMETER;
extern const char * cRTSP_INTERNAL_METHOD_OPTIONS;
extern const char * cRTSP_INTERNAL_METHOD_PAUSE;
extern const char * cRTSP_INTERNAL_METHOD_PLAY;
extern const char * cRTSP_INTERNAL_METHOD_RECORD;
extern const char * cRTSP_INTERNAL_METHOD_REDIRECT;
extern const char * cRTSP_INTERNAL_METHOD_SETUP;
extern const char * cRTSP_INTERNAL_METHOD_SET_PARAMETER;
extern const char * cRTSP_INTERNAL_METHOD_TEARDOWN;

//Known header strings
extern const char * cRTSP_INTERNAL_HEADER_CACHE_CONTROL;
extern const char * cRTSP_INTERNAL_HEADER_CONNECTION;
extern const char * cRTSP_INTERNAL_HEADER_DATE;
extern const char * cRTSP_INTERNAL_HEADER_VIA;
extern const char * cRTSP_INTERNAL_HEADER_ACCEPT;
extern const char * cRTSP_INTERNAL_HEADER_ACCEPT_ENCODING;
extern const char * cRTSP_INTERNAL_HEADER_ACCEPT_LANGUAGE;
extern const char * cRTSP_INTERNAL_HEADER_AUTHORIZATION;
extern const char * cRTSP_INTERNAL_HEADER_FROM;
extern const char * cRTSP_INTERNAL_HEADER_IF_MODIFIED_SINCE;
extern const char * cRTSP_INTERNAL_HEADER_RANGE;
extern const char * cRTSP_INTERNAL_HEADER_REFERER;
extern const char * cRTSP_INTERNAL_HEADER_USER_AGENT;
extern const char * cRTSP_INTERNAL_HEADER_LOCATION;
extern const char * cRTSP_INTERNAL_HEADER_PROXY_AUTHENTICATE;
extern const char * cRTSP_INTERNAL_HEADER_PUBLIC;
extern const char * cRTSP_INTERNAL_HEADER_RETRY_AFTER;
extern const char * cRTSP_INTERNAL_HEADER_SERVER;
extern const char * cRTSP_INTERNAL_HEADER_VARY;
extern const char * cRTSP_INTERNAL_HEADER_WWW_AUTHENTICATE;
extern const char * cRTSP_INTERNAL_HEADER_ALLOW;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_BASE;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_ENCODING;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_LANGUAGE;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_LENGTH;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_LOCATION;
extern const char * cRTSP_INTERNAL_HEADER_CONTENT_TYPE;
extern const char * cRTSP_INTERNAL_HEADER_EXPIRES;
extern const char * cRTSP_INTERNAL_HEADER_LAST_MODIFIED;
extern const char * cRTSP_INTERNAL_HEADER_BANDWITH;
extern const char * cRTSP_INTERNAL_HEADER_BLOCKSIZE;
extern const char * cRTSP_INTERNAL_HEADER_CONFERENCE;
extern const char * cRTSP_INTERNAL_HEADER_CSEQ;
extern const char * cRTSP_INTERNAL_HEADER_HOST;
extern const char * cRTSP_INTERNAL_HEADER_IF_MATCH;
extern const char * cRTSP_INTERNAL_HEADER_PROXY_REQUIRE;
extern const char * cRTSP_INTERNAL_HEADER_REQUIRE;
extern const char * cRTSP_INTERNAL_HEADER_RTP_INFO;
extern const char * cRTSP_INTERNAL_HEADER_SCALE;
extern const char * cRTSP_INTERNAL_HEADER_SPEED;
extern const char * cRTSP_INTERNAL_HEADER_SESSION;
extern const char * cRTSP_INTERNAL_HEADER_TIMESTAMP;
extern const char * cRTSP_INTERNAL_HEADER_TRANSPORT;
extern const char * cRTSP_INTERNAL_HEADER_UNSUPPORTED;

#endif //RTSP_PARSER_RTSP_INTERNAL_H
