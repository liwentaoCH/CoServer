#include "sylar.h"
#include <assert.h>

//  ！（x）  输出是1
#define SYLAR_ASSERT3(x) \
    if(SYLAR_UNLIKELY(!(x))) { \
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << sylar::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_assert() {
     SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10);
     SYLAR_ASSERT3(false);
}

int main(int argc, char** argv) {
     test_assert();
     //assert(1 == 1);
     return 0;
}