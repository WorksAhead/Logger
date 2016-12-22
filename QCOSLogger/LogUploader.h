#pragma once

#include "UploaderBase.h"

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>

namespace QCOS
{
	class LogUploader
	{
	public:
		LogUploader(UploaderBase::UploaderType uploader = UploaderBase::UPLOADER_TYPE_COS);
		~LogUploader();

		void UploadLogFile(const LogFile& logFile);

		void operator()();

	private:
		void SyncFiles();

	private:
		bool m_Stop {false};
		
		boost::mutex m_Mutex;
		boost::thread* m_Thread;
		
		boost::container::deque<LogFile> m_LogFileQueue;
		boost::container::deque<LogFile> m_UploadFileQueue;

		UploaderBase* m_Uploader;
	};
}