#include "bytearray.h"
#include "sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test() {
#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        SYLAR_ASSERT(v == vec[i]); \
    } \
    SYLAR_ASSERT(ba->getReadSize() == 0); \
    SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
}

    XX(int8_t,  100, writeFint8, readFint8, 100);
    XX(uint8_t, 100, writeFuint8, readFuint8, 100);
    XX(int16_t,  100, writeFint16,  readFint16, 100);
    XX(uint16_t, 100, writeFuint16, readFuint16, 100);
    XX(int32_t,  100, writeFint32,  readFint32, 100);
    XX(uint32_t, 100, writeFuint32, readFuint32, 100);
    XX(int64_t,  100, writeFint64,  readFint64, 100);
    XX(uint64_t, 100, writeFuint64, readFuint64, 100);

    XX(int32_t,  100, writeInt32,  readInt32, 100);
    XX(uint32_t, 100, writeUint32, readUint32, 100);
    XX(int64_t,  100, writeInt64,  readInt64, 100);
    XX(uint64_t, 100, writeUint64, readUint64, 100);
#undef XX

#define XX(type, len, write_fun, read_fun, base_len) {\
    std::vector<type> vec; \
    for(int i = 0; i < len; ++i) { \
        vec.push_back(rand()); \
    } \
    sylar::ByteArray::ptr ba(new sylar::ByteArray(base_len)); \
    for(auto& i : vec) { \
        ba->write_fun(i); \
    } \
    ba->setPosition(0); \
    for(size_t i = 0; i < vec.size(); ++i) { \
        type v = ba->read_fun(); \
        SYLAR_ASSERT(v == vec[i]); \
    } \
    SYLAR_ASSERT(ba->getReadSize() == 0); \
    SYLAR_LOG_INFO(g_logger) << #write_fun "/" #read_fun \
                    " (" #type " ) len=" << len \
                    << " base_len=" << base_len \
                    << " size=" << ba->getSize(); \
    ba->setPosition(0); \
    SYLAR_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    sylar::ByteArray::ptr ba2(new sylar::ByteArray(base_len * 2)); \
    SYLAR_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")); \
    ba2->setPosition(0); \
    SYLAR_ASSERT(ba->toString() == ba2->toString()); \
    SYLAR_ASSERT(ba->getPosition() == 0); \
    SYLAR_ASSERT(ba2->getPosition() == 0); \
}
    XX(int8_t,  100, writeFint8, readFint8, 100);
    XX(uint8_t, 100, writeFuint8, readFuint8, 100);
    XX(int16_t,  100, writeFint16,  readFint16, 100);
    XX(uint16_t, 100, writeFuint16, readFuint16, 100);
    XX(int32_t,  100, writeFint32,  readFint32, 100);
    XX(uint32_t, 100, writeFuint32, readFuint32, 100);
    XX(int64_t,  100, writeFint64,  readFint64, 100);
    XX(uint64_t, 100, writeFuint64, readFuint64, 100);

    XX(int32_t,  100, writeInt32,  readInt32, 100);
    XX(uint32_t, 100, writeUint32, readUint32, 100);
    XX(int64_t,  100, writeInt64,  readInt64, 100);
    XX(uint64_t, 100, writeUint64, readUint64, 100);

#undef XX
}

// 验证 函数 writeToFile
void test1() {
    uint8_t num = 3;
    sylar::ByteArray::ptr ba(new sylar::ByteArray(50));
    ba->clear();
    for(int i = 0; i < 112; i++)
        ba->writeFint8(num);
    SYLAR_LOG_INFO(g_logger) << "ba的size: " << ba->getSize();
    ba->setPosition(90);
    SYLAR_LOG_INFO(g_logger) << "position: " << ba->getPosition();
    std::string name("/tmp/test_bytearray.dat");
    SYLAR_ASSERT(ba->writeToFile(name));
}

int main(int argc, char** argv) {
    test1();
    return 0;
}
