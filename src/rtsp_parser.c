#include <rtsp_parser.h>
#include <rtsp_internal.h>


#define MAX_RTSP_REQUEST_URI_SIZE 128
#define MAX_RTSP_HEADER_CNT 10
#define MAX_RTSP_HEADER_BUFFER 896

const rtsp_parser_handle rtsp_parser_invalid_handle = NULL;

typedef enum {
    R_PS_INIT,

    R_PS_REQUEST_METHOD_DESCRIBE,
    R_PS_REQUEST_METHOD_ANNOUNCE,
    R_PS_REQUEST_METHOD_GET_PARAMETER,
    R_PS_REQUEST_METHOD_OPTIONS,
    R_PS_REQUEST_METHOD_P,
    R_PS_REQUEST_METHOD_PAUSE,
    R_PS_REQUEST_METHOD_PLAY,
    R_PS_REQUEST_METHOD_RE,
    R_PS_REQUEST_METHOD_RECORD,
    R_PS_REQUEST_METHOD_REDIRECT,
    R_PS_REQUEST_METHOD_SET,
    R_PS_REQUEST_METHOD_SETUP,
    R_PS_REQUEST_METHOD_SET_PARAMETER,
    R_PS_REQUEST_METHOD_TEARDOWN,

    R_PS_REQUEST_SKIP_WS_METHOD,
    R_PS_REQUEST_URI,
    R_PS_REQUEST_SKIP_WS_URI,
    R_PS_REQUEST_VERSION,

    R_PS_RESPONSE_VERSION,
    R_PS_RESPONSE_STATUS_CODE,
    R_PS_RESPONSE_SKIP_REASON,

    R_PS_HEADER_NEWLINE,
    R_PS_HEADER_NAME,

    R_PS_HEADER_NAME_A,
    R_PS_HEADER_NAME_ACCEPT,
    R_PS_HEADER_NAME_ACCEPT_,
    R_PS_HEADER_NAME_ACCEPT_ENCODING,
    R_PS_HEADER_NAME_ACCEPT_LANGUAGE,
    R_PS_HEADER_NAME_ALLOW,
    R_PS_HEADER_NAME_AUTHORIZATION,
    R_PS_HEADER_NAME_B,
    R_PS_HEADER_NAME_BANDWITH,
    R_PS_HEADER_NAME_BLOCKSIZE,
    R_PS_HEADER_NAME_C,
    R_PS_HEADER_NAME_CACHE_CONTROL,
    R_PS_HEADER_NAME_CON,
    R_PS_HEADER_NAME_CONFERENCE,
    R_PS_HEADER_NAME_CONNECTION,
    R_PS_HEADER_NAME_CONTENT_,
    R_PS_HEADER_NAME_CONTENT_BASE,
    R_PS_HEADER_NAME_CONTENT_ENCODING,
    R_PS_HEADER_NAME_CONTENT_L,
    R_PS_HEADER_NAME_CONTENT_LANGUAGE,
    R_PS_HEADER_NAME_CONTENT_LENGTH,
    R_PS_HEADER_NAME_CONTENT_LOCATION,
    R_PS_HEADER_NAME_CONTENT_TYPE,
    R_PS_HEADER_NAME_CSEQ,
    R_PS_HEADER_NAME_DATE,
    R_PS_HEADER_NAME_EXPIRES,
    R_PS_HEADER_NAME_FROM,
    R_PS_HEADER_NAME_HOST,
    R_PS_HEADER_NAME_IF_M,
    R_PS_HEADER_NAME_IF_MATCH,
    R_PS_HEADER_NAME_IF_MODIFIED_SINCE,
    R_PS_HEADER_NAME_L,
    R_PS_HEADER_NAME_LAST_MODIFIED,
    R_PS_HEADER_NAME_LOCATION,
    R_PS_HEADER_NAME_P,
    R_PS_HEADER_NAME_PROXY_,
    R_PS_HEADER_NAME_PROXY_AUTHENTICATE,
    R_PS_HEADER_NAME_PROXY_REQUIRE,
    R_PS_HEADER_NAME_PUBLIC,
    R_PS_HEADER_NAME_R,
    R_PS_HEADER_NAME_RANGE,
    R_PS_HEADER_NAME_RE,
    R_PS_HEADER_NAME_REFERER,
    R_PS_HEADER_NAME_RETRY_AFTER,
    R_PS_HEADER_NAME_REQUIRE,
    R_PS_HEADER_NAME_RTP_Info,
    R_PS_HEADER_NAME_S,
    R_PS_HEADER_NAME_SCALE,
    R_PS_HEADER_NAME_SPEED,
    R_PS_HEADER_NAME_SE,
    R_PS_HEADER_NAME_SERVER,
    R_PS_HEADER_NAME_SESSION,
    R_PS_HEADER_NAME_T,
    R_PS_HEADER_NAME_TIMESTAMP,
    R_PS_HEADER_NAME_TRANSPORT,
    R_PS_HEADER_NAME_U,
    R_PS_HEADER_NAME_UNSUPPORTED,
    R_PS_HEADER_NAME_USER_AGENT,
    R_PS_HEADER_NAME_V,
    R_PS_HEADER_NAME_VARY,
    R_PS_HEADER_NAME_VIA,
    R_PS_HEADER_NAME_WWW_AUTHENTICATE,

    R_PS_HEADER_VALUE,
    R_PS_SKIP_HEADER,

    R_PS_BODY,

    R_PS_TERM_UNSUPPORTED,
} eRTSP_PARSER_STATE;

