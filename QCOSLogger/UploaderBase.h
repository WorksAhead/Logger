#pragma once

#include <string>

namespace QCOS
{
	class LogFile;
	class UploaderBase
	{
	public:
		typedef enum
		{
			UPLOADER_TYPE_COS,
		} UploaderType;

		virtual ~UploaderBase() {};

		virtual void UploadLogFile(const LogFile& logFile) = 0;
	};
}