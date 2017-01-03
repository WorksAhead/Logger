#include "LogRecorder.h"
#include "BoostLogWrapper.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp> 
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

using namespace QCOS;

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

LogRecorder::LogRecorder(const std::string& logOutputDir, int logSinkInterval)
    : m_OutputDir{ logOutputDir }
    , m_SinkInterval{ logSinkInterval }
{

}

LogRecorder::~LogRecorder()
{

}

void
LogRecorder::operator()()
{
    while (!m_Stop)
    {
        UpdateLogFile();
        SinkLogFile();

        if (m_LogRecordQueue.empty())
        {
            boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(500);
            m_Thread->sleep(timeout);
        }
    }

    SinkLogFile();
    if (m_FileStream.is_open())
    {
        m_FileStream.close();
    }
}

void
LogRecorder::WriteLog(const std::string& text)
{
    boost::lock_guard<boost::mutex> guard(m_Mutex);
    m_LogRecordQueue.push_back(text);
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

    int recordMinutes = minutes - minutes % (m_SinkInterval / 60);

    LogFile logFile;
    logFile.DayFolder = boost::str(boost::format("%1$04d_%2$02d_%3$02d") % year % month % day);
    logFile.HourFolder = boost::str(boost::format("%1$02d") % hours);//boost::lexical_cast<std::string>(hours);
    logFile.FileName = boost::str(boost::format("%02d.txt") % recordMinutes);
    logFile.PathName = boost::str(boost::format("%1%/%2%/%3%") % m_OutputDir % logFile.DayFolder % logFile.HourFolder);
    logFile.FullPathName = boost::str(boost::format("%1%/%2%/%3%/%4%") % m_OutputDir % logFile.DayFolder % logFile.HourFolder % logFile.FileName);

    if (m_LogFile != logFile)
    {
        if (m_FileStream.is_open())
        {
            m_FileStream.close();
        }

        if (!m_LogRecordQueue.empty())
        {
            m_LogFile = logFile;

            if (!boost::filesystem::exists(m_LogFile.PathName))
            {
                boost::filesystem::create_directories(m_LogFile.PathName);
            }

            QCOS_LOG(debug) << "Sink new log file: " << m_LogFile.FullPathName;
            m_FileStream.open(m_LogFile.FullPathName, std::ofstream::out | std::ofstream::app);
        }
    }
}

void
LogRecorder::SinkLogFile()
{
    if (m_FileStream.is_open())
    {
        boost::lock_guard<boost::mutex> guard(m_Mutex);
        while (!m_LogRecordQueue.empty())
        {
            std::string log = m_LogRecordQueue.front();
            m_LogRecordQueue.pop_front();

            m_FileStream << log << std::endl;
        }

        m_FileStream.flush();
    }
}
