# Build COSTest.

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(COSTest)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

if(UNIX)
    if(NOT DEFINED BOOST_ROOT)
        message(FATAL_ERROR "You must specify BOOST_ROOT.")
    endif()

    find_package(Boost
                 1.6.2
                 REQUIRED
                 COMPONENTS system thread)
    include_directories(${Boost_INCLUDE_DIRS})

    add_subdirectory(./cos-cpp-sdk-v4/src)

    link_directories(./cos-cpp-sdk-v4/lib)
    link_directories(./cos-cpp-sdk-v4/build/lib)

    set(COS_LIBRARIES cossdk curl ssl dl crypto rt jsoncpp stdc++ pthread z)
endif()

add_subdirectory(./QCOSLogger)

include_directories(./QCOSLogger)

add_executable(COSTest Test.cpp)
target_link_libraries(COSTest QCOSLogger ${COS_LIBRARIES} ${Boost_LIBRARIES})
target_compile_features(COSTest PRIVATE cxx_generalized_initializers)
