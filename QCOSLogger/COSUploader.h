#pragma once

#include "UploaderBase.h"
#if defined(__linux__)
#include "CosApi.h"
#endif

namespace QCOS
{
	class COSUploader : public UploaderBase
	{
	public:
		~COSUploader() {}

		virtual void UploadLogFile(const LogFile& logFile);

        private:
#if defined(__linux__)
                qcloud_cos::CosConfig m_CosConfig {"cosconfig.json"};
                qcloud_cos::CosAPI m_Cos {m_CosConfig};
#endif
	};
}