typedef struct {
    eRTSP_PARSER_STATE rtspState;
    uint32_t rtspPos;

    eRTSP_TYPE rtspType;
    eRTSP_METHOD rtspMethod;
    eRTSP_VERSION rtspVersion;

    uint8_t requestUri[MAX_RTSP_REQUEST_URI_SIZE];
    uint8_t headerBuffer[MAX_RTSP_HEADER_BUFFER];
    uint32_t headerBufferIndex;

    uint32_t statusCode;

    eRTSP_HEADER last_header;
    uint32_t last_header_value;
    uint32_t lastHeaderIndex;

    sRTSP_HEADER_ENTRY headers[MAX_RTSP_HEADER_CNT];
    uint32_t headerCount;
    uint32_t contentLength;

    cbRTSP_RESPONSE_HEADER cbRtspResponseHeader;
    cbRTSP_BODY cbRtspResponseBody;
    cbRTSP_REQUEST_HEADER cbRtspRequestHeader;
    cbRTSP_BODY cbRtspRequestBody;
    void * callback_arg;
} sRTSPParserState;

static bool isWhitespace(uint8_t byte) {
    switch (byte) {
        case ' ':
            return true;
        default:
            return false;
    }
}

static uint8_t ASCII2Digit(uint8_t byte) {
    if (byte >= '0' && byte <= '9') {
        return byte - '0';
    }
    return 0;
}

static uint8_t ascii_tolower(uint8_t byte) {
    if (byte >= 'A' && byte <= 'Z') {
        return byte - 'A' + 'a';
    }
    return byte;
}

static void header_callback(sRTSPParserState *state) {
    switch (state->rtspType) {
        case RTSP_TYPE_RESPONSE:
            if (state->cbRtspResponseHeader != NULL) {
                sRTSP_RESPONSE resp;
                resp.statuscode = (eRTSP_STATUSCODE) state->statusCode;
                resp.header = state->headers;
                resp.header_cnt = state->headerCount;
                state->cbRtspResponseHeader(state->callback_arg,state, resp);
            }
            break;
        case RTSP_TYPE_REQUEST:
            if (state->cbRtspRequestHeader != NULL) {
                sRTSP_REQUEST req;
                req.method = state->rtspMethod;
                req.uri = state->requestUri;
                req.header = state->headers;
                req.header_cnt = state->headerCount;
                state->cbRtspRequestHeader(state->callback_arg,state, req);
            }
            break;
    }
}

static void body_callback(sRTSPParserState *state, uint8_t const *const data, uint32_t size) {
    switch (state->rtspType) {
        case RTSP_TYPE_RESPONSE:
            if (state->cbRtspResponseBody != NULL) {
                state->cbRtspResponseBody(state->callback_arg,state, data, size);
            }
            break;
        case RTSP_TYPE_REQUEST:
            if (state->cbRtspRequestBody != NULL) {
                state->cbRtspRequestBody(state->callback_arg,state, data, size);
            }
            break;
    }
}

bool rtsp_parser_register_arg(rtsp_parser_handle handle, void * arg){
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->callback_arg=arg;
}

bool register_rtsp_response_header_callback(rtsp_parser_handle handle, cbRTSP_RESPONSE_HEADER cb) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->cbRtspResponseHeader = cb;
}

bool register_rtsp_response_body_callback(rtsp_parser_handle handle, cbRTSP_BODY cb) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->cbRtspResponseBody = cb;
}

bool register_rtsp_request_header_callback(rtsp_parser_handle handle, cbRTSP_REQUEST_HEADER cb) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->cbRtspRequestHeader = cb;
}

bool register_rtsp_request_body_callback(rtsp_parser_handle handle, cbRTSP_BODY cb) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->cbRtspRequestBody = cb;
}

uint32_t get_rtsp_parser_buffer_size(void) {
    return sizeof(sRTSPParserState);
}


bool create_rtsp_parser(rtsp_parser_handle *handle, uint8_t *buffer, uint32_t *size) {
    const uint32_t minimal_size = get_rtsp_parser_buffer_size();
    if (size == NULL) {
        //nullptr for size given - no possibility to return the right size
        return false;
    }
    if (handle == NULL || buffer == NULL || (*size) < minimal_size) {
        //nullptr's given  or size is to small - error case - but set the valid size
        if (handle != NULL) {
            *handle = rtsp_parser_invalid_handle;
        }
        *size = minimal_size;
        return false;
    }

    *handle = buffer;
    *size = minimal_size;

    sRTSPParserState *state = (sRTSPParserState *) handle;
    state->cbRtspResponseHeader = NULL;
    state->cbRtspResponseBody = NULL;
    state->cbRtspRequestHeader = NULL;
    state->cbRtspRequestBody = NULL;
    return reset_rtsp_parser(*handle);
}

