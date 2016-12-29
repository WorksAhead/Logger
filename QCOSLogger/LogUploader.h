#pragma once

#include "LogThread.h"
#include "UploaderBase.h"

#include <boost/container/deque.hpp>
#include <boost/signals2.hpp>

namespace QCOS
{
    class LogUploader : public LogThread
    {
        typedef boost::signals2::signal<void(const LogFile&, bool result)> SignalType;

    public:
        LogUploader(UploaderBase::UploaderType uploaderType);
        ~LogUploader();

        virtual void operator()();

        // Upload result callback register.
        boost::signals2::connection RegisterUploadCallback(const SignalType::slot_type& subscriber);

        // Post log file to uploader thread for uploading.
        void UploadLogFile(const LogFile& logFile);

    private:
        void SyncFiles();
        
    private:
        UploaderBase* m_Uploader{ nullptr };
        SignalType m_UploadSignal;

        boost::container::deque<LogFile> m_LogFileQueue;
        boost::container::deque<LogFile> m_UploadFileQueue;
    };
}
