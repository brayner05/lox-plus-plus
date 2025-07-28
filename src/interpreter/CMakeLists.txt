# interpreter/CMakeLists.txt
file(GLOB_RECURSE INTERPRETER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(interpreter STATIC ${INTERPRETER_SOURCES})

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
