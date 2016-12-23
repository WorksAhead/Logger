#if defined(__linux__)

#include "COSUploader.h"
#include "LogFile.h"

#include <boost/thread.hpp>

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

using namespace QCOS;

bool
COSUploader::UploadLogFile(const LogFile& logFile)
{
    std::string bucket = "testbucket";
    std::string srcpath = logFile.FullPathName;
    std::string dstpath = "/" + logFile.DayFolder + "/" + logFile.HourFolder + logFile.FileName;

    FileUploadReq fileUploadReq1(bucket, srcpath, dstpath);
    fileUploadReq1.setInsertOnly(0);
    m_Cos.FileUpload(fileUploadReq1);

    return true;
}

#endif