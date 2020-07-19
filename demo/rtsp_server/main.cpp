#include <iostream>
#include <cassert>
#include "rtsp_server.h"
#include <lwip_api_mock.h>

extern "C" void app() {
    rtsp_init();
}

int main() {
    Start_lwipApplication(app);
    return 0;
}
