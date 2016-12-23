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
