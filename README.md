# easylog
easylogging++  with cpp

easylogging++的常规使用。
新增 LOG(UDP) 作为网络日志的级别。
在NetworkLogDispatchCallback中筛选，当level是UDP的时候，发送UDP消息。
对easylogging++的文件 有轻量修改。

新增进入函数体内的步入步出日志。LOG_FUNCTION(); // 记录函数的进入和离开
新增LOG_PRINT() 符合printf的打印风格。
新增UDP类型的日志级别（INFO， ERROR， UDP， ...），可以用于在回调中通过UDP或者其他网络方式，实现网络日志。
性能监控的bug修复，解决性能跟踪编译不过的问题。TIMED_FUNC，TIMED_SCOPE，TIMED_BLOCK


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

轻量修改了easylogging++的源文件
