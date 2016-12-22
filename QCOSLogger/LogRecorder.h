#pragma once
#include "LogFile.h"

#include <string>

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem/fstream.hpp>

namespace QCOS
{
	class LogUploader;
	class LogRecorder
	{
	public:
		// Log record interval, in minute.
		LogRecorder(int interval, LogUploader& uploader, const std::string& logRootDir);
		~LogRecorder();

		void WriteLog(const std::string& text);

		void operator()();

	private:
		void UpdateLogFile();

	private:
		bool m_Stop {false};
		// Record interval in minute.
		int m_Interval {5};

		LogUploader& m_Uploader;
		
		boost::mutex m_Mutex;
		boost::thread* m_Thread;
		
		boost::container::deque<std::string> m_LogQueue;

		LogFile m_LogFile;
		boost::filesystem::ofstream m_FileStream;

		std::string m_LogRootDir;
	};
}