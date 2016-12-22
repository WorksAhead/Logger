#include "LogFileMonitor.h"
#include "LogUploader.h"
#include "LogFile.h"

#include <boost/foreach.hpp> 
#include <boost/format.hpp> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

using namespace QCOS;

LogFileMonitor::LogFileMonitor(const std::string& logRootDir, int interval, LogUploader& uploader)
	: m_LogRootDir{logRootDir}
	, m_Interval{interval}
	, m_Uploader(uploader)
{


	// Initial day folders
	ScanFiles();
	UploadLogFiles();
}

void
LogFileMonitor::ScanFiles()
{
	path rootPath(m_LogRootDir);
	recursive_directory_iterator end;
	for (recursive_directory_iterator pos(rootPath); pos != end; ++pos)
	{
		path filePath(*pos);

		if (!is_regular_file(filePath))
		{
			continue;
		}

		std::string filePathStr = filePath.generic_string();
		typedef std::vector< std::string > split_vector_type;
		split_vector_type SplitVec;
		boost::split(SplitVec, filePathStr, boost::is_any_of("/"), boost::token_compress_on);

		size_t splitSize = SplitVec.size();
		if (splitSize < 3)
		{
			continue;
		}

		LogFile logFile;
		logFile.FileName = SplitVec[splitSize - 1];
		logFile.HourFolder = SplitVec[splitSize - 2];
		logFile.DayFolder = SplitVec[splitSize - 3];
		logFile.PathName = boost::str(boost::format("%1%/%2%/%3%") % m_LogRootDir % logFile.DayFolder % logFile.HourFolder);
		logFile.FullPathName = boost::str(boost::format("%1%/%2%/%3%/%4%") % m_LogRootDir % logFile.DayFolder % logFile.HourFolder % logFile.FileName);

		if (!boost::filesystem::exists(logFile.FullPathName))
		{
			continue;
		}

		m_MyLogFileQueue.push_back(logFile);
	}
}

void
LogFileMonitor::UploadLogFiles()
{
	// always keep at least one element in the queue.
	if (m_MyLogFileQueue.size() < 2)
	{
		return;
	}

	LogFile logFile = m_MyLogFileQueue.front();
	m_MyLogFileQueue.pop_back();

	m_Uploader.UploadLogFile(logFile);
}

void
LogFileMonitor::operator()()
{
	while (!m_Stop)
	{
		ScanFiles();
	}
}

void
LogFileMonitor::GetAllFiles(const boost::filesystem::path& destPath,
	                        boost::container::deque<std::string>& outQueue,
	                        int fileType)
{
	directory_iterator end;
	for (directory_iterator pos(destPath); pos != end; ++pos)
	{
		path filePath(*pos);

		// files and folders.
		if (fileType == 0)
		{
			outQueue.push_back(filePath.string());
		}
		else if (fileType == 1 && is_directory(filePath)) // only folders
		{
			outQueue.push_back(filePath.string());
		}
		else if (fileType == 2 && is_regular_file(filePath)) // only files
		{
			outQueue.push_back(filePath.string());
		}
	}
}