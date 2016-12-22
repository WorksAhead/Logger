#include "QCOSLogger.h"

#include "LogFileMonitor.h"

using namespace QCOS;

int main()
{
	LogUploader uploader;
	LogFileMonitor fileMonitor("./LogFiles", 1, uploader);
	//fileMonitor.ScanFiles();


	QCOSLogger logger;

	while (1)
	{
		logger.WriteLog("LDAP://10.1.0.11/OU=Users,OU=Managed,DC=cyou-inc,DC=com");
		boost::this_thread::sleep_for(boost::chrono::seconds(1));
	}

	return 0;
}
