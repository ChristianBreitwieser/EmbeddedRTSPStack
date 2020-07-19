//
// Created by christianb on 18.07.20.
//
// Inspired by http://git.savannah.nongnu.org/cgit/lwip.git/tree/src/apps/http/httpd.c?h=STABLE-2_1_x
//

#include "rtsp_server.h"

#include <lwip/tcp.h>

#include "rtsp_parser.h"
#include "rtsp_debug.h"

#define RTSP_SERVER_MAX_BUFFER_PER_CLIENT 2048
#define RTSP_SERVER_MAX_NR_OF_CLIENTS 10

#define RTSP_TCP_PRIO 10

#define RTSP_POLL_INTERVAL 10

#define RTSP_SERVER_PORT 6000

#define RTSP_MAX_RETRIES 10

static err_t
rtsp_poll(void *arg, struct tcp_pcb *pcb);

typedef struct {
    uint8_t databuffer[RTSP_SERVER_MAX_BUFFER_PER_CLIENT];
    rtsp_parser_handle parser_handle;
    struct tcp_pcb * pcb;
    uint32_t retries;

    bool pending_data;
    uint32_t left; //remaining data
    uint8_t * sendbuffer;
} rtsp_server_state_t;


struct {
    rtsp_server_state_t endpoints[RTSP_SERVER_MAX_NR_OF_CLIENTS];
    bool allocated_endpoints[RTSP_SERVER_MAX_NR_OF_CLIENTS];
} endpoint_allocator_storage;

static bool _init_server_state(rtsp_server_state_t* ss){
    ss->pcb=NULL;
    ss->retries=0;
    ss->pending_data=false;
    ss->left=0;
    ss->sendbuffer=NULL;

    uint32_t parser_size=RTSP_SERVER_MAX_BUFFER_PER_CLIENT;
    return create_rtsp_parser(&ss->parser_handle,ss->databuffer,&parser_size);
}

static rtsp_server_state_t * allocate_endpoint(void){
    for(uint32_t i=0;i<RTSP_SERVER_MAX_NR_OF_CLIENTS;i++){
        if(!endpoint_allocator_storage.allocated_endpoints[i]){
            endpoint_allocator_storage.allocated_endpoints[i]=true;
            rtsp_server_state_t * ep =&endpoint_allocator_storage.endpoints[i];

            if(_init_server_state(ep)){
                return &endpoint_allocator_storage.endpoints[i];
            }
        }
    }
    return NULL;
}

static void free_endpoint(rtsp_server_state_t * endpoint){
    for(uint32_t i=0;i<RTSP_SERVER_MAX_NR_OF_CLIENTS;i++){
        if((&endpoint_allocator_storage.endpoints[i])==endpoint){
            endpoint_allocator_storage.allocated_endpoints[i]=false;
        }
    }
}

void rtsp_response_header(void * arg,rtsp_parser_handle h, sRTSP_RESPONSE resp){
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;

    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("RTSP-RESPONSE\n %s\n", rtsp_debug_statuscode_to_string(resp.statuscode)));
    for(uint32_t i=0;i<resp.header_cnt;i++){
        if(resp.header[i].key==RTSP_HEADER_CONTENT_LENGTH){
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE,
                        ("%s: %d\n", rtsp_debug_header_to_string(resp.header[i].key), resp.header[i].value.number));
        } else {
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE,
                        ("%s: %s\n", rtsp_debug_header_to_string(resp.header[i].key), resp.header[i].value.text));
        }
    }
}
void rtsp_request_header(void * arg, rtsp_parser_handle h, sRTSP_REQUEST resp){
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;

    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("RTSP-REQUEST\n%s %s\n", rtsp_debug_method_to_string(resp.method),resp.uri));
    for(uint32_t i=0;i<resp.header_cnt;i++){
        if(resp.header[i].key==RTSP_HEADER_CONTENT_LENGTH){
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE,
                        ("%s: %d\n", rtsp_debug_header_to_string(resp.header[i].key), resp.header[i].value.number));
        } else {
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE,
                        ("%s:%s\n", rtsp_debug_header_to_string(resp.header[i].key), resp.header[i].value.text));
        }
    }
}
void rtsp_body(void * arg,rtsp_parser_handle h, uint8_t const * const data, uint32_t size){
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;
}

