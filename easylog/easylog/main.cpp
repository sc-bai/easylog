﻿// easylog.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <filesystem>
#include <io.h>
#include <chrono>

#include "easylog.hpp"
INITIALIZE_EASYLOGGINGPP

static std::string get_current_time() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm* now_tm = std::localtime(&now_time_t);
	std::stringstream ss;
	ss << std::put_time(now_tm, "%Y%m%d");
	ss << std::put_time(now_tm, "%H%M%S");
	return ss.str();
}

static std::wstring get_current_date() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm* now_tm = std::localtime(&now_time_t);
	std::wstringstream ss;
	ss << std::put_time(now_tm, L"%Y%m%d");
	return ss.str();
}

namespace fs = std::filesystem;
using namespace std::chrono;

/*
	删除文件路径下n天前的日志文件，由于删除日志文件导致的空文件夹会在下一次删除
	isRoot为true时，只会清理空的子文件夹
*/
void DeleteOldFiles(const std::string& path, int oldDays, bool isRoot) {
	auto now = file_clock::now();
	auto oldTime = now - file_clock::duration(oldDays * 24h);

	// 遍历目录
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory()) {
			// 如果是文件夹，递归调用 DeleteOldFiles
			DeleteOldFiles(entry.path().string(), oldDays, false);
			// 如果是根目录，并且文件夹为空，则删除空文件夹
			if (isRoot && fs::is_empty(entry.path())) {
				fs::remove(entry.path());
			}
		}
		else {
			// 如果是文件，获取最后修改时间
			auto fileTime = fs::last_write_time(entry.path());
			if (fileTime < oldTime) {
				fs::remove(entry.path());
			}
		}
	}
}

// 定义一个网络日志派发类
class NetworkLogDispatchCallback : public el::LogDispatchCallback {
public:
	void handle(const el::LogDispatchData* data) noexcept override {
		
		//std::cout << "Sending to network: type->" << (int)data->logMessage()->level() << ",data->" << data->logMessage()->message() << std::endl;
		if (data->logMessage()->logger()->id() == "error") {
			return;
		}
		// 将日志数据发送到网络的逻辑
		if (data->logMessage()->level() == el::Level::UDP) {
			// udp 
			return;
		}
		// 单独写入error文件
		else if (data->logMessage()->level() == el::Level::Error) {
			CLOG(ERROR, "error") << data->logMessage()->message();
		}
	}
};

