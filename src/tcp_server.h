#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include <memory>
#include <functional>
#include "iomanager.h"
#include "address.h"
#include "socket.h"
#include "noncopyable.h"

namespace coserver {

class TcpServer : public std::enable_shared_from_this<TcpServer> 
                    , Noncopyable{
public:
    typedef std::shared_ptr<TcpServer> ptr;
    TcpServer(coserver::IOManager* worker = coserver::IOManager::GetThis()
              ,coserver::IOManager* io_woker = coserver::IOManager::GetThis()
              ,coserver::IOManager* accept_worker = coserver::IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(coserver::Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr>& addr,
                        std::vector<Address::ptr>& fails);
    virtual bool start();
    virtual void stop();

    uint64_t getReadTimeout() const { return m_recvTimeout; }
    std::string getName() const { return m_name; }
    void setReadTimeout(uint64_t v) { m_recvTimeout = v; }
    virtual void setName(const std::string& v) { m_name = v;}
    virtual std::string toString(const std::string& prefix = "");

    bool isStop() const { return m_isStop; }
protected:
    virtual void handleClient(Socket::ptr client);
    virtual void startAccept(Socket::ptr sock);
private:
    std::vector<Socket::ptr> m_socks;
    IOManager* m_worker;
    IOManager* m_acceptWorker;
    IOManager* m_ioWorker;
    uint64_t m_recvTimeout;
    std::string m_name;
    bool m_isStop;

};

}




#endif