static err_t
rtsp_write(struct tcp_pcb *pcb, const void *ptr, u16_t *length, u8_t apiflags)
{
    u16_t len, max_len;
    err_t err;
    LWIP_ASSERT("length != NULL", length != NULL);
    len = *length;
    if (len == 0) {
        return ERR_OK;
    }
    /* We cannot send more data than space available in the send buffer. */
    max_len = tcp_sndbuf(pcb);
    if (max_len < len) {
        len = max_len;
    }
    do {
        LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("Trying to send %d bytes\n", len));
        err = tcp_write(pcb, ptr, len, apiflags);
        if (err == ERR_MEM) {
            if ((tcp_sndbuf(pcb) == 0) ||
                (pcb->snd_queuelen >= TCP_SND_QUEUELEN)) {
                /* no need to try smaller sizes */
                len = 1;
            } else {
                len /= 2;
            }
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE,
                        ("Send failed, trying less (%d bytes)\n", len));
        }
    } while ((err == ERR_MEM) && (len > 1));

    if (err == ERR_OK) {
        LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("Sent %d bytes\n", len));
        *length = len;
    } else {
        LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("Send failed with err %d (\"%s\")\n", err, lwip_strerr(err)));
        *length = 0;
    }

    return err;
}

static err_t
rtsp_close_or_abort_conn(struct tcp_pcb *pcb,  rtsp_server_state_t *ss, u8_t abort_conn)
{
    err_t err;
    LWIP_DEBUGF(RTSP_DEBUG, ("Closing connection %p\n", (void *)pcb));

    tcp_arg(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_sent(pcb, NULL);
    if (ss != NULL) {
        free_endpoint(ss);
    }

    if (abort_conn) {
        tcp_abort(pcb);
        return ERR_OK;
    }
    err = tcp_close(pcb);
    if (err != ERR_OK) {
        LWIP_DEBUGF(RTSP_DEBUG, ("Error %d closing %p\n", err, (void *)pcb));
        /* error closing, try again later in poll */
        tcp_poll(pcb, rtsp_poll, RTSP_POLL_INTERVAL);
    }
    return err;
}

static err_t
rtsp_close_conn(struct tcp_pcb *pcb,  rtsp_server_state_t *ss)
{
    return rtsp_close_or_abort_conn(pcb, ss, 0);
}

static void
rtsp_eof(struct tcp_pcb *pcb, rtsp_server_state_t *ss)
{
    rtsp_close_conn(pcb, ss);
}

static bool
rtsp_send(struct tcp_pcb *pcb, rtsp_server_state_t *ss)
{
    bool data_to_send = false;

    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("rtsp_send: pcb=%p ss=%p left=%d\n", (void *)pcb,
            (void *)ss, ss != NULL ? (int)ss->left : 0));


    /* If we were passed a NULL state structure pointer, ignore the call. */
    if (ss == NULL) {
        return 0;
    }

    err_t err;
    u16_t len;

    len = (u16_t)LWIP_MIN(ss->left, 0xffff);

    err = rtsp_write(pcb, ss->sendbuffer, &len,0);
    if (err == ERR_OK) {
        data_to_send = true;
    }


    if (ss->left == 0) {
        /* We reached the end of the file so this request is done.
         * This adds the FIN flag right into the last data segment. */
        LWIP_DEBUGF(RTSP_DEBUG, ("End of file.\n"));
        rtsp_eof(pcb, ss);
        return 0;
    }
    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("send_data end.\n"));
    return data_to_send;
}

static void
rtsp_err(void *arg, err_t err)
{
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;
    LWIP_UNUSED_ARG(err);

    LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_err: %s", lwip_strerr(err)));

    if (ss != NULL) {
        free_endpoint(ss);
    }
}

static err_t
rtsp_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;

    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("rtsp_sent %p\n", (void *)pcb));

    LWIP_UNUSED_ARG(len);

    if (ss == NULL) {
        return ERR_OK;
    }

    ss->retries = 0;

    rtsp_send(pcb, ss);

    return ERR_OK;
}

static err_t
rtsp_poll(void *arg, struct tcp_pcb *pcb)
{
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;
    LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("rtsp_poll: pcb=%p ss=%p pcb_state=%s\n",
            (void *)pcb, (void *)ss, tcp_debug_state_str(pcb->state)));

    if (ss == NULL) {
        err_t closed;
        /* arg is null, close. */
        LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_poll: arg is NULL, close\n"));
        closed = rtsp_close_conn(pcb, NULL);
        LWIP_UNUSED_ARG(closed);
        return ERR_OK;
    } else {
        ss->retries++;
        if (ss->retries == RTSP_MAX_RETRIES) {
            LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_poll: too many retries, close\n"));
            rtsp_close_conn(pcb, ss);
            return ERR_OK;
        }

        /* If this connection has pending data, try to send some more data. If
         * it has not yet received a request, don't do this since it will
         * cause the connection to close immediately. */
        if (ss->pending_data) {
            LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("rtsp_poll: try to send more data\n"));
            if (rtsp_send(pcb, ss)) {
                /* If we wrote anything to be sent, go ahead and send it now. */
                LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
                tcp_output(pcb);
            }
        }
    }

    return ERR_OK;
}


