#include "FakeDownloader.h"

#include "BoostLogWrapper.h"
#include <boost/thread.hpp>

using namespace QCOS;

bool
FakeDownloader::DownloadLogFile(const std::string& cosPathName, const std::string& localPathName)
{
    QCOS_LOG(debug) << "Start download file " << cosPathName;
    boost::this_thread::sleep_for(boost::chrono::seconds(5));
    QCOS_LOG(debug) << "Download finished.";

    return true;
}