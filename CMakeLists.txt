cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0128 OLD)

project(bde)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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
