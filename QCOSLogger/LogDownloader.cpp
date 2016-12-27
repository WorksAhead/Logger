#include "LogDownloader.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::gregorian;

using namespace QCOS;

LogDownloader::LogDownloader()
{
    m_Thread = new boost::thread(boost::ref(*this));
}

LogDownloader::~LogDownloader()
{
    m_Stop = true;

    m_Thread->join();

    delete m_Thread;
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
        }
        //bool result = m_Uploader->UploadLogFile(logFile);

        // Notify all observers that logFile upload success.
        // download.Signal();
        m_DownloadQueue.pop_front();
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