//
// Created by christianb on 19.07.20.
//

#include <rtsp_internal.h>

const char * rtsp_internal_header_to_string(eRTSP_HEADER h){
switch(h){
    case RTSP_HEADER_CACHE_CONTROL: return cRTSP_INTERNAL_HEADER_CACHE_CONTROL;
    case RTSP_HEADER_CONNECTION: return cRTSP_INTERNAL_HEADER_CONNECTION;
    case RTSP_HEADER_DATE: return cRTSP_INTERNAL_HEADER_DATE;
    case RTSP_HEADER_VIA: return cRTSP_INTERNAL_HEADER_VIA;
    case RTSP_HEADER_ACCEPT: return cRTSP_INTERNAL_HEADER_ACCEPT;
    case RTSP_HEADER_ACCEPT_ENCODING: return cRTSP_INTERNAL_HEADER_ACCEPT_ENCODING;
    case RTSP_HEADER_ACCEPT_LANGUAGE: return cRTSP_INTERNAL_HEADER_ACCEPT_LANGUAGE;
    case RTSP_HEADER_AUTHORIZATION: return cRTSP_INTERNAL_HEADER_AUTHORIZATION;
    case RTSP_HEADER_FROM: return cRTSP_INTERNAL_HEADER_FROM;
    case RTSP_HEADER_IF_MODIFIED_SINCE: return cRTSP_INTERNAL_HEADER_IF_MODIFIED_SINCE;
    case RTSP_HEADER_RANGE: return cRTSP_INTERNAL_HEADER_RANGE;
    case RTSP_HEADER_REFERER:return cRTSP_INTERNAL_HEADER_REFERER;
    case RTSP_HEADER_USER_AGENT: return cRTSP_INTERNAL_HEADER_USER_AGENT;

        //Response Header
    case RTSP_HEADER_LOCATION: return cRTSP_INTERNAL_HEADER_LOCATION;
    case RTSP_HEADER_PROXY_AUTHENTICATE: return cRTSP_INTERNAL_HEADER_PROXY_AUTHENTICATE;
    case RTSP_HEADER_PUBLIC: return cRTSP_INTERNAL_HEADER_PUBLIC;
    case RTSP_HEADER_RETRY_AFTER: return cRTSP_INTERNAL_HEADER_RETRY_AFTER;
    case RTSP_HEADER_SERVER: return cRTSP_INTERNAL_HEADER_SERVER;
    case RTSP_HEADER_VARY: return cRTSP_INTERNAL_HEADER_VARY;
    case RTSP_HEADER_WWW_AUTHENTICATE: return cRTSP_INTERNAL_HEADER_WWW_AUTHENTICATE;

        //Entity Header
    case RTSP_HEADER_ALLOW: return cRTSP_INTERNAL_HEADER_ALLOW;
    case RTSP_HEADER_CONTENT_BASE: return cRTSP_INTERNAL_HEADER_CONTENT_BASE;
    case RTSP_HEADER_CONTENT_ENCODING: return cRTSP_INTERNAL_HEADER_CONTENT_ENCODING;
    case RTSP_HEADER_CONTENT_LANGUAGE: return cRTSP_INTERNAL_HEADER_CONTENT_LANGUAGE;
    case RTSP_HEADER_CONTENT_LENGTH: return cRTSP_INTERNAL_HEADER_CONTENT_LENGTH;
    case RTSP_HEADER_CONTENT_LOCATION: return cRTSP_INTERNAL_HEADER_CONTENT_LOCATION;
    case RTSP_HEADER_CONTENT_TYPE: return cRTSP_INTERNAL_HEADER_CONTENT_TYPE;
    case RTSP_HEADER_EXPIRES: return cRTSP_INTERNAL_HEADER_EXPIRES;
    case RTSP_HEADER_LAST_MODIFIED: return cRTSP_INTERNAL_HEADER_LAST_MODIFIED;

        // other
    case RTSP_HEADER_BANDWITH: return cRTSP_INTERNAL_HEADER_BANDWITH;
    case RTSP_HEADER_BLOCKSIZE: return cRTSP_INTERNAL_HEADER_BLOCKSIZE;
    case RTSP_HEADER_CONFERENCE: return cRTSP_INTERNAL_HEADER_CONFERENCE;
    case RTSP_HEADER_CSEQ: return cRTSP_INTERNAL_HEADER_CSEQ;
    case RTSP_HEADER_HOST: return cRTSP_INTERNAL_HEADER_HOST;
    case RTSP_HEADER_IF_MATCH: return cRTSP_INTERNAL_HEADER_IF_MATCH;
    case RTSP_HEADER_PROXY_REQUIRE: return cRTSP_INTERNAL_HEADER_PROXY_REQUIRE;
    case RTSP_HEADER_REQUIRE: return cRTSP_INTERNAL_HEADER_REQUIRE;
    case RTSP_HEADER_RTP_INFO: return cRTSP_INTERNAL_HEADER_RTP_INFO;
    case RTSP_HEADER_SCALE: return cRTSP_INTERNAL_HEADER_SCALE;
    case RTSP_HEADER_SPEED: return cRTSP_INTERNAL_HEADER_SPEED;
    case RTSP_HEADER_SESSION: return cRTSP_INTERNAL_HEADER_SESSION;
    case RTSP_HEADER_TIMESTAMP: return cRTSP_INTERNAL_HEADER_TIMESTAMP;
    case RTSP_HEADER_TRANSPORT: return cRTSP_INTERNAL_HEADER_TRANSPORT;
    case RTSP_HEADER_UNSUPPORTED: return cRTSP_INTERNAL_HEADER_UNSUPPORTED;
    default: return NULL;
}
}
const char * rtsp_internal_method_to_string(eRTSP_METHOD m){
    switch(m){
        case RTSP_METHOD_DESCRIBE: return cRTSP_INTERNAL_METHOD_DESCRIBE;
        case RTSP_METHOD_ANNOUNCE: return cRTSP_INTERNAL_METHOD_DESCRIBE;
        case RTSP_METHOD_GET_PARAMETER: return cRTSP_INTERNAL_METHOD_GET_PARAMETER;
        case RTSP_METHOD_OPTIONS: return cRTSP_INTERNAL_METHOD_OPTIONS;
        case RTSP_METHOD_PAUSE: return cRTSP_INTERNAL_METHOD_PAUSE;
        case RTSP_METHOD_PLAY: return cRTSP_INTERNAL_METHOD_PLAY;
        case RTSP_METHOD_RECORD: return cRTSP_INTERNAL_METHOD_RECORD;
        case RTSP_METHOD_REDIRECT: return cRTSP_INTERNAL_METHOD_REDIRECT;
        case RTSP_METHOD_SETUP: return cRTSP_INTERNAL_METHOD_SETUP;
        case RTSP_METHOD_SET_PARAMETER: return cRTSP_INTERNAL_METHOD_SET_PARAMETER;
        case RTSP_METHOD_TEARDOWN: return cRTSP_INTERNAL_METHOD_TEARDOWN;
        default: return NULL;
    }
}

