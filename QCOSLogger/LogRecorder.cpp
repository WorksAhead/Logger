#include "LogRecorder.h"

#include <boost/format.hpp> 
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace QCOS;

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

LogRecorder::LogRecorder(const std::string& logRootDir, int interval)
    : m_LogRootDir{ logRootDir }
    , m_Interval{ interval }
{
    UpdateLogFile();
    m_Thread = new boost::thread(boost::ref(*this));
}

LogRecorder::~LogRecorder()
{
    m_Stop = true;

    m_Thread->join();

    delete m_Thread;
}

void
LogRecorder::WriteLog(const std::string& text)
{
    boost::lock_guard<boost::mutex> guard(m_Mutex);
    m_LogQueue.push_back(text);
}

void
LogRecorder::UpdateLogFile()
{
    time_zone_ptr timeZone{ new posix_time_zone{ "CET+8" } };
    local_date_time currentTime{ second_clock::universal_time(), timeZone };
    ptime localTime = currentTime.local_time();

    int year = localTime.date().year();
    int month = localTime.date().month();
    int day = localTime.date().day();
    int hours = localTime.time_of_day().hours();
    int minutes = localTime.time_of_day().minutes();
    int seconds = localTime.time_of_day().seconds();

    int recordMinutes = minutes - minutes % (m_Interval / 60);

    LogFile logFile;
    logFile.DayFolder = boost::str(boost::format("%1%_%2%_%3%") % year % month % day);
    logFile.HourFolder = boost::lexical_cast<std::string>(hours);
    logFile.FileName = boost::str(boost::format("%02d.txt") % recordMinutes);
    logFile.PathName = boost::str(boost::format("%1%/%2%/%3%") % m_LogRootDir % logFile.DayFolder % logFile.HourFolder);
    logFile.FullPathName = boost::str(boost::format("%1%/%2%/%3%/%4%") % m_LogRootDir % logFile.DayFolder % logFile.HourFolder % logFile.FileName);

    if (!boost::filesystem::exists(logFile.PathName))
    {
        boost::filesystem::create_directories(logFile.PathName);
    }

    if (logFile != m_LogFile)
    {
        if (m_FileStream.is_open())
        {
            m_FileStream.close();
        }

        m_LogFile = logFile;
        m_FileStream.open(m_LogFile.FullPathName, std::ofstream::out | std::ofstream::app);
    }
    else
    {
        m_FileStream.flush();
    }
}

void
LogRecorder::SyncFile()
{
    {
        boost::lock_guard<boost::mutex> guard(m_Mutex);
        while (!m_LogQueue.empty())
        {
            std::string log = m_LogQueue.front();
            m_LogQueue.pop_front();

            m_FileStream << log << std::endl;
        }
    }

    UpdateLogFile();
}

void
LogRecorder::operator()()
{
    while (!m_Stop)
    {
        SyncFile();

        if (m_LogQueue.empty())
        {
            boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(500);
            m_Thread->sleep(timeout);
        }
    }

    SyncFile();
    if (m_FileStream.is_open())
    {
        m_FileStream.close();
    }
}

