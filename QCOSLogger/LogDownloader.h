#pragma once

#include "DownloaderBase.h"
#include "LogThread.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/container/deque.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2.hpp>

namespace QCOS
{
    typedef boost::function<void(boost::container::deque<std::string>&)> DownloadCallback;

    struct DownloadSession
    {
        boost::container::deque<std::string> RequestPathQueue;
        boost::container::deque<std::string> ResponsePathQueue;
        DownloadCallback Callback;
    };

    class LogDownloader : public LogThread
    {
    public:
        // Scan file interval in seconds
        LogDownloader(const std::string& downloadDir, DownloaderBase::DownloaderType downloaderType);
        ~LogDownloader();

        virtual void operator()();

        // Post full path log file name to downloader thread.
        void DownloadLogFiles(const std::string& fromTimeStr, const std::string& toTimeStr, int interval, DownloadCallback callback);

    private:
        void SyncFiles();

        std::string GetCOSPathName(const boost::posix_time::ptime& pt);

    private:
        std::string m_DownloadDir;
        DownloaderBase* m_Downloader{ nullptr };

        boost::container::deque<boost::shared_ptr<DownloadSession> > m_SessionFrontQueue;
        boost::container::deque<boost::shared_ptr<DownloadSession> > m_SessionBackQueue;
    };
}
