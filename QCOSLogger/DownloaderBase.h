#pragma once

#include "LogFile.h"

namespace QCOS
{
    class DownloaderBase
    {
    public:
        typedef enum
        {
#if defined(__linux__)
            DOWNLOADER_TYPE_COS,
#endif
            DOWNLOADER_TYPE_FAKE,
        } DownloaderType;

        virtual ~DownloaderBase() {};

        virtual bool DownloadLogFile(const LogFile& logFile) = 0;

        virtual std::string GetTypeName() = 0;
    };
}