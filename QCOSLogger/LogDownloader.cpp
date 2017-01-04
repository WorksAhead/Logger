#include "LogDownloader.h"
#if defined(__linux__)
#include "COSDownloader.h"
#endif
#include "FakeDownloader.h"

#include <boost/format.hpp> 

using namespace QCOS;
using namespace boost::gregorian;
using namespace boost::posix_time;

LogDownloader::LogDownloader(const std::string& downloadDir, DownloaderBase::DownloaderType downloaderType)
    : m_DownloadDir{ downloadDir }
{
    switch (downloaderType)
    {
#if defined(__linux__)
    case DownloaderBase::DOWNLOADER_TYPE_COS:
        m_Downloader = new COSDownloader();
        break;
#endif
    case DownloaderBase::DOWNLOADER_TYPE_FAKE:
        m_Downloader = new FakeDownloader();
        break;
    default:
        m_Downloader = new FakeDownloader();
        break;
    }
}

LogDownloader::~LogDownloader()
{
    delete m_Downloader;
}

void
LogDownloader::operator()()
{
    while (!m_Stop)
    {
        SyncFiles();
    }

    // Flush log files.
    if (m_SessionFrontQueue.size() > 0)
    {
        SyncFiles();
    }
}

void
LogDownloader::DownloadLogFiles(const std::string& fromTimeStr, const std::string& toTimeStr, int interval, DownloadCallback callback)
{
    boost::shared_ptr<DownloadSession> downloadReq = boost::make_shared<DownloadSession>();
    downloadReq->Callback = callback;

    ptime from(time_from_string(fromTimeStr));
    ptime to(time_from_string(toTimeStr));

    time_iterator titr(from, minutes(interval));

    boost::lock_guard<boost::mutex> guard(m_Mutex);

    while (titr <= to)
    {
        ptime& pt = *titr;
        std::string cosPathName = GetCOSPathName(pt);
        downloadReq->RequestPathQueue.push_back(cosPathName);
        ++titr;
    }

    m_SessionFrontQueue.push_back(downloadReq);
}

void
LogDownloader::SyncFiles()
{
    if (m_SessionFrontQueue.size() > 0)
    {
        boost::lock_guard<boost::mutex> guard(m_Mutex);
        m_SessionBackQueue.swap(m_SessionFrontQueue);
    }
    else
    {
        boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
        m_Thread->sleep(timeout);
    }

    while (!m_SessionBackQueue.empty())
    {
        boost::shared_ptr<DownloadSession> downloadReq = m_SessionBackQueue.front();
        while (!downloadReq->RequestPathQueue.empty())
        {
            std::string& cosPathName = downloadReq->RequestPathQueue.front();

            bool result = m_Downloader->DownloadLogFile(cosPathName);

            if (result)
            {
                std::string localLogPath = boost::str(boost::format("%1%%2%") % m_DownloadDir % cosPathName);
                downloadReq->ResponsePathQueue.push_back(localLogPath);
                downloadReq->RequestPathQueue.pop_front();
            }
        }

        downloadReq->Callback(downloadReq->ResponsePathQueue);
        m_SessionBackQueue.pop_front();
    }
}

std::string
LogDownloader::GetCOSPathName(const ptime& pt)
{
    int year = pt.date().year();
    int month = pt.date().month();
    int day = pt.date().day();
    int hours = pt.time_of_day().hours();
    int minutes = pt.time_of_day().minutes();

    std::string dayFolder = boost::str(boost::format("%1$04d_%2$02d_%3$02d") % year % month % day);
    std::string hourFolder = boost::str(boost::format("%1$02d") % hours);
    std::string fileName = boost::str(boost::format("%02d.txt") % minutes);
    
    return boost::str(boost::format("/%1%/%2%/%3%") % dayFolder % hourFolder % fileName);
}