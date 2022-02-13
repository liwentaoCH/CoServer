#include "src/http/http_server.h"
#include "src/log.h"
#include "src/mysql_pool.h"
#include "src/mutex.h"

#include <sys/stat.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <time.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
static const char* judge = "/disk2/WebServer/sylar/root/judge.html";
static const char* register_html = "/disk2/WebServer/sylar/root/register.html";
static const char* log_html = "/disk2/WebServer/sylar/root/log.html";
static const char* registerError_html = "/disk2/WebServer/sylar/root/registerError.html";
static const char* welcome_html = "/disk2/WebServer/sylar/root/welcome.html";
static const char* logError_html = "/disk2/WebServer/sylar/root/logError.html";
static const char* picture1 = "/disk2/WebServer/sylar/root/xxx.jpg";
static const char* video1 = "/disk2/WebServer/sylar/root/xxx.mp4";

static std::unordered_map<std::string, std::string> mp;
static sylar::RWMutex mut;

int32_t setFile(const char* ch, sylar::http::HttpResponse::ptr rsp) {
    struct stat m_file_stat;
    if (stat(ch, &m_file_stat) < 0) {
        SYLAR_LOG_ERROR(g_logger) << "No Recourse";
        return 1;
    }
    else if (!(m_file_stat.st_mode & S_IROTH)){
        SYLAR_LOG_ERROR(g_logger) << "Recourse Cannot Read";
        return 2;
    }
    else if (S_ISDIR(m_file_stat.st_mode)) {
        SYLAR_LOG_ERROR(g_logger) << "Bad Recourse";
        return 3;
    }
    else {
        int fd = open(ch, O_RDONLY);
        rsp->setFile((char*)mmap(0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
        rsp->setFileLen(m_file_stat.st_size);
    }
    return 0;
}

bool hasUser(std::string& user) {
    mut.rdlock();
    auto it = mp.find(user);
    mut.unlock();
    if(it != mp.end()) return true;
    else {
        std::vector<std::vector<std::string>> res;
        std::stringstream ss;
        ss << "select name from users where name = '" << user.c_str() << "'";
        if(!sylar::Mysql_pool::GetInstance()->Query(ss.str().c_str(), res) || !res.size()){
            return false;
        }
        else {
            return true;
        }
    }
}

bool hasCookie(sylar::http::HttpRequest::ptr req, std::string user) {
    std::string last = req->getCookie("user");
    if(user != last) return false;
    if(!user.size()) return false;
    mut.rdlock();
    auto it = mp.find(user);
    mut.unlock();
    return it == mp.end() ? false : true;
}

bool hasCookie(sylar::http::HttpRequest::ptr req) {
    std::string user = req->getCookie("user");
    if(!user.size()) return false;
    mut.rdlock();
    auto it = mp.find(user);
    mut.unlock();
    return it == mp.end() ? false : true;
}

bool checkUser(std::string& user, std::string& password) {
    auto it = mp.find(user);
    if(it == mp.end()) {
        std::stringstream ss;
        ss << "select password from users where name = '" << user.c_str() << "'";
        std::vector<std::vector<std::string>> res;
        if(!sylar::Mysql_pool::GetInstance()->Query(ss.str().c_str(), res) || !res.size()) {
            return false;
        }
        else {
            return res[0][0] == password ? true : false;
        }
    }
    else {
        if(mp[user] == password)
            return true;
        else 
            return false;
    }
}

int32_t func_1(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);
    rsp->setStatus((sylar::http::HttpStatus)200);
    return setFile(judge, rsp); 
}

int32_t func_2(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);
    rsp->setStatus((sylar::http::HttpStatus)200);
    return setFile(register_html, rsp); 
}

int32_t func_3(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);
    rsp->setStatus((sylar::http::HttpStatus)200);
    return setFile(log_html, rsp); 
}

int32_t func_4(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);

    std::string user = req->getParam("user");
    std::string password = req->getParam("password");
    if(hasUser(user) || !password.size() || !user.size()) {
        return setFile(registerError_html, rsp);
    }
    else {
        std::stringstream ss;
        ss << "insert into users(name, password) values('" << user.c_str() << "'"<<",'" << password.c_str() << "')";
        if(!sylar::Mysql_pool::GetInstance()->Insert(ss.str().c_str())) {
            SYLAR_LOG_ERROR(g_logger) << "error: insert into users";
        }
        mut.wrlock();
        mp.insert({user, password});
        mut.unlock();
        rsp->setCookie("user", user, time(NULL) + 60 * 60 * 24, "/", "192.168.138.148", false);
    }
    return setFile(welcome_html, rsp);
}

int32_t func_5(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);

    std::string user = req->getParam("user");
    std::string password = req->getParam("password");
    if(hasCookie(req, user)) {
        return setFile(welcome_html, rsp);
    }
    if(user.size() && password.size() && checkUser(user, password)){
        mut.wrlock();
        mp.insert({user, password});
        mut.unlock();
        rsp->setCookie("user", user, time(NULL) + 60 * 60 * 24, "/", "192.168.138.148", false);
        return setFile(welcome_html, rsp);
    }
    else {
        return setFile(logError_html, rsp);
    }
}

int32_t func_6(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);
    if(hasCookie(req)) {
        return setFile(picture1, rsp); 
    }
    else {
        return setFile(log_html, rsp);
    }
}

int32_t func_7(sylar::http::HttpRequest::ptr req
                ,sylar::http::HttpResponse::ptr rsp
                ,sylar::http::HttpSession::ptr session) {
    rsp->setVersion(17);
    if(hasCookie(req)) {
        rsp->setHeader("Content-Type", "video/mp4");
        return setFile(video1, rsp);
    }
    else {
        return setFile(log_html, rsp);
    } 
}

void run() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while(!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    
    sd->addServlet("/sylar", func_1);
    sd->addServlet("/sylar/register", func_2);
    sd->addServlet("/sylar/log", func_3);
    sd->addServlet("/check_register", func_4);
    sd->addServlet("/check_log", func_5); 
    sd->addServlet("/picture/1", func_6);
    sd->addServlet("/video/1", func_7);

    sd->addGlobServlet("/*", func_1);

    server->start();
}

int main(int argc, char** argv){
    sylar::IOManager iom(1);
    //std::cout << "iom.m_ThreadCount: " <<iom.m_threadCount << std::endl;
    iom.schedule(run);
    return 0;
}