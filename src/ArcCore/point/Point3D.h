//*****************************************************************************//
//                  _____                                                      //
//                 / ____|                     /\                              //
//                | |  __   ___  _ __ ___     /  \    _ __  ___                //
//                | | |_ | / _ \| '_ ` _ \   / /\ \  | '__|/ __|               //
//                | |__| ||  __/| | | | | | / ____ \ | |  | (__                //
//                 \_____| \___||_| |_| |_|/_/    \_\|_|   \___|               //
//                                                                             //
//                                                                             //
//                                                                             //
//                                                                             //
//                                                                             //
//    文件名称：Point3D                                                          //
//    作   者：Dust                                                             //
//    时   间：2023年8月23日                                                     //
//                                                                             //
//*****************************************************************************//


#ifndef POINT3D_H
#define POINT3D_H

#include <cmath>
#include <ranges>

class Point3D
{
public:
    // 默认构造函数：初始化点为原点（0,0,0）
	Point3D();
    // 三参数构造函数：根据给定的 x、y 和 z 初始化点
    Point3D(double x, double y, double z);
    // 拷贝构造函数
    Point3D(const Point3D &other);
    // 移动构造函数
    Point3D(Point3D &&other);
    // 双参数构造函数：初始化 x 和 y 坐标，z 坐标默认为 0
    Point3D(double x, double y); 
    // 析构函数
    ~Point3D();
    
    // 赋值操作符
    Point3D &operator=(const Point3D &other);
    
    // 获取 x 坐标
    double getX() const;
    // 获取 y 坐标
    double getY() const;
    // 获取 z 坐标
    double getZ() const;

    // 设置 x 坐标
    void setX(double x);
    // 设置 y 坐标
    void setY(double y);
    // 设置 z 坐标
    void setZ(double z);

    // 计算到另一个点的距离
    double distanceTo(const Point3D &other) const;

private:
    double m_x;
    double m_y;
    double m_z;
};
#endif //POINT3D_H