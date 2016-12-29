#include "BoostLogWrapper.h"

thread_local src::severity_logger<logging::trivial::severity_level> t_LogSource;
thread_local bool t_InitLogAttributes = true;