#include "FakeUploader.h"
#include "LogFile.h"

#include <boost/thread.hpp>

using namespace QCOS;

bool
FakeUploader::UploadLogFile(const LogFile& logFile)
{
    std::cout << "Start upload file " << logFile.FullPathName << std::endl;
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
    std::cout << "Upload finished." << std::endl;

    return true;
}
