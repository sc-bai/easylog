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
INITIALIZE_EASYLOGGINGPP

#define MAX_lOG_FILE_SIZE		10485760	// 10m
#define LOG_TO_STANDARD_OUTPUT	"false"		// trueÎª´òÓ¡
#define APP_NAME				"easylog"
