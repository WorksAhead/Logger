#pragma once

#include <string>

#include "LogRecorder.h"
#include "LogUploader.h"

namespace QCOS
{
	class QCOSLogger
	{
	public:
		// Log record to disk interval, in minute.
		QCOSLogger(const std::string logRootDir = "./LogFiles", UploaderBase::UploaderType uploaderType = UploaderBase::UPLOADER_TYPE_COS, int interval = 1);

		void WriteLog(const std::string& text);

	private:
		LogUploader m_Uploader;
		LogRecorder m_Recorder;
		
	};
}
