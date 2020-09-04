#ifndef LOGGER_H
#define LOGGER_H

/* Copyright (c) 2020 [Rick de Bondt] - Logger.h
 *
 * This file contains the header for a Logger class which will be used to log things happening in the applications based
 * on different levels.
 *
 * */

#include <array>
#include <fstream>

// Does not exist in Visual Studio yet, https://github.com/microsoft/STL/pull/664
#if defined(__GNUC__) || defined(__GNUG__)
#include <experimental/source_location>
#endif

#include <sstream>
#include <string>


class Logger
{
public:
    Logger(const Logger &aLogger) = delete;
    Logger &operator=(const Logger &aLogger) = delete;

    enum Level
    {
        TRACE = 0,
        DEBUG,
        INFO,
        WARNING,
        // ERROR is already defined in windows
        ERR
    };

    /**
     * Gets the Logger singleton.
     * @return The Logger object.
     */
    static Logger &GetInstance()
    {
        static Logger lInstance;
        return lInstance;
    }

    /**
     * Initializes the logger singleton.
     * @param aLevel - Loglevel to set the logger to.
     * @param aLogToDisk - Whether we should log to disk or not.
     * @param aFileName - Filename to use for logging.
     */
    void Init(Level aLevel, bool aLogToDisk, const std::string &aFileName);

    /**
     * Logs given text to file.
     * @param aText - Text to be logged.
     * @param aLevel - Loglevel to use.
     * @param aLocation - Source location (keep empty).
     */
#if defined(__GNUC__) || defined(__GNUG__)
    void Log(const std::string &aText, Level aLevel, const std::experimental::source_location &aLocation =
    std::experimental::source_location::current());
#else
    void Log(const std::string& aText, Level aLevel);
#endif

    /**
     * Sets the loglevel to debug, warning or error, based upon this variable certain logmessages will be shown or not
     * shown.
     * @param aLevel - The level to set.
     */
    void SetLogLevel(Level aLevel);

    /**
     * Sets the filename to log to when logging to disk is enabled.
     * @param aFileName - Filename to save the log to.
     */
    void SetFileName(const std::string &aFileName);

    /**
     * Enables or disables logging to disk.
     * @param aLoggingToDiskEnabled - Set true when logging to disk should be enabled.
     */
    void SetLogToDisk(bool aLoggingToDiskEnabled);

private:
    Logger() = default;
    ~Logger();

    std::string mFileName{"log.txt"};
    Level mLogLevel{Level::ERR};
    std::array<std::string, 5> mLogLevelTexts{"TRACE", "DEBUG", "INFO", "WARNING", "ERROR"};
    std::ofstream mLogOutputStream{};
    bool mLogToDisk{false};
};


#endif //LOGGER_H