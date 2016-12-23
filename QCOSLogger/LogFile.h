#pragma once
#include <string>

namespace QCOS
{
    class LogFile
    {
    public:
        std::string FileName;
        std::string PathName;
        std::string FullPathName;
        std::string DayFolder;
        std::string HourFolder;

        void Delete() {}

        bool operator==(const LogFile& rhs) const
        {
            if (FileName == rhs.FileName &&
                PathName == rhs.PathName &&
                FullPathName == rhs.FullPathName &&
                DayFolder == rhs.DayFolder &&
                HourFolder == rhs.HourFolder)
            {
                return true;
            }

            return false;
        }

        bool operator!=(const LogFile& rhs) const
        {
            return !this->operator==(rhs);
        }
    };
}