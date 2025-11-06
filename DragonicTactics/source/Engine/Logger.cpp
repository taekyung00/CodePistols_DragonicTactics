/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Taekyung Ho
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Logger.hpp"
#include <iostream>

namespace CS230
{
    Logger::Logger(Severity severity, bool use_console, std::chrono::system_clock::time_point _start_time) : min_level(severity), out_stream("Trace.log"), start_time(_start_time)
    {
        if (use_console == true)
        {
            out_stream.basic_ios<char>::rdbuf(std::cout.rdbuf());
            ;
        }
    }

    void Logger::LogError(std::string text)
    {
        log(Severity::Error, text);
    }

    void Logger::LogEvent(std::string text)
    {
        log(Severity::Event, text);
    }

    void Logger::LogDebug(std::string text)
    {
        log(Severity::Debug, text);
    }

    void Logger::LogVerbose(std::string text)
    {
        log(Severity::Verbose, text);
    }

    void Logger::log(Severity severity, std::string message)
    {
        std::map<CS230::Logger::Severity, std::string> get_error_level = {
            { CS230::Logger::Severity::Verbose, "Verbose" },
            {   CS230::Logger::Severity::Debug,   "Debug" },
            {   CS230::Logger::Severity::Event,   "Event" },
            {   CS230::Logger::Severity::Error,   "Error" }
        };
        std::string answer = get_error_level[severity] + "\t" + message;
        //===========map_version


        if (int(CS230::Logger::min_level) <= int(severity))
        {
            out_stream.precision(4);
            out_stream << '[' << std::fixed << seconds_since_start() << "]\t";
            out_stream << answer << "\n";
        }
        return;
    }

    double Logger::seconds_since_start()
    {
        return std::chrono::duration<double>(std::chrono::system_clock::now() -start_time).count();
    }

    // note the proper way to redirect the rdbuf is `stream.basic_ios<char>::rdbuf(other_stream.rdbuf());`
    // note that we don't need a destructor ~Logger() if all we are doing is closing the streams. The std stream classes impl Rule of 5 and will auto flush & close themselves
}
