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
    pid_t child;
    FILE* f;
    char pidFile[80];

    child = fork();
    if (!child)
    {
        child = fork();
        if (child) exit(0);
    }
    else
    {
        // The master process collects the zombie 
        // process of forking the child daemon and exits 
        //#if !defined (__FreeBSD__)
        //                waitpid(child, NULL, 0);
        //                return 1;
        //#else
        exit(0);
        //#endif
    }

    // Get rid of controlling terminal.
    setsid();

    // Have to close the original standard I/O 
    // streams and create our own standard I/O streams,
    // otherwise the parment process cannot exit cleanly 
    umask(0);
    close(0);
    close(1);
    close(2);
    open("/dev/null", O_RDWR, 0);
    dup2(0, 1);
    dup2(0, 2);

    snprintf(pidFile, sizeof(pidFile), "/var/run/%s.pid", name);
    f = fopen(pidFile, "w");
    if (!f)
    {
        fprintf(stderr, "failed to open %s: %s", pidFile, strerror(errno));
    }
    else
    {
        fprintf(f, "%u\n", getpid());
        fclose(f);
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
        ("help", "Produce help message.")
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
