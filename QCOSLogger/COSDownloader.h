#pragma once
#include "DownloaderBase.h"

namespace QCOS
{
    class COSDownloader : public DownloaderBase
    {
    public:
        ~COSDownloader() {}

        virtual bool DownloadLogFile(const LogFile& logFile);

        virtual std::string GetTypeName()
        {
            return "COSDownloader";
        }

    private:
        //qcloud_cos::CosConfig m_CosConfig{ "cosconfig.json" };
        //qcloud_cos::CosAPI m_Cos{ m_CosConfig };
    };
}