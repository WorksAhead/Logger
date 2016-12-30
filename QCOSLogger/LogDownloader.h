#pragma once

#include "DownloaderBase.h"
#include "LogThread.h"

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

    class LogDownloader : public LogThread
    {
        typedef boost::signals2::signal<void(const LogFile&, bool result)> SignalType;

    public:
        // Scan file interval in seconds
        LogDownloader(const std::string& downloadDir, DownloaderBase::DownloaderType downloaderType);
        ~LogDownloader();

        virtual void operator()();

        boost::signals2::connection Download(const LogDate& fromDate,
                                             const LogDate& toDate,
                       const DownloadSignalType::slot_type& subscriber);

    private:
        void DownloadFiles();

        // get date between fromDate and toDate, exclude them.
        // interval is minute.
        bool GetNextLogDate(const LogDate& fromDate, 
                            const LogDate& toDate, 
                                  LogDate& nextDate, 
                                       int interval);

    private:
        std::string m_DownloadDir;
        DownloaderBase* m_Downloader{ nullptr };
        SignalType m_DownloadSignal;

        boost::container::deque<boost::shared_ptr<DownloadRequest> > m_DownloadRequestQueue;
        boost::container::deque<boost::shared_ptr<DownloadRequest> > m_DownloadQueue;
    };
}
