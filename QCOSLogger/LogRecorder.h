#pragma once
#include "LogFile.h"

#include <string>

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem/fstream.hpp>

namespace QCOS
{
    class LogRecorder
    {
    public:
        // Log record interval, in seconds.
        LogRecorder(const std::string& logRootDir, int interval);
        ~LogRecorder();

        void WriteLog(const std::string& text);

        void operator()();

    private:
        void UpdateLogFile();
        void SyncFile();

    private:
        std::string m_LogRootDir;

        bool m_Stop{ false };
        // Record interval in seconds.
        int m_Interval{ 300 };

        boost::mutex m_Mutex;
        boost::thread* m_Thread{ nullptr };

        // Current logging file.
        LogFile m_LogFile;
        // Current logginf file stream.
        boost::filesystem::ofstream m_FileStream;

        boost::container::deque<std::string> m_LogQueue;
    };
}