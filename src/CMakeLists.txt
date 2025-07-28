# Recursively grab all .cpp files under src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Create the executable
add_executable(loxpp ${SOURCES})

# Include src/ for header files
target_include_directories(loxpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
