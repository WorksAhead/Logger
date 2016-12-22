#include "COSUploader.h"
#include "LogFile.h"

#include <boost/thread.hpp>

#if defined(__linux__)
#include "CosApi.h"
#include "CosConfig.h"
#include "CosParams.h"
#include "CosDefines.h"
#include "CosSysConfig.h"
#include "util/StringUtil.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace qcloud_cos;
#endif
using namespace QCOS;

void
COSUploader::UploadLogFile(const LogFile& logFile)
{
	// simulate sync upload.
	/*
	std::cout << "Start upload file " << fileName << std::endl;
	boost::this_thread::sleep_for(boost::chrono::seconds(300));
	std::cout << "Upload finished." << std::endl;
    */

#if defined(__linux__)
        std::string bucket = "testbucket";
        std::string srcpath = logFile.FullPathName;
        std::string dstpath = "/" + logFile.DayFolder + "/" + logFile.HourFolder + logFile.FileName;

        FileUploadReq fileUploadReq1(bucket,srcpath, dstpath);
        fileUploadReq1.setInsertOnly(0);
        m_Cos.FileUpload(fileUploadReq1);
#else
	std::cout << "Start upload file " << logFile.FullPathName << std::endl;
	boost::this_thread::sleep_for(boost::chrono::seconds(300));
	std::cout << "Upload finished." << std::endl;
#endif
}
