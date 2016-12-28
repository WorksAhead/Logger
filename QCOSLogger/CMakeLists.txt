# Build QCOSLogger

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(QCOSLogger)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)

if(UNIX)
    include_directories(../cos-cpp-sdk-v4/include)

    find_package(Boost
                 1.6.2
                 REQUIRED
                 COMPONENTS filesystem thread system)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(WIN32)
    add_compile_options(/wd"4819")
	# Disable warning C4996: 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
	# It is nothing to worry about. In the last few releases of MSVC, they've gone into full security-paranoia mode. 
	# std::copy issues this warning when it is used with raw pointers, because when used incorrectly, it can result in buffer overflows.
    # Their iterator implementation performs bounds checking to ensure this doesn't happen, at a significant performance cost.
    # So feel free to ignore the warning. It doesn't mean there's anything wrong with your code. 
	# It is just saying that if there is something wrong with your code, then bad things will happen. Which is an odd thing to issue warnings about
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

add_library(QCOSLogger
            QCOSLogger.cpp
            QCOSLogger.h
            LogFile.h
            LogRecorder.cpp
            LogRecorder.h
            UploaderBase.h
            LogUploader.cpp
            LogUploader.h
            COSUploader.cpp
            COSUploader.h
            FakeUploader.cpp
            FakeUploader.h
            LogMonitor.cpp
            LogMonitor.h
            LogDownloader.cpp
            LogDownloader.h)
#set_target_properties(QCOSLogger PROPERTIES OUTPUT_NAME_DEBUG QCOSLogger_d)
target_compile_features(QCOSLogger PRIVATE cxx_generalized_initializers)
