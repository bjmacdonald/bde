cmake_minimum_required(VERSION 3.19)
project(bde)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Was min 14, now set to 20 and use std::any
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# BDE build system selector
if (BBS_BUILD_SYSTEM)
    enable_testing()

    find_package(BdeBuildSystem REQUIRED)

    add_subdirectory(thirdparty)
    add_subdirectory(groups)
    add_subdirectory(standalones)
else()
    if (NOT CMAKE_MODULE_PATH)
        message(FATAL "Please specify path to BDE cmake modules.")
    endif()

    include(bde_workspace)

    bde_process_workspace(${CMAKE_CURRENT_LIST_DIR})
endif()