bool reset_rtsp_parser(rtsp_parser_handle handle) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;

    state->rtspState = R_PS_INIT;
    state->rtspPos = 0;

    state->rtspType = RTSP_TYPE_UNKNOWN;
    state->rtspMethod = RTSP_METHOD_UNKNOWN;
    state->rtspVersion = RTSP_VERSION_UNKNOWN;

    state->requestUri[0] = '\0';
    state->statusCode = 0;

    state->last_header = RTSP_UNKOWN_HEADER;
    state->lastHeaderIndex = 0;
    state->headerBufferIndex = 0;

    state->headerCount = 0;
    state->contentLength = 0;
    return true;
}

// Tokenizer and LL(1) parser for RTSP packets - beware of very long method
bool parse_rtsp_data(rtsp_parser_handle handle, uint8_t const *const data, uint32_t size) {
    if (handle == rtsp_parser_invalid_handle) {
        return false;
    }

    sRTSPParserState *state = (sRTSPParserState *) handle;

    eRTSP_PARSER_STATE parser_state = state->rtspState;
    eRTSP_HEADER last_header = state->last_header;
    uint32_t lastHeaderIndex = state->lastHeaderIndex;
    uint32_t pos = state->rtspPos;
    uint32_t contentLength = state->contentLength;
    uint32_t headerCount = state->headerCount;
    uint32_t headerBufferIndex = state->headerBufferIndex;

    for (uint32_t i = 0; i < size; i++) {
        uint8_t byte = data[i];
        if (byte == '\0') {
            continue; //skip null bytes if introduced by any reception layer
        }
        switch (parser_state) {

            case R_PS_INIT:
                switch (pos) {
                    case 0:
                        switch (byte) {
                            //first letter of a known method -> request
                            case 'D':
                                parser_state = R_PS_REQUEST_METHOD_DESCRIBE;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'A':
                                parser_state = R_PS_REQUEST_METHOD_ANNOUNCE;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'G':
                                parser_state = R_PS_REQUEST_METHOD_GET_PARAMETER;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'O':
                                parser_state = R_PS_REQUEST_METHOD_OPTIONS;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'P':
                                parser_state = R_PS_REQUEST_METHOD_P;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'S':
                                parser_state = R_PS_REQUEST_METHOD_SET;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'T':
                                parser_state = R_PS_REQUEST_METHOD_TEARDOWN;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;


                                //first letter R - could either be request (RECORD,REDIRECT) or response (RTSP 1.0,RTSP 2.0)
                            case 'R':
                                parser_state = R_PS_INIT;
                                break;

                                //first letter not allowed - unsupported
                            default:
                                parser_state = R_PS_TERM_UNSUPPORTED;
                                break;
                        }
                        break;
                    case 1: //first letter was a R
                        switch (byte) {
                            case 'E': //second letter of known method -> request
                                parser_state = R_PS_REQUEST_METHOD_RE;
                                state->rtspType = RTSP_TYPE_REQUEST;
                                break;
                            case 'T': //second letter of possible version -> response
                                parser_state = R_PS_RESPONSE_VERSION;
                                state->rtspType = RTSP_TYPE_RESPONSE;
                                break;
                                //second letter not allowed - unsupported
                            default:
                                parser_state = R_PS_TERM_UNSUPPORTED;
                                break;
                        }
                }
                pos++;
                break;
//region Parser: Method - Parse Tree
            case R_PS_REQUEST_METHOD_P:
                switch (byte) {
                    case 'A':
                        parser_state = R_PS_REQUEST_METHOD_PAUSE;
                        break;
                    case 'L':
                        parser_state = R_PS_REQUEST_METHOD_PLAY;
                        break;
                    default:
                        parser_state = R_PS_TERM_UNSUPPORTED;
                        break;
                }
                pos++;
                break;
            case R_PS_REQUEST_METHOD_RE:
                switch (byte) {
                    case 'C':
                        parser_state = R_PS_REQUEST_METHOD_RECORD;
                        break;
                    case 'D':
                        parser_state = R_PS_REQUEST_METHOD_REDIRECT;
                        break;
                    default:
                        parser_state = R_PS_TERM_UNSUPPORTED;
                        break;
                }
                pos++;
                break;
            case R_PS_REQUEST_METHOD_SET:
                switch (pos) {
                    case 1:
                        if (byte != 'E') {
                            parser_state = R_PS_TERM_UNSUPPORTED;
                        }
                        break;
                    case 2:
                        if (byte != 'T') {
                            parser_state = R_PS_TERM_UNSUPPORTED;
                        }
                        break;
                    case 3:
                        switch (byte) {
                            case 'U':
                                parser_state = R_PS_REQUEST_METHOD_SETUP;
                                break;
                            case '_':
                                parser_state = R_PS_REQUEST_METHOD_SET_PARAMETER;
                                break;
                            default:
                                parser_state = R_PS_TERM_UNSUPPORTED;
                                break;
                        }
                        break;
                }
                pos++;
                break;

//avoid code duplication in the tokenizer/LL(1) parser
//every state parsing a method looks the same except of the parsed method
#define SIMPLE_METHOD_STATE(_expected, _method)      { \
                uint8_t exp_byte = (_expected)[pos]; \
                if (exp_byte!=byte){ \
                    if(exp_byte=='\0'){ \
                        state->rtspMethod=(_method); \
                        parser_state=R_PS_REQUEST_SKIP_WS_METHOD; \
                    } else { \
                        parser_state = R_PS_TERM_UNSUPPORTED; \
                    } \
                } \
                pos++; \
        }
            case R_PS_REQUEST_METHOD_DESCRIBE: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_DESCRIBE,
                                                                   RTSP_METHOD_DESCRIBE);
                break;
            case R_PS_REQUEST_METHOD_ANNOUNCE: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_ANNOUNCE,
                                                                   RTSP_METHOD_ANNOUNCE);
                break;
            case R_PS_REQUEST_METHOD_GET_PARAMETER: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_GET_PARAMETER,
                                                                        RTSP_METHOD_GET_PARAMETER);
                break;
            case R_PS_REQUEST_METHOD_OPTIONS: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_OPTIONS, RTSP_METHOD_OPTIONS);
                break;
            case R_PS_REQUEST_METHOD_PAUSE: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_PAUSE, RTSP_METHOD_PAUSE);
                break;
            case R_PS_REQUEST_METHOD_PLAY: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_PLAY, RTSP_METHOD_PLAY);
                break;
            case R_PS_REQUEST_METHOD_RECORD: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_RECORD, RTSP_METHOD_RECORD);
                break;
            case R_PS_REQUEST_METHOD_REDIRECT: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_REDIRECT,
                                                                   RTSP_METHOD_REDIRECT);
                break;
            case R_PS_REQUEST_METHOD_SETUP: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_SETUP, RTSP_METHOD_SETUP);
                break;
            case R_PS_REQUEST_METHOD_SET_PARAMETER: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_SET_PARAMETER,
                                                                        RTSP_METHOD_SET_PARAMETER);
                break;
            case R_PS_REQUEST_METHOD_TEARDOWN: SIMPLE_METHOD_STATE(cRTSP_INTERNAL_METHOD_TEARDOWN,
                                                                   RTSP_METHOD_TEARDOWN);
                break;

