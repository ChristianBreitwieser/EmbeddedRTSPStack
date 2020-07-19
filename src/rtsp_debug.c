//
// Created by christianb on 19.07.20.
//

#include <rtsp_debug.h>
#include <rtsp_internal.h>

char const * rtsp_debug_method_to_string(eRTSP_METHOD m){
    switch(m){
        case RTSP_METHOD_UNKNOWN: return "Unknown method";
        case RTSP_METHOD_UNSUPPORTED: return "Unsupported method";
        default: {
            const char * possible=rtsp_internal_method_to_string(m);
            if(possible==NULL){
                return "";
            } else {
                return possible;
            }
        }
    }
}

char const * rtsp_debug_statuscode_to_string(eRTSP_STATUSCODE m){
    switch(m){
        case RTSP_STATUSCODE_CONTINUE: return "100 continue";
        case RTSP_STATUSCODE_OK: return "200 ok";
        case RTSP_STATUSCODE_CREATED: return "201 created";
        case RTSP_STATUSCODE_LOW_ON_STORAGE_SPACE: return "250 low on storage space";
        case RTSP_STATUSCODE_MULTIPLE_CHOICES: return "300 multiple choices";
        case RTSP_STATUSCODE_MOVED_PERMANENTLY: return "301 moved permanently";
        case RTSP_STATUSCODE_MOVED_TEMPORARILY: return "302 moved temporarily";
        case RTSP_STATUSCODE_SEE_OTHER: return "303 see other";
        case RTSP_STATUSCODE_NOT_MODIFIED: return "304 not modified";
        case RTSP_STATUSCODE_USE_PROXY: return "305 use proxy";
        case RTSP_STATUSCODE_BAD_REQUEST: return "400 bad request";
        case RTSP_STATUSCODE_UNAUTHORIZED: return "401 unauthorized";
        case RTSP_STATUSCODE_PAYMENT_REQUIRED: return "402 payment required";
        case RTSP_STATUSCODE_FORBIDDEN: return "403 forbidden";
        case RTSP_STATUSCODE_NOT_FOUND: return "404 not found";
        case RTSP_STATUSCODE_METHOD_NOT_ALLOWED: return "405 method not allowed";
        case RTSP_STATUSCODE_NOT_ACCEPTABLE: return "406 not acceptable";
        case RTSP_STATUSCODE_PROXY_AUTHENTICATION_REQUIRED: return "407 proxy authentication required";
        case RTSP_STATUSCODE_REQUEST_TIMED_OUT: return "408 request timed out";
        case RTSP_STATUSCODE_GONE: return "410 gone";
        case RTSP_STATUSCODE_LENGTH_REQUIRED: return "411 length required";
        case RTSP_STATUSCODE_PRECONDITION_FAILED: return "412 precondition failed";
        case RTSP_STATUSCODE_REQUEST_ENTITY_TO_LARGE: return "413 request entity to large";
        case RTSP_STATUSCODE_REQUEST_URI_TO_LARGE: return "414 request uri to large";
        case RTSP_STATUSCODE_UNSUPPORTED_MEDIA_TYPE: return "415 unsupported media type";
        case RTSP_STATUSCODE_PARAMETER_NOT_UNDERSTOOD: return "451 parameter not understood";
        case RTSP_STATUSCODE_CONFERENCE_NOT_FOUND: return "452 conference not found";
        case RTSP_STATUSCODE_NOT_ENOUGH_BANDWITH: return "453 not enough bandwidth";
        case RTSP_STATUSCODE_SESSION_NOT_FOUND: return "454 session not found";
        case RTSP_STATUSCODE_METHOD_NOT_VALID_IN_THIS_STATE: return "455 method not valid in this state";
        case RTSP_STATUSCODE_HEADER_FIELD_NOT_VALID_FOR_RESSOURCE:return "456 header field not valid for ressource";
        case RTSP_STATUSCODE_INVALID_RANGE: return "457 invalid range";
        case RTSP_STATUSCODE_PARAMETER_IS_READ_ONLY: return "458 parameter is read only";
        case RTSP_STATUSCODE_AGGREGATE_OPERATION_NOT_ALLOWED: return "459 aggregate operation not allowed";
        case RTSP_STATUSCODE_ONLY_AGGREGATE_OPERATION_ALLOWED: return "460 only aggregate operation allowed";
        case RTSP_STATUSCODE_UNSUPPORTED_TRANSPORT: return "461 unsupported transport";
        case RTSP_STATUSCODE_DESTINATION_UNREACHABLE: return "462 destination unreachable";
        case RTSP_STATUSCODE_INTERNAL_SERVER_ERROR: return "500 internal server error";
        case RTSP_STATUSCODE_NOT_IMPLEMENTED: return "501 not implemented";
        case RTSP_STATUSCODE_BAD_GATEWAY: return "502 bad gateway";
        case RTSP_STATUSCODE_SERVICE_UNAVAILABLE: return "503 service unavailable";
        case RTSP_STATUSCODE_GATEWAY_TIME_OUT: return "504 gateway timeout";
        case RTSP_STATUSCODE_RTSP_VERSION_NOT_SUPPORTED: return "505 rtsp version not supported";
        case RTSP_STATUSCODE_OPTION_NOT_SUPPORTED: return "551 option not supported";
        default: {
            uint16_t status = m;
            status /= 100;
            switch (status) {
                case 1:
                    return "1XX continue";
                case 2:
                    return "2XX ok";
                case 3:
                    return "3XX multiple choices";
                case 4:
                    return "4XX bad request";
                case 5:
                    return "5XX internal server error";
                default:
                    return "Unknown statuscode";
            }
        }
            break;
    }
}

char const * rtsp_debug_header_to_string(eRTSP_HEADER h){
    const char * possible=rtsp_internal_header_to_string(h);
    if(possible==NULL){
        return "unknown header";
    } else {
        return possible;
    }
}