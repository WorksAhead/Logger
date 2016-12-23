#include "LogMonitor.h"
#include "LogUploader.h"
#include "LogFile.h"

#include <boost/foreach.hpp> 
#include <boost/format.hpp> 
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

using namespace QCOS;

LogMonitor::LogMonitor(const std::string& logRootDir, int interval, LogUploader& uploader)
    : m_LogRootDir{ logRootDir }
    , m_Interval{ interval }
    , m_Uploader{ uploader }
{
    // Register callback to uploader.
    uploader.RegisterUploadCallback(boost::bind(&LogMonitor::UploadCallback, this, _1, _2));

    // Initial day folders
    ScanFiles();
    UploadLogFiles();

    m_Thread = new boost::thread(boost::ref(*this));
}

LogMonitor::~LogMonitor()
{
    m_Stop = true;

    m_Thread->join();

    delete m_Thread;
}

void
LogMonitor::UploadCallback(const LogFile& logFile, bool result)
{
    const LogFile& frontLogFile = m_LogFileQueue.front();

    if (frontLogFile != logFile)
    {
        std::cout << "Log file dont match!" << std::endl;
        return;
    }

    if (result)
    {
        std::cout << "Log file: " << logFile.FullPathName << " has been uploaded." << std::endl;
        remove(logFile.FullPathName);
        m_LogFileQueue.pop_front();
    }
    else
    {
        std::cout << "Log file: " << logFile.FullPathName << " upload failed, try again." << std::endl;
        m_Uploader.UploadLogFile(logFile);
    }
}

void
LogMonitor::ScanFiles()
{
    path rootPath(m_LogRootDir);

    if (!boost::filesystem::exists(rootPath))
    {
        boost::filesystem::create_directories(rootPath);
    }

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

        std::cout << logFile.FullPathName << std::endl;

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

    std::cout << std::endl;

    for (size_t i = 0; i < m_LogFileQueue.size() - 1; i++)
    {
        const LogFile& logFile = m_LogFileQueue[i];

        m_Uploader.UploadLogFile(logFile);
    }
}

void
LogMonitor::operator()()
{
    while (!m_Stop)
    {
        if (m_LogFileQueue.size() < 2)
        {
            ScanFiles();
            UploadLogFiles();

            boost::this_thread::sleep_for(boost::chrono::seconds(5));
        }
        else
        {
            boost::this_thread::sleep_for(boost::chrono::seconds(1));
        }
    }
}
