#include "QCOSLogger.h"

#include "LogMonitor.h"

using namespace QCOS;

int main()
{
    QCOSLogger logger;

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