//endregion
            case R_PS_REQUEST_SKIP_WS_METHOD:
                if (!isWhitespace(byte)) {
                    pos = 1;
                    state->requestUri[0] = byte;
                    parser_state = R_PS_REQUEST_URI;
                }
                break;
            case R_PS_REQUEST_URI:
                if (isWhitespace(byte)) {
                    parser_state = R_PS_REQUEST_SKIP_WS_URI;
                } else if (pos < (MAX_RTSP_REQUEST_URI_SIZE - 1)) {
                    state->requestUri[pos] = byte;
                    state->requestUri[pos + 1] = '\0';
                    pos++;
                }
                break;
            case R_PS_REQUEST_SKIP_WS_URI:
                if (!isWhitespace(byte)) {
                    if (byte == 'R') { //first byte of version
                        pos = 1;
                        parser_state = R_PS_REQUEST_VERSION;
                    } else {
                        parser_state = R_PS_TERM_UNSUPPORTED;
                    }
                }
                break;
            case R_PS_REQUEST_VERSION: {
                uint8_t exp_byte = cRTSP_INTERNAL_VERSION_1_0[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        state->rtspVersion = RTSP_VERSION_1_0;
                        if (byte == '\n') {
                            pos = 0;
                            headerBufferIndex = 0;
                            headerCount = 0;
                            contentLength = 0;
                            parser_state = R_PS_HEADER_NAME;
                        } else {
                            parser_state = R_PS_HEADER_NEWLINE;
                        }
                    } else {
                        parser_state = R_PS_TERM_UNSUPPORTED;
                    }
                }
                pos++;
            }
                break;
            case R_PS_RESPONSE_VERSION: {
                uint8_t exp_byte = cRTSP_INTERNAL_VERSION_1_0[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        state->rtspVersion = RTSP_VERSION_1_0;
                        state->statusCode = ASCII2Digit(byte);
                        parser_state = R_PS_RESPONSE_STATUS_CODE;
                    } else {
                        parser_state = R_PS_TERM_UNSUPPORTED;
                    }
                }
                pos++;
            }
                break;
            case R_PS_RESPONSE_STATUS_CODE:
                if (!isWhitespace(byte)) {
                    uint32_t code = 10 * state->statusCode;
                    code += ASCII2Digit(byte);
                    state->statusCode = code;
                } else {
                    parser_state = R_PS_RESPONSE_SKIP_REASON;
                }
                break;
            case R_PS_RESPONSE_SKIP_REASON:
                if (byte == '\n') {
                    pos = 0;
                    headerBufferIndex = 0;
                    headerCount = 0;
                    contentLength = 0;
                    parser_state = R_PS_HEADER_NAME;
                }
                break;
            case R_PS_HEADER_NEWLINE:
                if (byte == '\n') {
                    pos = 0;
                    parser_state = R_PS_HEADER_NAME;
                }
                break;
            case R_PS_HEADER_NAME:
                if (byte == '\n') {
                    pos = 0;
                    parser_state = R_PS_BODY;
                    //The parsing of the header is finished
                    state->headerCount = headerCount;
                    header_callback(state);
                } else {
                    switch (ascii_tolower(byte)) {
                        case '\r':
                            parser_state = R_PS_HEADER_NAME;//ignore
                            break;
                        case 'a':
                            parser_state = R_PS_HEADER_NAME_A;
                            break;
                        case 'b':
                            parser_state = R_PS_HEADER_NAME_B;
                            break;
                        case 'c':
                            parser_state = R_PS_HEADER_NAME_C;
                            break;
                        case 'd':
                            parser_state = R_PS_HEADER_NAME_DATE;
                            break;
                        case 'e':
                            parser_state = R_PS_HEADER_NAME_EXPIRES;
                            break;
                        case 'f':
                            parser_state = R_PS_HEADER_NAME_FROM;
                            break;
                        case 'h':
                            parser_state = R_PS_HEADER_NAME_HOST;
                            break;
                        case 'i':
                            parser_state = R_PS_HEADER_NAME_IF_M;
                            break;
                        case 'l':
                            parser_state = R_PS_HEADER_NAME_L;
                            break;
                        case 'p':
                            parser_state = R_PS_HEADER_NAME_P;
                            break;
                        case 'r':
                            parser_state = R_PS_HEADER_NAME_R;
                            break;
                        case 's':
                            parser_state = R_PS_HEADER_NAME_S;
                            break;
                        case 't':
                            parser_state = R_PS_HEADER_NAME_T;
                            break;
                        case 'u':
                            parser_state = R_PS_HEADER_NAME_U;
                            break;
                        case 'v':
                            parser_state = R_PS_HEADER_NAME_V;
                            break;
                        case 'w':
                            parser_state = R_PS_HEADER_NAME_WWW_AUTHENTICATE;
                            break;
                        default:
                            parser_state = R_PS_SKIP_HEADER;
                            break;
                    }
                    pos++;
                }
                break;
