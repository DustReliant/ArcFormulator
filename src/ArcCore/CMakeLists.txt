# CMakeLists.txt in src/core directory
# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

project ("GeometryCore")

# 源文件列表
set(SOURCES
   "Point2D.h"
   "Point2D.cpp"
)

# 创建动态库
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES})

# 指定头文件的搜索路径
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

