#include "LogMonitor.h"
#include "BoostLogWrapper.h"
#include "LogUploader.h"
#include "LogFile.h"

#include <boost/format.hpp> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

using namespace QCOS;

LogMonitor::LogMonitor(const std::string& monitorDir, int scanInterval, LogUploader& uploader)
    : m_MonitorDir{ monitorDir }
    , m_ScanInterval{ scanInterval }
    , m_Uploader{ uploader }
    , m_Connection{ uploader.RegisterUploadCallback(boost::bind(&LogMonitor::UploadCallback, this, _1, _2)) }
{
    if (!exists(m_MonitorDir))
    {
        create_directories(m_MonitorDir);
    }
}

LogMonitor::~LogMonitor()
{
    m_Connection.disconnect();
}

void
LogMonitor::operator()()
{
    //InitThread();

    while (!m_Stop)
    {
        if (m_LogFileQueue.size() < 2)
        {
            ScanFiles();
            UploadLogFiles();

            boost::this_thread::sleep_for(boost::chrono::seconds(10));
        }
        else
        {
            boost::this_thread::sleep_for(boost::chrono::seconds(20));
        }
    }
}

void
LogMonitor::ScanFiles()
{
    path rootPath(m_MonitorDir);

    recursive_directory_iterator end;
    for (recursive_directory_iterator pos(rootPath); pos != end; ++pos)
    {
        path filePath(*pos);

        // Ignore folders.
        if (!is_regular_file(filePath))
        {
            continue;
        }

        std::string filePathStr = filePath.generic_string();
        typedef std::vector< std::string > split_vector_type;
        split_vector_type SplitVec;
        boost::split(SplitVec, filePathStr, boost::is_any_of("/"), boost::token_compress_on);

        // Skip files that path depth less than 3.
        size_t splitSize = SplitVec.size();
        if (splitSize < 3)
        {
            continue;
        }

        LogFile logFile;
        logFile.FileName = SplitVec[splitSize - 1];
        logFile.HourFolder = SplitVec[splitSize - 2];
        logFile.DayFolder = SplitVec[splitSize - 3];
        logFile.PathName = boost::str(boost::format("%1%/%2%/%3%") % m_MonitorDir % logFile.DayFolder % logFile.HourFolder);
        logFile.FullPathName = boost::str(boost::format("%1%/%2%/%3%/%4%") % m_MonitorDir % logFile.DayFolder % logFile.HourFolder % logFile.FileName);

        // Check whether we have conject a corrent file name.
        if (!boost::filesystem::exists(logFile.FullPathName))
        {
            continue;
        }

        // Skip the log file that pushed back in last time.
        if (m_LogFileQueue.size() != 0)
        {
            if (m_LogFileQueue.front() != logFile)
            {
                m_LogFileQueue.push_back(logFile);
            }
        }
        else
        {
            m_LogFileQueue.push_back(logFile);
        }
    }
}

void
LogMonitor::UploadLogFiles()
{
    // always keep at least one element in the queue.
    if (m_LogFileQueue.size() < 2)
    {
        return;
    }

    boost::lock_guard<boost::mutex> guard(m_Mutex);
    for (size_t i = 0; i < m_LogFileQueue.size() - 1; i++)
    {
        const LogFile& logFile = m_LogFileQueue[i];

        QCOS_LOG(debug) << "Found log file for uploading: " << logFile.FullPathName;
        m_Uploader.UploadLogFile(logFile);
    }
}

void
LogMonitor::UploadCallback(const LogFile& logFile, bool result)
{
    const LogFile& uploadingLogFile = m_LogFileQueue.front();

    if (uploadingLogFile != logFile)
    {
        QCOS_LOG(warning) << "Callback logfile: " << logFile.FileName << " doesn't match with current uploading logfile: " << uploadingLogFile.FileName;
        return;
    }

    if (result)
    {
        QCOS_LOG(debug) << "Log file: " << logFile.FullPathName << " has been uploaded.";

        boost::lock_guard<boost::mutex> guard(m_Mutex);
        remove(logFile.FullPathName);
        m_LogFileQueue.pop_front();
    }
    else
    {
        QCOS_LOG(warning) << "Log file: " << logFile.FullPathName << " upload failed, try again.";
        // m_Uploader.UploadLogFile(logFile);
    }
}
