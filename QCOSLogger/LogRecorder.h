#pragma once

#include "LogThread.h"
#include "LogFile.h"

#include <boost/filesystem.hpp>
#include <boost/container/deque.hpp>

namespace QCOS
{
    class LogRecorder : public LogThread
    {
    public:
        LogRecorder(const std::string& logOutputDir, int logSinkInterval);
        ~LogRecorder();

        virtual void operator()();

        void WriteLog(const std::string& text);

    private:
        void UpdateLogFile();
        void SinkLogFile();

    private:
        // Log file's output path.
        std::string m_OutputDir;
        // Log record's sink interval, in seconds.
        int m_SinkInterval{ 300 };

        // Current logging file.
        LogFile m_LogFile;
        // Current logging file stream.
        boost::filesystem::ofstream m_FileStream;

        // Log records that is ready to sink to log file.
        boost::container::deque<std::string> m_LogRecordQueue;
    };
}
