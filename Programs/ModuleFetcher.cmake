# Copyright mzoesch. All rights reserved.

foreach(search_path ${JAFG_SEARCH_PATHS})
    message(VERBOSE "Searching for modules in [${search_path}] ...")

    file(GLOB children_flat RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${search_path}/CMakeLists.txt")
    foreach(child ${children_flat})
        get_filename_component(child_dir "${child}" DIRECTORY)

        set(exclude_found FALSE)
        foreach(exclude_path ${JAFG_EXCLUDE_PATHS})
            if(child_dir MATCHES "^${exclude_path}")
                set(exclude_found TRUE)
                break()
            endif()
        endforeach()

        if(exclude_found)
            message(STATUS "Skipping module at [${child_dir}] due to exclusion.")
            continue()
        endif()

        message(STATUS "Found module at [${child_dir}].")
        add_subdirectory("${child_dir}")
    endforeach()

    file(GLOB_RECURSE children_recursive RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${search_path}/*/CMakeLists.txt")
    foreach(child ${children_recursive})
        get_filename_component(child_dir "${child}" DIRECTORY)

        set(exclude_found FALSE)
        foreach(exclude_path ${JAFG_EXCLUDE_PATHS})
            if(child_dir MATCHES "^${exclude_path}")
                set(exclude_found TRUE)
                break()
            endif()
        endforeach()

        if(exclude_found)
            message(STATUS "Skipping module at [${child_dir}] due to exclusion.")
            continue()
        endif()

        message(STATUS "Found module at [${child_dir}].")
        add_subdirectory("${child_dir}")
    endforeach()
endforeach()
