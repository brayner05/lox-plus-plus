# interpreter/CMakeLists.txt
file(GLOB_RECURSE PARSER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(parser STATIC ${PARSER_SOURCES})

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
