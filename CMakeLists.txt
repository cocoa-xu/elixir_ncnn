cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(elixir_ncnn)

find_package(ncnn REQUIRED PATHS "${PRIV_DIR}/" NO_DEFAULT_PATH)
file(GLOB elixir_ncnn_sources CONFIGURE_DEPENDS "${C_SRC}/*.c" "${C_SRC}/*.cpp" "${C_SRC}/*.hpp" "${C_SRC}/*.h")
add_executable(elixir_ncnn.so ${elixir_ncnn_sources})
include_directories("${PRIV_DIR}/include")
target_link_libraries(elixir_ncnn.so "ncnn")

set_property(TARGET elixir_ncnn.so PROPERTY CXX_STANDARD 14)

set_target_properties(elixir_ncnn.so PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared")
    set_target_properties(elixir_ncnn.so PROPERTIES INSTALL_RPATH "\$ORIGIN/lib")
else()
    # Although the compiler complains about not using these,
    # things only work with them set
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup -dynamiclib")
    set(CMAKE_SHARED_LINKER_FLAGS "-bundle -flat_namespace -undefined suppress")
    set_target_properties(ncnn.so PROPERTIES INSTALL_RPATH "@loader_path/lib")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -I${ERTS_INCLUDE_DIR} -O3 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
