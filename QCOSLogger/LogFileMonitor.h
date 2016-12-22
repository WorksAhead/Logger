#pragma once

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem.hpp>

namespace QCOS
{
	class LogFile;
	class LogUploader;
	class LogFileMonitor
	{
	public:
		// Scan file interval
		LogFileMonitor(const std::string& logRootDir, int interval, LogUploader& uploader);
		~LogFileMonitor() {};

		void operator()();

	//private:
		void ScanFiles();

		void UploadLogFiles();
	private:
		// file type 0 is folders and files
		// file type 1 is only folders
		// file type 2 is only files
		void GetAllFiles(const boost::filesystem::path& destPath,
						 boost::container::deque<std::string>& outQueue,
						 int fileType);

	private:
		bool m_Stop{ false };

		boost::mutex m_Mutex;
		boost::thread* m_Thread;

		LogUploader& m_Uploader;

		std::string m_LogRootDir{ "./" };
		int m_Interval{ 6 };

		boost::container::deque<std::string> m_DayFolderQueue;
		boost::container::deque<std::string> m_HourFolderQueue;
		boost::container::deque<std::string> m_LogFileQueue;

		boost::container::deque<LogFile> m_MyLogFileQueue;
	};
}