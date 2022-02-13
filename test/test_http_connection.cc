#include <iostream>
#include "http/http_connection.h"
#include "log.h"
#include "iomanager.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run() {
    sylar::Address::ptr = sylar::Address::LookupAnyIPAddress("www.sylar.top:80");
    sylar::Socket::ptr sock = syular::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    sylar::http::HttpConnection ptr conn(new sylar::http::HttpConnection(sock));

    sylar::http::HttpRequest::ptr req(new syalr::http::HttpRequest);
    SYLAR_LOG_INFO(g_logger) << "req: " << std::endl
        << * req;
    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    if(!rsp) {
        SYLAR_LOG_INFO(g_logger) << "recv response error"；
        return;
    }
    SYLAR_LOG_INFO(g_logger) << "rsp: " << std::endl;
        << *rsp;
}

int main() {
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}