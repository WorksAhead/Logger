#if defined(__linux__)

#include "COSDownloader.h"
#include "BoostLogWrapper.h"

#include <boost/filesystem.hpp>
#include <boost/smart_ptr.hpp>

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
COSDownloader::DownloadLogFile(const std::string& cosPathName, const std::string& localPathName)
{
    std::string bucket = "testbucket";
    std::string dstpath = cosPathName;

    FileStatReq fileStatReq(bucket, dstpath);
    std::string result = m_Cos.FileStat(fileStatReq);

    QCOS_LOG(debug) << "Get File " << cosPathName << " status: " << result;

    Json::Value root_json = StringUtil::StringToJson(result);
    int code = -1;
    if (root_json.isMember("code"))
    {
        code = root_json["code"].asInt();
        if (code != 0)
        {
            return false;
        }

        size_t fileSize = root_json["data"]["filesize"].asInt();
        QCOS_LOG(debug) << "File size is " << fileSize;

        FileDownloadReq fileDownloadReq(bucket, dstpath);
        boost::scoped_array<char> buf(new (nothrow) char[fileSize]);
	int ret_code;
	size_t size = m_Cos.FileDownload(fileDownloadReq, buf.get(), fileSize, 0, &ret_code);

        if (ret_code != 0)
        {
            QCOS_LOG(debug) << "Failed to download file " << cosPathName;
            return false;
        }

        if (size != fileSize)
        {
            QCOS_LOG(debug) << "Downloaded file " << cosPathName << " with incorrect size";
            return false;
        }

        boost::filesystem::ofstream fileStream;
        fileStream.open(localPathName, std::ofstream::out | std::ofstream::app);
        fileStream << buf.get();
        fileStream.close();
    }

    return true;
}

#endif
