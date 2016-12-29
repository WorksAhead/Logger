#include "QCOSLogger.h"
#include "BoostLogWrapper.h"

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
    InitBoostLog("COSTest.log");

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
