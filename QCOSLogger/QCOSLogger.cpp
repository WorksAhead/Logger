#include "QCOSLogger.h"

#include <boost/filesystem.hpp>

using namespace QCOS;

QCOSLogger::QCOSLogger(const std::string logRootDir, int interval, UploaderBase::UploaderType uploaderType)
    : m_Uploader{ uploaderType }
    , m_Recorder{ boost::filesystem::path(logRootDir).generic_string(), interval }
    , m_Monitor{ boost::filesystem::path(logRootDir).generic_string(), interval, m_Uploader }
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