//region Parser: Header - Parse Tree
            case R_PS_HEADER_NAME_A:
                switch (ascii_tolower(byte)) {
                    case 'c':
                        parser_state = R_PS_HEADER_NAME_ACCEPT;
                        break;
                    case 'l':
                        parser_state = R_PS_HEADER_NAME_ALLOW;
                        break;
                    case 'u':
                        parser_state = R_PS_HEADER_NAME_AUTHORIZATION;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_ACCEPT: {
                byte = ascii_tolower(byte);
                uint8_t exp_byte = cRTSP_INTERNAL_HEADER_ACCEPT[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        switch (byte) {
                            case ':':
                                last_header = RTSP_HEADER_ACCEPT;
                                lastHeaderIndex = headerBufferIndex;
                                parser_state = R_PS_HEADER_VALUE;
                                pos = 0;
                                break;
                            case '-':
                                parser_state = R_PS_HEADER_NAME_ACCEPT_;
                                pos++;
                                break;
                            default:
                                parser_state = R_PS_SKIP_HEADER;
                                break;
                        }
                    } else {
                        parser_state = R_PS_SKIP_HEADER;
                    }
                } else {
                    pos++;
                }

            }
                break;
            case R_PS_HEADER_NAME_ACCEPT_:
                switch (ascii_tolower(byte)) {
                    case 'e':
                        parser_state = R_PS_HEADER_NAME_ACCEPT_ENCODING;
                        break;
                    case 'l':
                        parser_state = R_PS_HEADER_NAME_ACCEPT_LANGUAGE;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_B:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_BANDWITH;
                        break;
                    case 'l':
                        parser_state = R_PS_HEADER_NAME_BLOCKSIZE;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_C:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_CACHE_CONTROL;
                        break;
                    case 'o':
                        parser_state = R_PS_HEADER_NAME_CON;
                        break;
                    case 's':
                        parser_state = R_PS_HEADER_NAME_CSEQ;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_CON: {
                byte = ascii_tolower(byte);
                uint8_t exp_byte = "con"[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        switch (byte) {
                            case 'f':
                                parser_state = R_PS_HEADER_NAME_CONFERENCE;
                                break;
                            case 'n':
                                parser_state = R_PS_HEADER_NAME_CONNECTION;
                                break;
                            case 't':
                                parser_state = R_PS_HEADER_NAME_CONTENT_;
                                break;
                            default:
                                parser_state = R_PS_SKIP_HEADER;
                                break;
                        }
                    } else {
                        parser_state = R_PS_SKIP_HEADER;
                    }
                }
                pos++;
            }
                break;
            case R_PS_HEADER_NAME_CONTENT_: {
                byte = ascii_tolower(byte);
                uint8_t exp_byte = "content-"[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        switch (byte) {
                            case 'b':
                                parser_state = R_PS_HEADER_NAME_CONTENT_BASE;
                                break;
                            case 'e':
                                parser_state = R_PS_HEADER_NAME_CONTENT_ENCODING;
                                break;
                            case 'l':
                                parser_state = R_PS_HEADER_NAME_CONTENT_L;
                                break;
                            case 't':
                                parser_state = R_PS_HEADER_NAME_CONTENT_TYPE;
                                break;
                            default:
                                parser_state = R_PS_SKIP_HEADER;
                                break;
                        }
                    } else {
                        parser_state = R_PS_SKIP_HEADER;
                    }
                }
                pos++;
            }
                break;
            case R_PS_HEADER_NAME_CONTENT_L:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_CONTENT_LANGUAGE;
                        break;
                    case 'e':
                        parser_state = R_PS_HEADER_NAME_CONTENT_LENGTH;
                        break;
                    case 'o':
                        parser_state = R_PS_HEADER_NAME_CONTENT_LOCATION;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_IF_M: {
                byte = ascii_tolower(byte);
                uint8_t exp_byte = "if-m"[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        switch (byte) {
                            case 'a':
                                parser_state = R_PS_HEADER_NAME_IF_MATCH;
                                break;
                            case 'o':
                                parser_state = R_PS_HEADER_NAME_IF_MODIFIED_SINCE;
                                break;
                            default:
                                parser_state = R_PS_SKIP_HEADER;
                                break;
                        }
                    } else {
                        parser_state = R_PS_SKIP_HEADER;
                    }
                }
                pos++;
            }
                break;
            case R_PS_HEADER_NAME_L:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_LAST_MODIFIED;
                        break;
                    case 'o':
                        parser_state = R_PS_HEADER_NAME_LOCATION;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_P:
                switch (ascii_tolower(byte)) {
                    case 'r':
                        parser_state = R_PS_HEADER_NAME_PROXY_;
                        break;
                    case 'u':
                        parser_state = R_PS_HEADER_NAME_PUBLIC;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_PROXY_: {
                byte = ascii_tolower(byte);
                uint8_t exp_byte = "proxy-"[pos];
                if (exp_byte != byte) {
                    if (exp_byte == '\0') {
                        switch (byte) {
                            case 'a':
                                parser_state = R_PS_HEADER_NAME_PROXY_AUTHENTICATE;
                                break;
                            case 'r':
                                parser_state = R_PS_HEADER_NAME_PROXY_REQUIRE;
                                break;
                            default:
                                parser_state = R_PS_SKIP_HEADER;
                                break;
                        }
                    } else {
                        parser_state = R_PS_SKIP_HEADER;
                    }
                }
                pos++;
            }
                break;
            case R_PS_HEADER_NAME_R:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_RANGE;
                        break;
                    case 'e':
                        parser_state = R_PS_HEADER_NAME_RE;
                        break;
                    case 't':
                        parser_state = R_PS_HEADER_NAME_RTP_Info;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_RE:
                switch (ascii_tolower(byte)) {
                    case 'f':
                        parser_state = R_PS_HEADER_NAME_REFERER;
                        break;
                    case 't':
                        parser_state = R_PS_HEADER_NAME_RETRY_AFTER;
                        break;
                    case 'q':
                        parser_state = R_PS_HEADER_NAME_REQUIRE;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_S:
                switch (ascii_tolower(byte)) {
                    case 'c':
                        parser_state = R_PS_HEADER_NAME_SCALE;
                        break;
                    case 'p':
                        parser_state = R_PS_HEADER_NAME_SPEED;
                        break;
                    case 'e':
                        parser_state = R_PS_HEADER_NAME_SE;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_SE:
                switch (ascii_tolower(byte)) {
                    case 'r':
                        parser_state = R_PS_HEADER_NAME_SERVER;
                        break;
                    case 's':
                        parser_state = R_PS_HEADER_NAME_SESSION;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_T:
                switch (ascii_tolower(byte)) {
                    case 'i':
                        parser_state = R_PS_HEADER_NAME_TIMESTAMP;
                        break;
                    case 'r':
                        parser_state = R_PS_HEADER_NAME_TRANSPORT;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_U:
                switch (ascii_tolower(byte)) {
                    case 'n':
                        parser_state = R_PS_HEADER_NAME_UNSUPPORTED;
                        break;
                    case 's':
                        parser_state = R_PS_HEADER_NAME_USER_AGENT;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;
            case R_PS_HEADER_NAME_V:
                switch (ascii_tolower(byte)) {
                    case 'a':
                        parser_state = R_PS_HEADER_NAME_VARY;
                        break;
                    case 'i':
                        parser_state = R_PS_HEADER_NAME_VIA;
                        break;
                    default:
                        parser_state = R_PS_SKIP_HEADER;
                        break;
                }
                pos++;
                break;

//avoid code duplication in the tokenizer/LL(1) parser
//every state parsing a header looks the same except of the parsed header
#define SIMPLE_HEADER_STATE(_expected, _header)             { \
                byte = ascii_tolower(byte); \
                uint8_t exp_byte = (_expected)[pos]; \
                if (exp_byte!=byte){ \
                    if(exp_byte=='\0'){ \
                        switch(byte){ \
                            case ':': \
                                last_header=(_header);\
                                lastHeaderIndex=headerBufferIndex;\
                                parser_state=R_PS_HEADER_VALUE; \
                                pos=0; \
                                break; \
                            default: \
                                parser_state = R_PS_SKIP_HEADER; \
                                break; \
                        } \
                    } else { \
                        parser_state = R_PS_SKIP_HEADER; \
                    } \
                } else { \
                    pos++; \
                } \
        }

            case R_PS_HEADER_NAME_ACCEPT_ENCODING: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_ACCEPT_ENCODING,
                                                                       RTSP_HEADER_ACCEPT_ENCODING);
                break;
            case R_PS_HEADER_NAME_ACCEPT_LANGUAGE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_ACCEPT_LANGUAGE,
                                                                       RTSP_HEADER_ACCEPT_LANGUAGE);
                break;
            case R_PS_HEADER_NAME_ALLOW: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_ALLOW, RTSP_HEADER_ALLOW);
                break;
            case R_PS_HEADER_NAME_AUTHORIZATION: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_AUTHORIZATION,
                                                                     RTSP_HEADER_AUTHORIZATION);
                break;
            case R_PS_HEADER_NAME_BANDWITH: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_BANDWITH, RTSP_HEADER_BANDWITH);
                break;
            case R_PS_HEADER_NAME_BLOCKSIZE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_BLOCKSIZE,
                                                                 RTSP_HEADER_BLOCKSIZE);
                break;
            case R_PS_HEADER_NAME_CACHE_CONTROL: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CACHE_CONTROL,
                                                                     RTSP_HEADER_CACHE_CONTROL);
                break;
            case R_PS_HEADER_NAME_CONFERENCE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONFERENCE,
                                                                  RTSP_HEADER_CONFERENCE);
                break;
            case R_PS_HEADER_NAME_CONNECTION: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONNECTION,
                                                                  RTSP_HEADER_CONNECTION);
                break;
            case R_PS_HEADER_NAME_CONTENT_BASE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_BASE,
                                                                    RTSP_HEADER_CONTENT_BASE);
                break;
            case R_PS_HEADER_NAME_CONTENT_ENCODING: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_ENCODING,
                                                                        RTSP_HEADER_CONTENT_ENCODING);
                break;
            case R_PS_HEADER_NAME_CONTENT_LANGUAGE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_LANGUAGE,
                                                                        RTSP_HEADER_CONTENT_LANGUAGE);
                break;
            case R_PS_HEADER_NAME_CONTENT_LENGTH: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_LENGTH,
                                                                      RTSP_HEADER_CONTENT_LENGTH);
                break;
            case R_PS_HEADER_NAME_CONTENT_LOCATION: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_LOCATION,
                                                                        RTSP_HEADER_CONTENT_LOCATION);
                break;
            case R_PS_HEADER_NAME_CONTENT_TYPE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CONTENT_TYPE,
                                                                    RTSP_HEADER_CONTENT_TYPE);
                break;
            case R_PS_HEADER_NAME_CSEQ: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_CSEQ, RTSP_HEADER_CSEQ);
                break;
            case R_PS_HEADER_NAME_DATE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_DATE, RTSP_HEADER_DATE);
                break;
            case R_PS_HEADER_NAME_EXPIRES: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_EXPIRES, RTSP_HEADER_EXPIRES);
                break;
            case R_PS_HEADER_NAME_FROM: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_FROM, RTSP_HEADER_FROM);
                break;
            case R_PS_HEADER_NAME_HOST: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_HOST, RTSP_HEADER_HOST);
                break;
            case R_PS_HEADER_NAME_IF_MATCH: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_IF_MATCH, RTSP_HEADER_IF_MATCH);
                break;
            case R_PS_HEADER_NAME_IF_MODIFIED_SINCE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_IF_MODIFIED_SINCE,
                                                                         RTSP_HEADER_IF_MODIFIED_SINCE);
                break;
            case R_PS_HEADER_NAME_LAST_MODIFIED: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_LAST_MODIFIED,
                                                                     RTSP_HEADER_LAST_MODIFIED);
                break;
            case R_PS_HEADER_NAME_LOCATION: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_LOCATION, RTSP_HEADER_LOCATION);
                break;
            case R_PS_HEADER_NAME_PROXY_AUTHENTICATE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_PROXY_AUTHENTICATE,
                                                                          RTSP_HEADER_PROXY_AUTHENTICATE);
                break;
            case R_PS_HEADER_NAME_PROXY_REQUIRE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_REQUIRE,
                                                                     RTSP_HEADER_PROXY_REQUIRE);
                break;
            case R_PS_HEADER_NAME_PUBLIC: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_PUBLIC, RTSP_HEADER_PUBLIC);
                break;
            case R_PS_HEADER_NAME_RANGE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_RANGE, RTSP_HEADER_RANGE);
                break;
            case R_PS_HEADER_NAME_REFERER: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_REFERER, RTSP_HEADER_REFERER);
                break;
            case R_PS_HEADER_NAME_RETRY_AFTER: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_RETRY_AFTER,
                                                                   RTSP_HEADER_RETRY_AFTER);
                break;
            case R_PS_HEADER_NAME_REQUIRE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_REQUIRE, RTSP_HEADER_REQUIRE);
                break;
            case R_PS_HEADER_NAME_RTP_Info: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_RTP_INFO, RTSP_HEADER_RTP_INFO);
                break;
            case R_PS_HEADER_NAME_SCALE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_SCALE, RTSP_HEADER_SCALE);
                break;
            case R_PS_HEADER_NAME_SPEED: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_SPEED, RTSP_HEADER_SPEED);
                break;
            case R_PS_HEADER_NAME_SERVER: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_SERVER, RTSP_HEADER_SERVER);
                break;
            case R_PS_HEADER_NAME_SESSION: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_SESSION, RTSP_HEADER_SESSION);
                break;
            case R_PS_HEADER_NAME_TIMESTAMP: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_TIMESTAMP,
                                                                 RTSP_HEADER_TIMESTAMP);
                break;
            case R_PS_HEADER_NAME_TRANSPORT: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_TRANSPORT,
                                                                 RTSP_HEADER_TRANSPORT);
                break;
            case R_PS_HEADER_NAME_UNSUPPORTED: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_UNSUPPORTED,
                                                                   RTSP_HEADER_UNSUPPORTED);
                break;
            case R_PS_HEADER_NAME_USER_AGENT: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_USER_AGENT,
                                                                  RTSP_HEADER_USER_AGENT);
                break;
            case R_PS_HEADER_NAME_VARY: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_VARY, RTSP_HEADER_VARY);
                break;
            case R_PS_HEADER_NAME_VIA: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_VIA, RTSP_HEADER_VIA);
                break;
            case R_PS_HEADER_NAME_WWW_AUTHENTICATE: SIMPLE_HEADER_STATE(cRTSP_INTERNAL_HEADER_WWW_AUTHENTICATE,
                                                                        RTSP_HEADER_WWW_AUTHENTICATE);
                break;
