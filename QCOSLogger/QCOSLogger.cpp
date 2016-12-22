#include "QCOSLogger.h"

using namespace QCOS;

QCOSLogger::QCOSLogger(const std::string logRootDir, UploaderBase::UploaderType uploaderType, int interval)
	: m_Uploader { uploaderType }
	, m_Recorder { interval, m_Uploader, logRootDir }
{

}

void
QCOSLogger::WriteLog(const std::string& text)
{
	m_Recorder.WriteLog(text);
}
