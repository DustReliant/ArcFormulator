#include "LogManager.h"

LogManager::LogManager()
{

}

LogManager::LogManager(emLogLevel level, const std::string& logFilename) 
{

}

LogManager::~LogManager()
{

}

LogManager& LogManager::instance()
{
	LogManager logManager(emLogLevel::LOG_LEVEL_ALL, "app.log");
	return logManager;
}

bool LogManager::LogInit()
{

	return false;
}
