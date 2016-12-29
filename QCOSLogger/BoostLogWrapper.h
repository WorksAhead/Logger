#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/mutable_constant.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

extern thread_local src::severity_logger<logging::trivial::severity_level> t_LogSource;
extern thread_local bool t_InitLogAttributes;

// Set attribute and return the new value
template<typename ValueType>
ValueType set_get_attrib(const char* name, ValueType value) {
    auto attr = logging::attribute_cast<attrs::mutable_constant<ValueType>>(logging::core::get()->get_thread_attributes()[name]);
    attr.set(value);
    return attr.get();
}

// Convert file path to only the filename
inline std::string path_to_filename(std::string path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

inline void InitLogAttributes()
{
    if (t_InitLogAttributes)
    {
        t_InitLogAttributes = false;
        // New attributes that hold filename and line number
        logging::core::get()->add_thread_attribute("File", attrs::mutable_constant<std::string>(""));
        logging::core::get()->add_thread_attribute("Line", attrs::mutable_constant<int>(0));
    }
}

inline void InitBoostLog(const std::string& logFileName, bool runAsDaemon = false)
{
    InitLogAttributes();

    logging::formatter formatter =
        expr::stream
        << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
        << ": <" << boost::log::trivial::severity << "> "
        << '[' << expr::attr<std::string>("File")
        << ':' << expr::attr<int>("Line") << "] "
        << expr::smessage;

    if (runAsDaemon)
    {
        auto fileSink = logging::add_file_log(
            keywords::file_name = logFileName
        );
        fileSink->set_formatter(formatter);
    }
    else
    {
        auto consoleSink = logging::add_console_log();
        consoleSink->set_formatter(formatter);
    }

    logging::add_common_attributes();

#if defined(NDEBUG)
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
#endif
}

// New macro that includes severity, filename and line number
#define QCOS_LOG(sev) \
   InitLogAttributes(); \
   BOOST_LOG_STREAM_WITH_PARAMS( \
         (t_LogSource), \
         (set_get_attrib("File", path_to_filename(__FILE__))) \
         (set_get_attrib("Line", __LINE__)) \
         (::boost::log::keywords::severity = (boost::log::trivial::sev)) \
   )

