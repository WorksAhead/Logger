#include "LogDownloader.h"
#if defined(__linux__)
#include "COSDownloader.h"
#endif
#include "FakeDownloader.h"

#include <boost/format.hpp> 
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace QCOS;
using namespace boost::gregorian;

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
    m_DownloadSignal.disconnect_all_slots();
    delete m_Downloader;
}

void
LogDownloader::operator()()
{
    while (!m_Stop)
    {
        DownloadFiles();
    }

    // Flush log files.
    if (m_DownloadRequestQueue.size() > 0)
    {
        DownloadFiles();
    }
}

boost::signals2::connection
LogDownloader::Download(const LogDate& fromDate,
                        const LogDate& toDate,
  const DownloadSignalType::slot_type& subscriber)
{
    boost::lock_guard<boost::mutex> guard(m_Mutex);

    boost::shared_ptr<DownloadRequest> request(new DownloadRequest);
    request->From = fromDate;
    request->To = toDate;
    boost::signals2::connection conn = request->Signal.connect(subscriber);
    m_DownloadRequestQueue.push_back(request);

    return conn;
}

void
LogDownloader::DownloadFiles()
{
    if (m_DownloadRequestQueue.size() > 0)
    {
        boost::lock_guard<boost::mutex> guard(m_Mutex);
        m_DownloadQueue.swap(m_DownloadRequestQueue);
    }
    else
    {
        boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
        m_Thread->sleep(timeout);
    }

    while (!m_DownloadQueue.empty())
    {
        boost::shared_ptr<DownloadRequest> downloadReq = m_DownloadQueue.front();
        
        LogDate logDate;
        while (GetNextLogDate(downloadReq->From, downloadReq->To, logDate, 1))
        {
            std::cout << "Download: " << logDate.Day << "/" << logDate.Hour << "/" << logDate.Minute << ".txt" << std::endl;
            downloadReq->From = logDate;

            LogFile logFile;
            logFile.DayFolder = logDate.Day;
            logFile.HourFolder = logDate.Hour;
            logFile.FileName = boost::str(boost::format("%1%.txt") % logDate.Minute);
            logFile.PathName = boost::str(boost::format("%1%/%2%/%3%") % m_DownloadDir % logFile.DayFolder % logFile.HourFolder);
            logFile.FullPathName = boost::str(boost::format("%1%/%2%/%3%/%4%") % m_DownloadDir % logFile.DayFolder % logFile.HourFolder % logFile.FileName);

            RETRY:
            bool result = m_Downloader->DownloadLogFile(logFile);

            m_DownloadSignal(logFile, result);

            if (result)
            {
                m_DownloadQueue.pop_front();
            }
            else
            {
                goto RETRY;
            }
        }
    }
}

bool
LogDownloader::GetNextLogDate(const LogDate& fromDate, const LogDate& toDate, LogDate& nextDate, int interval)
{
    int newMin = boost::lexical_cast<int>(fromDate.Minute);
    newMin += interval;

    int newHour = boost::lexical_cast<int>(fromDate.Hour);
    if (newMin >= 60)
    {
        newHour++;
        newMin -= 60;
    }

    std::string dayStr = boost::replace_all_copy(fromDate.Day, "_", "/");

    date newDay = from_string(dayStr);
    if (newHour >= 24)
    {
        date_duration day(1);
        newDay += day;
        newHour -= 24;
    }

    int year = newDay.year();
    int month = newDay.month();
    int day = newDay.day();

    nextDate.Day = boost::str(boost::format("%1%_%2%_%3%") % year % month % day);
    nextDate.Hour = boost::lexical_cast<std::string>(newHour);
    nextDate.Minute = boost::lexical_cast<std::string>(newMin);

    if (toDate == nextDate)
    {
        return false;
    }

    return true;
}
