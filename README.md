# easylog
easylogging++  with cpp

easylogging++的使用。
新增 LOG(UDP) 作为网络日志的级别。
在NetworkLogDispatchCallback中筛选，当level是UDP的时候，发送UDP消息。
对easylogging++的文件 有少量修复。