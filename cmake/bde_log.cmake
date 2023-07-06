include(bde_include_guard)
bde_include_guard()

set(BDE_LOG_LEVEL "NORMAL" CACHE STRING "Logging verbosity")
set_property(CACHE BDE_LOG_LEVEL PROPERTY STRINGS QUIET NORMAL VERBOSE VERY_VERBOSE)

set(BDE_LOG_LEVEL_QUIET 0)
set(BDE_LOG_LEVEL_NORMAL 1)
set(BDE_LOG_LEVEL_VERBOSE 2)
set(BDE_LOG_LEVEL_VERY_VERBOSE 3)

function(bde_log level)
    if (${BDE_LOG_LEVEL_${level}} LESS_EQUAL ${BDE_LOG_LEVEL_${BDE_LOG_LEVEL}})
        message(STATUS "${ARGN}")
    endif()
endfunction()
