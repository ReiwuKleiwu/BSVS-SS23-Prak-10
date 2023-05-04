#include "request.h"
#include "string.h"
#include <sys/socket.h>

void send_response(Request client_request) {
    send(client_request.client_socket, client_request.response, strlen(client_request.response), 0);
}