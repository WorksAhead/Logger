#include "QCOSLogger.h"

#include "LogMonitor.h"

using namespace QCOS;

class test
{
public:
    void my(std::vector<LogFile>&)
    {

    }
};

int main()
{
    QCOSLogger logger;

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


    //LogUploader uploader;
    //LogMonitor fileMonitor("./LogFiles", 1, uploader);
    //fileMonitor.ScanFiles();

    while (1)
    {
        logger.WriteLog("LDAP://10.1.0.11/OU=Users,OU=Managed,DC=cyou-inc,DC=com");
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }

    return 0;
}
