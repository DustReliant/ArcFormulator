# CMakeList.txt: CMake 项目，在此处包括源代码并定义项目特定的逻辑。
cmake_minimum_required (VERSION 3.8)

# 项目名称
project ("ArcFormUI")

# 设定C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 在这里启用Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设定头文件，同样需要根据自己的项目修改
set(HEADER_FILES
    #mainwindow.h
    # ... 其他.h文件 ...
)

# 设定.ui文件
set(UI_FILES
    #mainwindow.ui
    # ... 其他.ui文件 ...
)

# 如果有.qrc资源文件，添加资源文件
set(QRC_FILES
    # resources.qrc
    # ... 其他.qrc文件 ...
)

# 创建一个可执行文件
message("${CMAKE_PROJECT_NAME}0000000000000000000000000000")
add_executable(${PROJECT_NAME} WIN32
    "main.cpp"
    "MainWindow.cpp"
    "MainWindow.h"
    "MainWindow.ui"
)

# 链接Qt5库到你的项目
# 搜索Qt的库。请确保设置了Qt的路径，或者Qt已安装在标准路径中
find_package(Qt5 COMPONENTS Core Widgets)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt::Core
        Qt::Widgets
)

# 根据需要，可以设置C++标准
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