//Version strings
const char * cRTSP_INTERNAL_VERSION_1_0 ="RTSP/1.0";
const char * cRTSP_INTERNAL_VERSION_2_0 ="RTSP/2.0";

//Methods
const char * cRTSP_INTERNAL_METHOD_DESCRIBE = "DESCRIBE";
const char * cRTSP_INTERNAL_METHOD_ANNOUNCE = "ANNOUNCE";
const char * cRTSP_INTERNAL_METHOD_GET_PARAMETER = "GET_PARAMETER";
const char * cRTSP_INTERNAL_METHOD_OPTIONS = "OPTIONS";
const char * cRTSP_INTERNAL_METHOD_PAUSE = "PAUSE";
const char * cRTSP_INTERNAL_METHOD_PLAY = "PLAY";
const char * cRTSP_INTERNAL_METHOD_RECORD = "RECORD";
const char * cRTSP_INTERNAL_METHOD_REDIRECT = "REDIRECT";
const char * cRTSP_INTERNAL_METHOD_SETUP = "SETUP";
const char * cRTSP_INTERNAL_METHOD_SET_PARAMETER = "SET_PARAMETER";
const char * cRTSP_INTERNAL_METHOD_TEARDOWN = "TEARDOWN";

//Known header strings
const char * cRTSP_INTERNAL_HEADER_CACHE_CONTROL = "cache-control";
const char * cRTSP_INTERNAL_HEADER_CONNECTION = "connection";
const char * cRTSP_INTERNAL_HEADER_DATE = "date";
const char * cRTSP_INTERNAL_HEADER_VIA = "via";
const char * cRTSP_INTERNAL_HEADER_ACCEPT = "accept";
const char * cRTSP_INTERNAL_HEADER_ACCEPT_ENCODING = "accept-encoding";
const char * cRTSP_INTERNAL_HEADER_ACCEPT_LANGUAGE = "accept-language";
const char * cRTSP_INTERNAL_HEADER_AUTHORIZATION = "authorization";
const char * cRTSP_INTERNAL_HEADER_FROM = "from";
const char * cRTSP_INTERNAL_HEADER_IF_MODIFIED_SINCE = "if-modified-since";
const char * cRTSP_INTERNAL_HEADER_RANGE = "range";
const char * cRTSP_INTERNAL_HEADER_REFERER = "referer";
const char * cRTSP_INTERNAL_HEADER_USER_AGENT = "user-agent";
const char * cRTSP_INTERNAL_HEADER_LOCATION = "location";
const char * cRTSP_INTERNAL_HEADER_PROXY_AUTHENTICATE = "proxy-authenticate";
const char * cRTSP_INTERNAL_HEADER_PUBLIC = "public";
const char * cRTSP_INTERNAL_HEADER_RETRY_AFTER = "retry-after";
const char * cRTSP_INTERNAL_HEADER_SERVER = "server";
const char * cRTSP_INTERNAL_HEADER_VARY = "vary";
const char * cRTSP_INTERNAL_HEADER_WWW_AUTHENTICATE = "www-authenticate";
const char * cRTSP_INTERNAL_HEADER_ALLOW = "allow";
const char * cRTSP_INTERNAL_HEADER_CONTENT_BASE = "content-base";
const char * cRTSP_INTERNAL_HEADER_CONTENT_ENCODING = "content-encoding";
const char * cRTSP_INTERNAL_HEADER_CONTENT_LANGUAGE = "content-language";
const char * cRTSP_INTERNAL_HEADER_CONTENT_LENGTH = "content-length";
const char * cRTSP_INTERNAL_HEADER_CONTENT_LOCATION = "content-location";
const char * cRTSP_INTERNAL_HEADER_CONTENT_TYPE = "content-type";
const char * cRTSP_INTERNAL_HEADER_EXPIRES = "expires";
const char * cRTSP_INTERNAL_HEADER_LAST_MODIFIED = "last-modified";
const char * cRTSP_INTERNAL_HEADER_BANDWITH = "bandwidth";
const char * cRTSP_INTERNAL_HEADER_BLOCKSIZE = "blocksize";
const char * cRTSP_INTERNAL_HEADER_CONFERENCE = "conference";
const char * cRTSP_INTERNAL_HEADER_CSEQ = "cseq";
const char * cRTSP_INTERNAL_HEADER_HOST = "host";
const char * cRTSP_INTERNAL_HEADER_IF_MATCH = "if-match";
const char * cRTSP_INTERNAL_HEADER_PROXY_REQUIRE = "proxy-require";
const char * cRTSP_INTERNAL_HEADER_REQUIRE = "require";
const char * cRTSP_INTERNAL_HEADER_RTP_INFO = "rtp-info";
const char * cRTSP_INTERNAL_HEADER_SCALE = "scale";
const char * cRTSP_INTERNAL_HEADER_SPEED = "speed";
const char * cRTSP_INTERNAL_HEADER_SESSION = "session";
const char * cRTSP_INTERNAL_HEADER_TIMESTAMP = "timestamp";
const char * cRTSP_INTERNAL_HEADER_TRANSPORT = "transport";
const char * cRTSP_INTERNAL_HEADER_UNSUPPORTED = "unsupported";