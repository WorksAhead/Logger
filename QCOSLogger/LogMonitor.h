#pragma once

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem.hpp>

namespace QCOS
{
    class LogFile;
    class LogUploader;
    class LogMonitor
    {
    public:
        // Scan file interval in seconds
        LogMonitor(const std::string& logRootDir, int interval, LogUploader& uploader);
        ~LogMonitor();

        void operator()();

        //private:
        void ScanFiles();

        void UploadLogFiles();

        //
        void UploadCallback(const LogFile& logFile, bool result);

    private:
        bool m_Stop{ false };

        boost::mutex m_Mutex;
        boost::thread* m_Thread;

        LogUploader& m_Uploader;

        std::string m_LogRootDir{ "./" };
        int m_Interval{ 6 };

        boost::container::deque<LogFile> m_LogFileQueue;
    };
}