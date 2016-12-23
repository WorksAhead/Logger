#pragma once

#include "LogFile.h"

namespace QCOS
{
    class UploaderBase
    {
    public:
        typedef enum
        {
#if defined(__linux__)
            UPLOADER_TYPE_COS,
#endif
            UPLOADER_TYPE_FAKE,
        } UploaderType;

        virtual ~UploaderBase() {};

        virtual bool UploadLogFile(const LogFile& logFile) = 0;
    };
}