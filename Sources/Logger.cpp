#include "../Includes/Logger.h"

/* Copyright (c) 2020 [Rick de Bondt] - Logger.cpp */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

Logger::~Logger()
{
    if (mLogOutputStream.is_open()) {
        mLogOutputStream.close();
    }
}

void Logger::Init(Level aLevel, bool aLogToDisk, const std::string& aFileName = "")
{
    SetLogLevel(aLevel);
    SetFileName(aFileName);
    SetLogToDisk(aLogToDisk);
}

void Logger::SetFileName(const std::string& aFileName)
{
    mFileName = aFileName;
}

Logger::Level Logger::GetLogLevel()
{
    return mLogLevel;
}

void Logger::SetLogLevel(Level aLevel)
{
    mLogLevel = aLevel;
}

void Logger::SetLogToDisk(bool aLoggingToDiskEnabled)
{
    if (aLoggingToDiskEnabled && !mLogOutputStream.is_open() && !mFileName.empty()) {
        mLogOutputStream.open(mFileName);
        if (mLogOutputStream.fail()) {
            std::cerr << "Opening log file failed, logging may be incomplete! " << mFileName << std::endl;
        }
    } else if (!aLoggingToDiskEnabled && mLogOutputStream.is_open()) {
        mLogOutputStream.close();
    }

    mLogToDisk = aLoggingToDiskEnabled;
}

void Logger::SetLogToScreen(bool aLoggingToScreenEnabled)
{
    mLogToScreen = aLoggingToScreenEnabled;
}

#if defined(__GNUC__) || defined(__GNUG__)
void Logger::Log(const std::string& aText, Level aLevel, const std::experimental::source_location& aLocation)
#else
void Logger::Log(const std::string& aText, Level aLevel)
#endif
{
    std::stringstream lLogEntry;

    if (aLevel >= mLogLevel) {
        auto lTime        = std::chrono::system_clock::now();
        auto lTimeAsTimeT = std::chrono::system_clock::to_time_t(lTime);
        auto lTimeMs      = std::chrono::duration_cast<std::chrono::milliseconds>(lTime.time_since_epoch()) % 1000;

#if defined(__GNUC__) || defined(__GNUG__)
        lLogEntry << std::put_time(std::gmtime(&lTimeAsTimeT), "%H:%M:%S:") << std::setfill('0') << std::setw(3)
                  << lTimeMs.count() << ": " << mLogLevelTexts.at(static_cast<unsigned long>(aLevel)) << ": "
                  << aLocation.file_name() << ":" << aLocation.line() << ":" << aText;
#else
        lLogEntry << std::put_time(std::gmtime(&lTimeAsTimeT), "%H:%M:%S:") << std::setfill('0') << std::setw(3)
                  << lTimeMs.count() << ": " << mLogLevelTexts.at(static_cast<unsigned long>(aLevel)) << ":" << aText;
#endif

        if (mLogToScreen)
        {
            std::cout << lLogEntry.str() << std::endl;
        }

        // Save message to log file
        if (mLogToDisk && mLogOutputStream.is_open()) {
            mLogOutputStream << lLogEntry.str() << std::endl;
        }
    }
}
