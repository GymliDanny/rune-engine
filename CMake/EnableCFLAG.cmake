include(CheckCCompilerFlag)

function(enable_c_compiler_flag_if_supported flag)
        string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
        if (flag_already_set EQUAL -1)
                check_c_compiler_flag("${flag}" flag_supported)
                if (flag_supported)
                        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
                endif ()
                unset(flag_supported CACHE)
        endif ()
endfunction ()
