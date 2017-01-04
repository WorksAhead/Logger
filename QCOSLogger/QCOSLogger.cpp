#include "QCOSLogger.h"

#include <boost/filesystem.hpp>

using namespace QCOS;

QCOSLogger::QCOSLogger(const std::string& logOutputDir,
                                      int logSinkInterval,
               UploaderBase::UploaderType uploaderType,
                       const std::string& downloadDir,
           DownloaderBase::DownloaderType downloaderType)
    : m_Uploader{ uploaderType }
    , m_Downloader{ downloadDir, downloaderType }
    , m_Recorder{ boost::filesystem::path(logOutputDir).generic_string(), logSinkInterval }
    , m_Monitor{ boost::filesystem::path(logOutputDir).generic_string(), logSinkInterval, m_Uploader }
{

}

void
QCOSLogger::WriteLog(const std::string& text)
{
    m_Recorder.WriteLog(text);
}

void
QCOSLogger::DownloadLogFiles(const std::string& fromTimeStr, const std::string& toTimeStr, int interval, DownloadCallback callback)
{
    m_Downloader.DownloadLogFiles(fromTimeStr, toTimeStr, interval, callback);
}