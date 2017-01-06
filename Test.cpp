#if defined (__linux__)
# include <unistd.h>
# include <errno.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>

# include <boost/program_options.hpp>
namespace po = boost::program_options;
#endif

#include "QCOSLogger.h"
#include "BoostLogWrapper.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

using namespace QCOS;

void DownloadCB(boost::container::deque<std::string>& localLogPathQueue)
{
    QCOS_LOG(debug) << "**********************************";
    BOOST_FOREACH(std::string& logFilePath, localLogPathQueue)
    {
        QCOS_LOG(debug) << logFilePath;
    }
    QCOS_LOG(debug) << "**********************************";
}

void WriteLog(const boost::system::error_code&, boost::asio::deadline_timer* t, QCOSLogger* logger)
{
    logger->WriteLog("LDAP://10.1.0.11/OU=Users,OU=Managed,DC=cyou-inc,DC=com");

    // Wait again.
    t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
    t->async_wait(boost::bind(WriteLog,
        boost::asio::placeholders::error, t, logger));
}

void Download(const boost::system::error_code&, boost::asio::deadline_timer* t, QCOSLogger* logger)
{
    QCOS_LOG(debug) << "It's time to download log files.";

    logger->DownloadLogFiles("2017-01-04 13:00:00", "2017-01-04 14:00:00", 1, &DownloadCB);

    // Download again.
    /*
    t->expires_at(t->expires_at() + boost::posix_time::seconds(10));
    t->async_wait(boost::bind(Download,
        boost::asio::placeholders::error, t, logger));
    */
}

int Daemonize(const char* name)
{
#if defined (_WIN32)
    // Nothing to do
#elif defined (__linux__) || defined (__FreeBSD__)
    // Fork the process and have the parent exit. If the process was started
    // from a shell, this returns control to the user. Forking a new process is
    // also a prerequisite for the subsequent call to setsid().
    if (pid_t pid = fork())
    {
        if (pid > 0)
        {
            // We're in the parent process and need to exit.
            //
            // When the exit() function is used, the program terminates without
            // invoking local variables' destructors. Only global variables are
            // destroyed. As the io_service object is a local variable, this means
            // we do not have to call:
            //
            //   io_service.notify_fork(boost::asio::io_service::fork_parent);
            //
            // However, this line should be added before each call to exit() if
            // using a global io_service object. An additional call:
            //
            //   io_service.notify_fork(boost::asio::io_service::fork_prepare);
            //
            // should also precede the second fork().
            exit(0);
        }
        else
        {
            QCOS_LOG(error) << "First fork failed.";
            exit(-1);
        }
    }

    // Make the process a new session leader. This detaches it from the
    // terminal.
    setsid();

    // A process inherits its working directory from its parent. This could be
    // on a mounted filesystem, which means that the running daemon would
    // prevent this filesystem from being unmounted. Changing to the root
    // directory avoids this problem.
    // chdir("/");

    // The file mode creation mask is also inherited from the parent process.
    // We don't want to restrict the permissions on files created by the
    // daemon, so the mask is cleared.
    umask(0);

    // A second fork ensures the process cannot acquire a controlling terminal.
    if (pid_t pid = fork())
    {
        if (pid > 0)
        {
            exit(0);
        }
        else
        {
            QCOS_LOG(error) << "Second fork failed.";
            exit(-1);
        }
    }

    // Close the standard streams. This decouples the daemon from the terminal
    // that started it.
    close(0);
    close(1);
    close(2);

    // We don't want the daemon to have any standard input.
    if (open("/dev/null", O_RDONLY) < 0)
    {
        QCOS_LOG(error) << "Unable to open /dev/null.";
        exit(-1);
    }

    // Send standard output to a log file.
    const char* output = "./LoginServer.log";
    const int flags = O_WRONLY | O_CREAT | O_APPEND;
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if (open(output, flags, mode) < 0)
    {
        QCOS_LOG(error) << "Unable to open output file " << output << ".";
        exit(-1);
    }

    // Also send standard error to the same log file.
    if (dup(1) < 0)
    {
        QCOS_LOG(error) << "Unable to dup output descriptor.";
        exit(-1);
    }
#endif
    return 0;
}

int main(int ac, char* av[])
{
    bool runAsDaemon = false;

#if defined (__linux__)
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Produce help message")
        ("daemon,d", "Running as a daemon process.")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << "Usage: COSTest [options]" << std::endl;
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("daemon"))
    {
        std::cout << "Process is running as a daemon." << std::endl;
        runAsDaemon = true;
        Daemonize("QCOSTest");
    }
#endif

    InitBoostLog("COSTest.log", runAsDaemon);

    QCOSLogger logger("./LogFiles", 60, DEFAULT_UPLOADER, "./LogDownloadFiles", DEFAULT_DOWNLOADER);

    boost::asio::io_service io;
    
    boost::asio::deadline_timer writeLogTimer(io, boost::posix_time::seconds(1));
    writeLogTimer.async_wait(boost::bind(WriteLog, boost::asio::placeholders::error, &writeLogTimer, &logger));

    boost::asio::deadline_timer downloadTimer(io, boost::posix_time::seconds(10));
    downloadTimer.async_wait(boost::bind(Download, boost::asio::placeholders::error, &downloadTimer, &logger));

    io.run();

    return 0;
}
