#ifndef POINT2D_H
#define POINT2D_H

#include <cmath>
#include <iostream>

class Point2D 
{
public:
    // 默认构造函数，将点初始化为原点 (0, 0)
    Point2D() : m_x(0), m_y(0){}

    // 参数构造
    Point2D(double x, double y);

    // 拷贝构造
    Point2D(const Point2D &point2d);

    // 设置点的x坐标
    void setX(double x);

    // 设置点的y坐标
    void setY(double y);

    // 获取点的x坐标
    double getX() const;

    // 获取点的y坐标
    double getY() const;

    // 计算与另一点的距离
    double distanceTo(const Point2D &other) const;

    // 输出点的坐标
    void print() const;

private:
    double m_x;
    double m_y;
};

#endif // POINT2D_H