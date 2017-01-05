#pragma once
#include <string>

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

        virtual bool DownloadLogFile(const std::string& cosPathName, const std::string& localPathName) = 0;

        virtual std::string GetTypeName() = 0;
    };
}