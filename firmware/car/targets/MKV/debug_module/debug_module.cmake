add_library(debug_module STATIC
                            ${CMAKE_CURRENT_LIST_DIR}/printf.c
                            ${CMAKE_CURRENT_LIST_DIR}/ring_buffer.c
                            ${CMAKE_CURRENT_LIST_DIR}/logger.cpp 
                            )
target_include_directories(debug_module PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(debug_module drivers)