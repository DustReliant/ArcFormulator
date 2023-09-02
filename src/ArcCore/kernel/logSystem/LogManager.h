/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

// 日志管理器


#ifndef LOGMANGER_H
#define LOGMANGER_H

#include "log.h"
#include <string>

class LogManager : public Log
{
public:
	LogManager();
	LogManager(emLogLevel level, const std::string& logFilename);

	~LogManager();



    LogManager& instance();

protected:

	virtual bool LogInit() override;

};
#endif // LOGMANGER_H