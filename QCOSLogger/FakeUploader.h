#pragma once
#include "UploaderBase.h"

namespace QCOS
{
    class FakeUploader : public UploaderBase
    {
    public:
        ~FakeUploader() {}

        virtual bool UploadLogFile(const LogFile& logFile);
    };
}
#pragma once
