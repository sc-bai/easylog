#pragma once
#include <io.h>
/*
#define EASYLOGGING
#define ELPP_UNICODE
#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_STL_LOGGING
#define ELPP_THREAD_SAFE

#define ELPP_FEATURE_PERFORMANCE_TRACKING
*/
#include "easylogging++.h"


#define MAX_lOG_FILE_SIZE		10485760	// 10m
#define LOG_TO_STANDARD_OUTPUT	"false"		// true为打印
#define APP_NAME				"easylog"


// 定义一个日志记录器类，用于记录函数的进入和离开
class FunctionLogger {
public:
	FunctionLogger(const char* function) : m_function(function) {
		LOG(INFO) << "----------->Entering function:" << m_function;
	}
	~FunctionLogger() {
		LOG(INFO) << "<-----------Leaving function:" << m_function;
	}

private:
	const char* m_function; 
};

// 使用宏简化FunctionLogger的使用
#define LOG_FUNCTION() FunctionLogger _logger(__FUNCTION__)

// log_print

#define LOG_PRINT(level, format, ...) \
    do { \
        char logBuffer[4096] = {0}; \
        std::snprintf(logBuffer, sizeof(logBuffer), format, ##__VA_ARGS__); \
        LOG(level) << logBuffer; \
    } while(0)

#define LOG_PRINT_N(format, ...) \
    do { \
        char logBuffer[4096] = {0}; \
        std::snprintf(logBuffer, sizeof(logBuffer), format, ##__VA_ARGS__); \
        LOG(INFO)<< logBuffer;\
    } while(0)

#define LOG_PRINT_E(format, ...) \
    do { \
        char logBuffer[4096] = {0}; \
        std::snprintf(logBuffer, sizeof(logBuffer), format, ##__VA_ARGS__); \
        LOG(ERROR)<< logBuffer;\
    } while(0)