void initLog(int argc, char* argv[]) {

	el::Helpers::installLogDispatchCallback<NetworkLogDispatchCallback>("NetworkLogDispatch");
	{
		START_EASYLOGGINGPP(argc, argv);

		wchar_t buffer[MAX_PATH] = {0};
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring exePathW(buffer);
		std::filesystem::path exePath(exePathW);
		std::filesystem::path logPath = exePath.parent_path() / L"log" ;
		std::filesystem::path logDatePath = logPath / get_current_date();
		
		if (_access(logDatePath.string().c_str(), 0) != 0) {
			std::filesystem::create_directories(logDatePath);
		}
		/*
		std::filesystem::path logDatePath = logPath.parent_path() / get_current_date();
		if (_access((logDatePath.string()).c_str(), 0) != 0) {
			std::filesystem::create_directory(logDatePath);
		}
		*/
		DeleteOldFiles(logPath.string(), 30, false); // 删除前30天的数据
		std::string strCurrentTime = get_current_time();
		std::string logInfoFile = std::format("{}/{}_info_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);
		std::string logDebugFile = std::format("{}/{}_debug_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);
		std::string logWarningFile = std::format("{}/{}_warning_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);
		std::string logErrorFile = std::format("{}/{}_error_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);
		std::string logFatalFile = std::format("{}/{}_fatal_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);
		std::string logUDPFile = std::format("{}/{}_udp_{}.log", logDatePath.string(), APP_NAME, strCurrentTime);

		el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog); // 打开的话 LOG(FATAL)不会中断程序
		el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

		el::Configurations defaultConf;
		defaultConf.setToDefault();
		defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, std::string(LOG_TO_STANDARD_OUTPUT));
		defaultConf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, std::to_string(MAX_lOG_FILE_SIZE));

		// 该方式是写入到一个文件中。
		//defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, logFile);
		//el::Loggers::reconfigureLogger("default", defaultConf);
		//el::Loggers::reconfigureAllLoggers(defaultConf);

		// 不同的级别写入不同文件
		/*
		defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Error, el::ConfigurationType::Filename, logErrorFile);
		defaultConf.set(el::Level::Debug, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Info, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Warning, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Error, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Fatal, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::UDP, el::ConfigurationType::Filename, logInfoFile); // network
		el::Loggers::reconfigureLogger("default", defaultConf);
		*/
		
		// 设置全局日志文件
		defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, logInfoFile);
		defaultConf.set(el::Level::Error, el::ConfigurationType::Filename, logInfoFile); // 写入全局日志文件
		//el::Loggers::reconfigureAllLoggers(defaultConf);


		// 配置错误日志记录器，用于记录 ERROR 级别的日志
		el::Configurations errorConf;
		//errorConf.setToDefault(el::Level::Error);
		errorConf.set(el::Level::Error, el::ConfigurationType::Enabled, "true");
		errorConf.set(el::Level::Error, el::ConfigurationType::Filename, logErrorFile);
		errorConf.set(el::Level::Error, el::ConfigurationType::ToStandardOutput, std::string(LOG_TO_STANDARD_OUTPUT));
		errorConf.set(el::Level::Error, el::ConfigurationType::MaxLogFileSize, std::to_string(MAX_lOG_FILE_SIZE));
		//el::Loggers::reconfigureLogger("error", errorConf);

		// 应用通用配置
		el::Loggers::reconfigureLogger("default", defaultConf);
		el::Loggers::reconfigureLogger("error", errorConf);
	}

	LOG(INFO) << "*****************************************";
	LOG(INFO) << "app start.";
	LOG(ERROR) << "*****************************************";
	LOG(ERROR) << "app start.";

	LOG(INFO) << "你好";
	LOG(INFO) << std::string("通用！").c_str();

	/*
	LOG(DEBUG) << "*****************************************";
	LOG(DEBUG) << "app start.";
	LOG(WARNING) << "*****************************************";
	LOG(WARNING) << "app start.";
	LOG(ERROR) << "*****************************************";
	LOG(ERROR) << "app start.";
	LOG(FATAL) << "*****************************************";
	LOG(FATAL) << "app start.";
	*/
}

/*
	性能追踪
*/
void PerformanceTest()
{
	LOG_FUNCTION(); // 记录函数的进入和离开
	// TIMED_xxx 默认INFO级别

	// TIMED_FUNC会统计其后续所有代码的执行时间
	TIMED_FUNC(timerFunObj);
	Sleep(100);

	// TIMED_SCOPE会统计其后续所有代码的执行时间
	TIMED_SCOPE(timerScopeObj, "TIMED_SCOPE_Test");
	Sleep(100);

	// TIMED_BLOCK只会统计花括号{}里所有代码的执行时间
	TIMED_BLOCK(timerBlockObj, "TIMED_BLOCK_Test")
	{
		Sleep(100);
	}

	Sleep(100);
}
#include <string>
int main(int argc, char* argv[])
{
	// initLog(argc, argv);

	initLog(0, nullptr);
	LOG_FUNCTION(); // 记录函数的进入和离开

	// 正常写入
	LOG(INFO) << " info log.";
	LOG(WARNING) << " warning log.";
	LOG(DEBUG) << " debug log.";
	LOG(ERROR) << " error log.";
	// LOG(FATAL) << "fatal log."; // 会中断程序
	LOG(UDP) << " udp log.";

	// printf format
	LOG_PRINT_N(" info log with printf format.");
	LOG_PRINT_E(" info log with printf format.%d:", 666);
	LOG_PRINT(INFO, "this is a info test.%s", "info messing.");
	LOG_PRINT(ERROR, "this is a err test.%s", "error messing.");

	// 判断写入
	int a = 0, b = 1;
	LOG_IF(a < b, INFO) << "a<b"; // 输出 "a<b"
	LOG_IF(a > b, INFO) << "a>b"; // 不输出 "a<b"

	// 性能追踪
	PerformanceTest();
    std::cout << "Hello World!\n";
	return 0;
}
