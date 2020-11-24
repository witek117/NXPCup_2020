set(FORMATTING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

function(formatting_support)  
    add_clang_format()

    find_program(CLANG clang-format)

    if(NOT CLANG)
        message(WARNING "clang-format executable was not found!")
    endif()

endfunction()


function(add_clang_format)
    set(CLANG_FORMAT_CONFIG "${FORMATTING_DIRECTORY}/.clang-format.config")
    set(CLANG_FORMAT_FILE "${CMAKE_SOURCE_DIR}/.clang-format")

    if(NOT EXISTS ${CLANG_FORMAT_FILE})
        configure_file(${CLANG_FORMAT_CONFIG} ${CLANG_FORMAT_FILE} COPYONLY)
    # else()
    #     # .clang_format file already in root directory. Compare it with bob's source
    #     execute_process( COMMAND ${CMAKE_COMMAND} -E compare_files ${BOB_CLANG_FORMAT_FILE} ${CLANG_FORMAT_FILE} RESULT_VARIABLE COMPARE_RESULT )
    #     if( COMPARE_RESULT EQUAL 0)
    #         message(VERBOSE ".clang-format file in project root directory matches bob's source")
    #     elseif( COMPARE_RESULT EQUAL 1)
    #         if(NOT EXISTS ${CLANG_FORMAT_FILE_ALT})
    #             message(WARNING "Different .clang-format file already exists in top level directory. Bob's version will be stored as .clang-format_bob.")
    #             configure_file(${BOB_CLANG_FORMAT_FILE} ${CLANG_FORMAT_FILE_ALT} COPYONLY)
    #         endif()
    #     else()
    #         message(FATAL_ERROR "Error while comparing the files.")
    #     endif()
    endif()
endfunction()