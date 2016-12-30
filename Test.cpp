#include "QCOSLogger.h"
#include "BoostLogWrapper.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace QCOS;

class test
{
public:
    void my(std::vector<LogFile>&)
    {

    }
};

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
    po::options_description desc("Generic options");
    desc.add_options()
        ("daemon,d", "Running as a daemon process.")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(desc);

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("daemon"))
    {
        runAsDaemon = true;
        Daemonize("QCOSTest");
    }
#endif

    InitBoostLog("COSTest.log", runAsDaemon);

    QCOSLogger logger;

    /*
    return 0;

    LogDate fromDate;
    fromDate.Day = "2016_12_26";
    fromDate.Hour = "00";
    fromDate.Minute = "00";
    LogDate toDate;
    toDate.Day = "2016_12_27";
    toDate.Hour = "15";
    toDate.Minute = "58";

    test t;

    logger.DownloadLogFiles(fromDate, toDate, boost::bind(&test::my, &t, _1));
    */

    while (1)
    {
        logger.WriteLog("LDAP://10.1.0.11/OU=Users,OU=Managed,DC=cyou-inc,DC=com");
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    return 0;
}
