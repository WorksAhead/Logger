#pragma once

#include "LogThread.h"

#include <boost/container/deque.hpp>
#include <boost/signals2.hpp>

namespace QCOS
{
    class LogFile;
    class LogUploader;
    class LogMonitor : public LogThread
    {
    public:
        // Scan file interval in seconds
        LogMonitor(const std::string& monitorDir, int scanInterval, LogUploader& uploader);
        ~LogMonitor();

        virtual void operator()();

    private:
        void ScanFiles();
        void UploadLogFiles();

        void UploadCallback(const LogFile& logFile, bool result);

    private:
        // Log file path for monitor.
        std::string m_MonitorDir;
        // Monitor scan interval, in seconds.
        int m_ScanInterval{ 6 };

        // Log file uploader.
        LogUploader& m_Uploader;

        // Log files that is waiting for upload.
        boost::container::deque<LogFile> m_LogFileQueue;

        // Upload callback signal connection.
        boost::signals2::connection m_Connection;
    };
}