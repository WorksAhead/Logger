#pragma once

#include <string>

#include "LogRecorder.h"
#include "LogUploader.h"
#include "LogMonitor.h"
#include "LogDownloader.h"

namespace QCOS
{
    #if defined(__linux__)
    #define DEFAULT_UPLOADER UploaderBase::UPLOADER_TYPE_COS
    #define DEFAULT_DOWNLOADER DownloaderBase::DOWNLOADER_TYPE_COS
    #else
    #define DEFAULT_UPLOADER UploaderBase::UPLOADER_TYPE_FAKE
    #define DEFAULT_DOWNLOADER DownloaderBase::DOWNLOADER_TYPE_FAKE
    #endif

    class QCOSLogger
    {
    public:
        // @logRootDir: Log files' saving path.
        // @interval: Log record to disk interval, in seconds.
        // @uploaderType: Internal uploader's type.
        QCOSLogger(const std::string& logOutputDir,
                                  int logSinkInterval,
           UploaderBase::UploaderType uploaderType,
                   const std::string& downloadDir,
       DownloaderBase::DownloaderType downloaderType);

        void WriteLog(const std::string& text);

        void DownloadLogFiles(const std::string& fromTimeStr, const std::string& toTimeStr, int interval, DownloadCallback callback);

    private:
        LogUploader   m_Uploader;
        LogDownloader m_Downloader;
        LogRecorder   m_Recorder;
        LogMonitor    m_Monitor;
    };
}
