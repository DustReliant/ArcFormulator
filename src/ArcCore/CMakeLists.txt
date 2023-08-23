# CMakeLists.txt in src/core directory
# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.10)

project ("GeometryCore")

# 源文件列表
set(SOURCES
   #点
   "point/point.h"
   "point/Point2D.h"
   "point/Point2D.cpp"
   "point/Point3D.h"
   "point/Point3D.cpp"
   # 曲线
   "curve/curve.h"
   "curve/GemBezierCurve.h"
   "curve/GemBezierCurve.cpp"
   "curve/GemCircleCurve.h"
   "curve/GemCircleCurve.cpp"
   "curve/GemCosCurve.h"
   "curve/GemCosCurve.cpp"
   "curve/GemSinCurve.h"
   "curve/GemSinCurve.cpp"
   "curve/GemTanCurve.h"
   "curve/GemTanCurve.cpp"
   "curve/GemLineCurve.h"
   "curve/GemLineCurve.cpp"
   "curve/GemHyperbolaCurve.h"
   "curve/GemHyperbolaCurve.cpp"
   "curve/GemEllipseCurve.h"
   "curve/GemEllipseCurve.cpp"
)

# 创建动态库
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCES} "curve/curve.h")

# 指定头文件的搜索路径
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

