# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ArcFormulator" VERSION 1.0)

# 开启C++11支持
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 编译选项
option(ENABLE_TESTS "Enable test compilation and execution" OFF)

# 全局编译标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(${CMAKE_SOURCE_DIR}/3dparty/SARibbon/include)
include_directories(${CMAKE_SOURCE_DIR}/3dparty/pugixml/include)

# 添加子目录
add_subdirectory(${CMAKE_SOURCE_DIR}/src/ArcCore)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/ui)

# 包含生成动态库的输出目录
#link_directories(${CMAKE_BINARY_DIR}/src/core)

# 添加可执行文件
#add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

# 指定DLL的源目录和目标目录
set(SARIBBON_DLL_DIR "${CMAKE_SOURCE_DIR}/3dparty")
set(TARGET_DEBUG_DIR "${CMAKE_SOURCE_DIR}/out/build/x64-debug/src/ui")
set(TARGET_RELEASE_DIR "${CMAKE_SOURCE_DIR}/out/build/x64-release/src/ui")

# 使用file命令搜索所有DLL文件
file(GLOB_RECURSE ALL_DLLS "${SARIBBON_DLL_DIR}/*.dll")
message(STATUS "0000000000000000000000000Found DLLs: ${ALL_DLLS}")

# 使用循环来复制每个DLL文件
foreach(DLL_PATH ${ALL_DLLS})
  file(COPY ${DLL_PATH} DESTINATION ${TARGET_DEBUG_DIR})
  file(COPY ${DLL_PATH} DESTINATION ${TARGET_RELEASE_DIR})
endforeach()

