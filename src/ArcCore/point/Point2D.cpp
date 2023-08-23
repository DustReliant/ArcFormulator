#include "Point2D.h"

Point2D::Point2D(double x, double y) : m_x(x), m_y(y) 
{

}

Point2D::Point2D(const Point2D &point2d) 
{
    m_x = point2d.m_x;
    m_x = point2d.m_y;
}

Point2D::~Point2D()
{

}

void Point2D::setX(double x) {
    m_x = x;
}

void Point2D::setY(double y) 
{
    m_y = y;
}

double Point2D::getX() const 
{
    return m_x;
}

double Point2D::getY() const 
{
    return m_y;
}

double Point2D::distanceTo(const Point2D &other) const 
{
    double dx = m_x - other.m_x;
    double dy = m_y - other.m_x;
    return std::sqrt(dx * dx + dy * dy);
}

void Point2D::print() const 
{
    std::cout << "(" << m_x << ", " << m_y << ")" << std::endl;
}