//endregion
            case R_PS_HEADER_VALUE:
                switch (last_header) {
                    case RTSP_HEADER_CONTENT_LENGTH:
                        if (byte == '\r' || byte == '\n') {
                            parser_state = R_PS_HEADER_NEWLINE;
                            contentLength = state->last_header_value;
                            if (headerCount < MAX_RTSP_HEADER_CNT) {
                                state->headers[headerCount].key = last_header;
                                state->headers[headerCount].value.number = state->last_header_value;
                                headerCount++;
                            }
                        } else if (pos == 0) {
                            state->last_header_value = ASCII2Digit(byte);
                        } else {
                            state->last_header_value *= 10;
                            state->last_header_value += ASCII2Digit(byte);
                        }
                        pos++;
                        break;

                    default:
                        if (byte == '\r' || byte == '\n') {
                            state->headerBuffer[headerBufferIndex++] = '\0';
                            parser_state = R_PS_HEADER_NEWLINE;
                            if (headerCount < MAX_RTSP_HEADER_CNT) {
                                state->headers[headerCount].key = last_header;
                                state->headers[headerCount].value.text = (char const *) &state->headerBuffer[lastHeaderIndex];
                                headerCount++;
                            }
                        } else if (headerBufferIndex < (MAX_RTSP_HEADER_BUFFER - 1)) {
                            state->headerBuffer[headerBufferIndex++] = byte;
                            state->headerBuffer[headerBufferIndex] = '\0';
                        }
                        break;
                }
                break;
            case R_PS_SKIP_HEADER:
                if (byte == '\r' || byte == '\n') {
                    parser_state = R_PS_HEADER_NEWLINE;
                }
                break;

            case R_PS_BODY:
                if (pos < contentLength) {
                    const uint32_t remainingBodyBytes = (contentLength - pos);
                    uint32_t remainingFrameBytes = (size - i);
                    if (remainingBodyBytes < remainingFrameBytes) {
                        remainingFrameBytes = remainingBodyBytes;
                    }
                    body_callback(state, &data[i], remainingFrameBytes);
                    pos += remainingFrameBytes;
                    i += remainingFrameBytes - 1;
                } else {
                    //data_overflow
                }
                break;
            default:
                break;
        }

    }


    if (parser_state == R_PS_BODY && pos == contentLength) {
        //Reached the end of an rtsp message (content length has to be given all the time)
        reset_rtsp_parser(handle);
    } else {
        state->rtspPos = pos;
        state->rtspState = parser_state;
        state->last_header = last_header;
        state->lastHeaderIndex = lastHeaderIndex;
        state->headerCount = headerCount;
        state->headerBufferIndex = headerBufferIndex;
        state->contentLength = contentLength;
    }

    return true;
}