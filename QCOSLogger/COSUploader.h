#pragma once
#if defined(__linux__)

#include "UploaderBase.h"
#include "CosApi.h"

namespace QCOS
{
    class COSUploader : public UploaderBase
    {
    public:
        ~COSUploader() {}

        virtual bool UploadLogFile(const LogFile& logFile);

    private:
        qcloud_cos::CosConfig m_CosConfig{ "cosconfig.json" };
        qcloud_cos::CosAPI m_Cos{ m_CosConfig };
    };
}

#endif