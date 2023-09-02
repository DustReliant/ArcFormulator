
#ifndef LOG_H
#define LOG_H




// 日志级别枚举
enum LOGLEVEL 
{
	LOG_LEVEL_ALL = 0,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR
};

// 日志输出位置
enum LOGTARGET 
{
	LOG_TARGET_NONE	   = 0x00,
	LOG_TARGET_CONSOLE = 0x01,
	LOG_TARGET_FILE	   = 0x10
};


class Log
{
public:
	virtual int LogInit() = 0;
};


// 定义日志宏
#define LOG(level, message) LogManager::instance().log(level, message, __FILE__, __LINE__)


#define LEVEL_DEBUG						// 调试
#define LEVEL_INFO 						// 显示
#define LEVEL_WARNING					// 警告
#define LEVEL_ERROR						// 错误

#endif // !LOG_H