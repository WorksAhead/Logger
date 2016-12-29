#include "LogThread.h"

using namespace QCOS;

LogThread::LogThread()
{
    m_Thread = new boost::thread(boost::ref(*this));
}

LogThread::~LogThread()
{
    m_Stop = true;
    m_Thread->join();
    delete m_Thread;
}
