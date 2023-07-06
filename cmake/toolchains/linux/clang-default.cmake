# Default compiler-less toolchains for BDE build system.
# The actual compiler for this toolchain is passed via CXX and CC 
# environment variables.
#
# Linux, clang

set(DEFAULT_CXX_FLAGS_INIT "$ENV{CXXFLAGS}")

string(CONCAT DEFAULT_CXX_FLAGS
       "${DEFAULT_CXX_FLAGS_INIT} "
       "-m${BUILD_BITNESS} "
       "-march=westmere "
       "-fno-strict-aliasing "
       "-fdiagnostics-show-option "
      )

set(DEFAULT_C_FLAGS_INIT "$ENV{CFLAGS}")

string(CONCAT DEFAULT_C_FLAGS
       "${DEFAULT_C_FLAGS_INIT} "
       "-m${BUILD_BITNESS} "
       "-march=westmere "
       "-fno-strict-aliasing "
       "-fdiagnostics-show-option "
      )

if(BUILD_BITNESS EQUAL "32")
    string(CONCAT DEFAULT_CXX_FLAGS
           "${DEFAULT_CXX_FLAGS} "
           "-mstackrealign "
          )
    string(CONCAT DEFAULT_C_FLAGS
           "${DEFAULT_C_FLAGS} "
           "-mstackrealign "
          )
endif()

set(CMAKE_CXX_FLAGS ${DEFAULT_CXX_FLAGS} CACHE STRING "Default" FORCE)
set(CMAKE_C_FLAGS   ${DEFAULT_C_FLAGS}   CACHE STRING "Default" FORCE)


set(CMAKE_CXX_FLAGS_RELEASE         "-O2 -DNDEBUG"
    CACHE STRING "Release"        FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-O2 -DNDEBUG"
    CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g -DNDEBUG"
    CACHE STRING "RelWithDebInfo" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG           "-g"
    CACHE STRING "Debug"          FORCE)

set(CMAKE_C_FLAGS_RELEASE           "-O2 -DNDEBUG"
    CACHE STRING "Release"        FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL        "-O2 -DNDEBUG"
    CACHE STRING "MinSizeRel"     FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g -DNDEBUG"
    CACHE STRING "RelWithDebInfo" FORCE)
set(CMAKE_C_FLAGS_DEBUG             "-g"
    CACHE STRING "Debug"          FORCE)

# Disable GNU c++ extensions.
set(CMAKE_CXX_EXTENSIONS OFF)
