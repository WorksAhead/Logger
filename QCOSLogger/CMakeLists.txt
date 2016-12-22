# Build QCOSLogger

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(QCOSLogger)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)

if(UNIX)
    include_directories(../cos-cpp-sdk-v4/include)

    if(NOT DEFINED BOOST_ROOT)
        message(FATAL_ERROR "You must specify BOOST_ROOT.")
    endif()

    find_package(Boost
                 1.6.2
                 REQUIRED
                 COMPONENTS system thread)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(WIN32)
    add_compile_options(/wd"4819")
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
            LogFileMonitor.cpp
            LogFileMonitor.h)
#set_target_properties(QCOSLogger PROPERTIES OUTPUT_NAME_DEBUG QCOSLogger_d)
target_compile_features(QCOSLogger PRIVATE cxx_generalized_initializers)
