#pragma once

#include <boost/thread.hpp>

namespace QCOS
{
    class LogThread
    {
    public:
        LogThread();
        virtual ~LogThread();

        virtual void operator()() = 0;

    protected:
        bool m_Stop{ false };

        boost::mutex m_Mutex;
        boost::thread* m_Thread;
    };
}