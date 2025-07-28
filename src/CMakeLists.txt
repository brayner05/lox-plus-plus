# src/CMakeLists.txt
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_subdirectory(interpreter)
add_subdirectory(parser)
add_subdirectory(scanner)

add_executable(loxpp ${SOURCES})

target_include_directories(loxpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(loxpp PRIVATE interpreter)
target_link_libraries(loxpp PRIVATE parser)
target_link_libraries(loxpp PRIVATE scanner)
