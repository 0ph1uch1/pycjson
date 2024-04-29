cmake_minimum_required(VERSION 3.12)

project(pycjson C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

find_package(Python3 COMPONENTS Development)

set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps)
file(GLOB_RECURSE SRC_C_FILES ${SRC_PATH}/*.c)
file(GLOB_RECURSE DEPS_CPP_FILES ${DEPS_PATH}/*.cpp)

add_library(cjson SHARED ${SRC_C_FILES} ${DEPS_CPP_FILES})
target_include_directories(cjson PUBLIC ${SRC_PATH} ${DEPS_PATH} ${Python3_INCLUDE_DIRS})
target_link_libraries(cjson ${Python3_LIBRARIES})
set_target_properties(cjson PROPERTIES PREFIX "")
set_target_properties(cjson PROPERTIES LINKER_LANGUAGE C)
