#pragma once

#include <boost/thread.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

namespace QCOS
{
    class LogFile;

    typedef boost::signals2::signal<void(std::vector<LogFile>&)> DownloadSignalType;

    struct LogDate
    {
        std::string Day;
        std::string Hour;
        std::string Minute;

        bool operator==(const LogDate& rhs) const
        {
            if (Day == rhs.Day &&
                Hour == rhs.Hour &&
                Minute == rhs.Minute)
            {
                return true;
            }

            return false;
        }
    };

    struct DownloadRequest
    {
        LogDate From;
        LogDate To;
        DownloadSignalType Signal;
    };

    class LogDownloader
    {
    public:
        // Scan file interval in seconds
        LogDownloader();
        ~LogDownloader();

        boost::signals2::connection Download(const LogDate& fromDate,
                                             const LogDate& toDate,
                       const DownloadSignalType::slot_type& subscriber);

        void DownloadFiles();

        void operator()();

    private:
        // get date between fromDate and toDate, exclude them.
        // interval is minute.
        bool GetNextLogDate(const LogDate& fromDate, 
                            const LogDate& toDate, 
                                  LogDate& nextDate, 
                                       int interval);

    private:
        bool m_Stop{ false };

        boost::mutex m_Mutex;
        boost::thread* m_Thread{ nullptr };

        boost::container::deque<boost::shared_ptr<DownloadRequest> > m_DownloadRequestQueue;
        boost::container::deque<boost::shared_ptr<DownloadRequest> > m_DownloadQueue;
    };
}
