#include "FakeDownloader.h"

#include "BoostLogWrapper.h"
#include <boost/thread.hpp>

using namespace QCOS;

bool
FakeDownloader::DownloadLogFile(const LogFile& logFile)
{
    QCOS_LOG(debug) << "Start download file " << logFile.FullPathName;
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
    QCOS_LOG(debug) << "Download finished.";

    return true;
}