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

link_directories(${CMAKE_SOURCE_DIR}/3dparty/SARibbon/lib)
link_directories(${CMAKE_SOURCE_DIR}/3dparty/pugixml/lib)


# 获取view目录下的所有.cpp和.h文件
file(GLOB_RECURSE VIEW_CPP_FILES "view/*.cpp")
file(GLOB_RECURSE VIEW_HEADER_FILES "view/*.h")

# 获取ribbon目录下的所有.cpp和.h文件
file(GLOB_RECURSE RIBBON_CPP_FILES "ribbon/*.cpp")
file(GLOB_RECURSE RIBBON_HEADER_FILES "ribbon/*.h")
file(GLOB_RECURSE RIBBON_UI_FILES "ribbon/*.ui")

# 合并所有获取到的文件列表
set(ALL_CPP_FILES ${VIEW_CPP_FILES} ${RIBBON_CPP_FILES})
set(ALL_HEADER_FILES ${VIEW_HEADER_FILES} ${RIBBON_HEADER_FILES})
set(ALL_UI_FILES ${RIBBON_UI_FILES})

# 创建一个可执行文件
message("${CMAKE_PROJECT_NAME}")
add_executable(${PROJECT_NAME} WIN32
    "main.cpp"
    "MainWindow.cpp"
    "MainWindow.h"
    #"MainWindow.ui"
    ${ALL_CPP_FILES} 
    ${ALL_HEADER_FILES}
    ${ALL_UI_FILES}
)

# 链接Qt5库到你的项目
# 搜索Qt的库。请确保设置了Qt的路径，或者Qt已安装在标准路径中
find_package(Qt5 COMPONENTS Core Gui Widgets Sql)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
)

# 根据需要，可以设置C++标准
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()



if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SARibbonBard
        pugixmld
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        SARibbonBar
        pugixml
    )
endif()

# TODO: 如有需要，请添加测试并安装目标。