static err_t rtsp_recv(void *arg, struct tcp_pcb * pcb, struct pbuf * p, err_t err){
    rtsp_server_state_t *ss = (rtsp_server_state_t *)arg;
        LWIP_DEBUGF(RTSP_DEBUG | LWIP_DBG_TRACE, ("rtsp_recv: pcb=%p pbuf=%p err=%s\n", (void *)pcb,
                (void *)p, lwip_strerr(err)));

        if ((err != ERR_OK) || (p == NULL) || (ss == NULL)) {
            /* error or closed by other side? */
            if (p != NULL) {
                /* Inform TCP that we have taken the data. */
                tcp_recved(pcb, p->tot_len);
                pbuf_free(p);
            }
            if (ss == NULL) {
                /* this should not happen, only to be robust */
                LWIP_DEBUGF(RTSP_DEBUG, ("Error, rtsp_recv: hs is NULL, close\n"));
            }
            rtsp_close_conn(pcb, ss);
            return ERR_OK;
        }

            /* Inform TCP that we have taken the data. */
            tcp_recved(pcb, p->tot_len);
            struct pbuf * buffer = p;
            while(buffer!=NULL) {
                parse_rtsp_data(ss->parser_handle, buffer->payload, buffer->len);
                buffer=buffer->next;
            }
            pbuf_free(p);

        return ERR_OK;
    }

static err_t rtsp_accept(void *arg, struct tcp_pcb *pcb, err_t err){
    LWIP_UNUSED_ARG(err);
    LWIP_UNUSED_ARG(arg);
    LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_accept %p / %p\n", (void *)pcb, arg));

    if ((err != ERR_OK) || (pcb == NULL)) {
        return ERR_VAL;
    }

    /* Set priority */
    tcp_setprio(pcb, RTSP_TCP_PRIO);

    /* Allocate memory for the structure that holds the state of the
       connection - initialized by that function. */
    rtsp_server_state_t * ss = allocate_endpoint();
    if (ss == NULL) {
        LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_accept: Out of memory, RST\n"));
        return ERR_MEM;
    }
    ss->pcb = pcb;

    /* Tell TCP that this is the structure we wish to be passed for our
       callbacks. */
    tcp_arg(pcb, ss);
    rtsp_parser_register_arg(ss->parser_handle,ss);

    /* Set up the various callback functions */

    register_rtsp_response_header_callback(ss->parser_handle,rtsp_response_header);
    register_rtsp_response_body_callback(ss->parser_handle,rtsp_body);
    register_rtsp_request_header_callback(ss->parser_handle,rtsp_request_header);
    register_rtsp_request_body_callback(ss->parser_handle,rtsp_body);

    tcp_recv(pcb, rtsp_recv);
    tcp_err(pcb, rtsp_err);
    tcp_poll(pcb, rtsp_poll, RTSP_POLL_INTERVAL);
    tcp_sent(pcb, rtsp_sent);

    return ERR_OK;
}

static void
rtsp_init_pcb(struct tcp_pcb *pcb, u16_t port)
{
    err_t err;

    if (pcb) {
        tcp_setprio(pcb, RTSP_TCP_PRIO);
        /* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
        err = tcp_bind(pcb, IP_ADDR_ANY, port);
        LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
        LWIP_ASSERT("rtsp_init: tcp_bind failed", err == ERR_OK);
        pcb = tcp_listen(pcb);
        LWIP_ASSERT("rtsp_init: tcp_listen failed", pcb != NULL);
        tcp_accept(pcb, rtsp_accept);
    }
}

void
rtsp_init(void)
{
    struct tcp_pcb *pcb;

    LWIP_DEBUGF(RTSP_DEBUG, ("rtsp_init\n"));

    pcb = tcp_new();
    LWIP_ASSERT("rtsp_init: tcp_new failed", pcb != NULL);
    rtsp_init_pcb(pcb, RTSP_SERVER_PORT);
}