add_library(sha204_library
        sha204_library.c
        sha204_library_swi.c
)

pico_generate_pio_header(sha204_library ${CMAKE_CURRENT_LIST_DIR}/sha204_library_swi.pio)

target_link_libraries(sha204_library PRIVATE
        pico_stdlib
        hardware_pio
)

target_include_directories(sha204_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/sha204_includes
)
