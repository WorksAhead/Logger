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

boost::signals2::connection
QCOSLogger::DownloadLogFiles(const LogDate& fromDate,
                             const LogDate& toDate,
       const DownloadSignalType::slot_type& subscriber)
{
    return m_Downloader.Download(fromDate, toDate, subscriber);
}