#include "FakeUploader.h"
#include "BoostLogWrapper.h"
#include <boost/thread.hpp>

using namespace QCOS;

bool
FakeUploader::UploadLogFile(const LogFile& logFile)
{
    QCOS_LOG(debug) << "Start upload file " << logFile.FullPathName;
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
    QCOS_LOG(debug) << "Upload finished.";

    return true;
}
