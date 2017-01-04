#pragma once
#include "DownloaderBase.h"

namespace QCOS
{
    class FakeDownloader : public DownloaderBase
    {
    public:
        ~FakeDownloader() {}

        virtual bool DownloadLogFile(const std::string& cosPathName);

        virtual std::string GetTypeName()
        {
            return "FakeDownloader";
        }
    };
}
