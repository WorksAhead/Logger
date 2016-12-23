#include "LogUploader.h"
#if defined(__linux__)
#include "COSUploader.h"
#endif
#include "FakeUploader.h"

using namespace QCOS;

LogUploader::LogUploader(UploaderBase::UploaderType type)
{
    switch (type)
    {
#if defined(__linux__)
    case UploaderBase::UPLOADER_TYPE_COS:
        m_Uploader = new COSUploader();
        break;
#endif
    case UploaderBase::UPLOADER_TYPE_FAKE:
        m_Uploader = new FakeUploader();
        break;
    default:
        m_Uploader = new FakeUploader();
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

    m_UploadSignal.disconnect_all_slots();
}

void
LogUploader::UploadLogFile(const LogFile& logFile)
{
    boost::lock_guard<boost::mutex> guard(m_Mutex);
    m_LogFileQueue.push_back(logFile);
}

void
LogUploader::RegisterUploadCallback(const boost::function<void(const LogFile&, bool result)>& callback)
{
    m_UploadSignal.connect(callback);
}

void
LogUploader::SyncFiles()
{
    if (m_LogFileQueue.size() > 0)
    {
        boost::lock_guard<boost::mutex> guard(m_Mutex);
        m_UploadFileQueue.swap(m_LogFileQueue);
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

        bool result = m_Uploader->UploadLogFile(logFile);

        // Notify all observers that logFile upload success.
        m_UploadSignal(logFile, result);
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