#pragma once

#include "UploaderBase.h"

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/signals2.hpp>

namespace QCOS
{
    class LogUploader
    {
        typedef boost::signals2::signal<void(const LogFile&, bool result)> UploadSignal;

    public:
        LogUploader(UploaderBase::UploaderType type);
        ~LogUploader();

        void UploadLogFile(const LogFile& logFile);

        void RegisterUploadCallback(const boost::function<void(const LogFile&, bool result)>& callback);

        void operator()();

    private:
        void SyncFiles();
        
    private:
        bool m_Stop{ false };

        boost::mutex m_Mutex;
        boost::thread* m_Thread{ nullptr };

        boost::container::deque<LogFile> m_LogFileQueue;
        boost::container::deque<LogFile> m_UploadFileQueue;

        UploaderBase* m_Uploader{ nullptr };
        UploadSignal m_UploadSignal;
    };
}