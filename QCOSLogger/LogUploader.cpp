#include "LogUploader.h"
#include "COSUploader.h"
#include "LogFile.h"

using namespace QCOS;

LogUploader::LogUploader(UploaderBase::UploaderType uploader)
{
	switch (uploader)
	{
	case UploaderBase::UPLOADER_TYPE_COS:
		m_Uploader = new COSUploader();
		break;
	}

	m_Thread = new boost::thread(boost::ref(*this));
}

LogUploader::~LogUploader()
{
	m_Stop = true;

	m_Thread->join();
	
	delete m_Thread;
	delete m_Uploader;
}

void
LogUploader::UploadLogFile(const LogFile& logFile)
{
	boost::lock_guard<boost::mutex> guard(m_Mutex);
	m_LogFileQueue.push_back(logFile);
}

void
LogUploader::SyncFiles()
{
	if (m_LogFileQueue.size() > 0)
	{
		boost::lock_guard<boost::mutex> guard(m_Mutex);

		std::cout << "Swap upload queue." << std::endl;
		std::cout << "Before swap log file queue size is: " << m_LogFileQueue.size() << std::endl;
		std::cout << "Before swap upload file queue size is: " << m_UploadFileQueue.size() << std::endl;

		m_UploadFileQueue.swap(m_LogFileQueue);

		std::cout << "Swapped log file queue size is: " << m_LogFileQueue.size() << std::endl;
		std::cout << "Swapped upload file queue size is: " << m_UploadFileQueue.size() << std::endl;
	}
	else
	{
		boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
		m_Thread->sleep(timeout);
	}

	while (!m_UploadFileQueue.empty())
	{
		LogFile logFile = m_UploadFileQueue.front();
		m_UploadFileQueue.pop_front();

		m_Uploader->UploadLogFile(logFile);
	}
}

void
LogUploader::operator()()
{
	while (!m_Stop)
	{
		SyncFiles();
	}

	// Flush log files.
	if (m_LogFileQueue.size() > 0)
	{
		SyncFiles();
	}
}