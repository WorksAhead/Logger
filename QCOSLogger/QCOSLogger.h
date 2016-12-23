#pragma once

#include <string>

#include "LogRecorder.h"
#include "LogUploader.h"
#include "LogMonitor.h"

namespace QCOS
{
    #if defined(__linux__)
    #define DEFAULT_UPLOADER UploaderBase::UPLOADER_TYPE_COS
    #else
    #define DEFAULT_UPLOADER UploaderBase::UPLOADER_TYPE_FAKE
    #endif

    class QCOSLogger
    {
    public:
        // @logRootDir: Log files' saving path.
        // @interval: Log record to disk interval, in seconds.
        // @uploaderType: Internal uploader's type.
        QCOSLogger(const std::string logRootDir = "./LogFiles",
            int interval = 60,
            UploaderBase::UploaderType uploaderType = DEFAULT_UPLOADER);

        void WriteLog(const std::string& text);

    private:
        LogUploader m_Uploader;
        LogRecorder m_Recorder;
        LogMonitor m_Monitor;
    };
}
