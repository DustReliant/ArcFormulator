#include "Point3D.h"

Point3D::Point3D() 
    : m_x(0)
    , m_y(0)
    , m_z(0)
{
}

Point3D::Point3D(double x, double y, double z) 
    : m_x(x)
    , m_y(y)
    , m_z(z)
{
}

Point3D::Point3D(const Point3D &other) 
    : m_x(other.m_x)
    , m_y(other.m_y)
    , m_z(other.m_z)
{
}

Point3D::Point3D(Point3D &&other) 
    : m_x(other.m_x)
    , m_y(other.m_y)
    , m_z(other.m_z)
{
    other.m_x = 0;
    other.m_y = 0;
    other.m_z = 0;
}

Point3D::Point3D(double x, double y) 
    : m_x(x)
    , m_y(y)
    , m_z(0)
{
}

Point3D::~Point3D()
{
}

Point3D &Point3D::operator=(const Point3D &other)
{
    // 检查自我赋值
    if (this == &other)
    {
        return *this;
    }

    // 复制数据（在这种情况下，非常直接）
    this->m_x = other.m_x;
    this->m_y = other.m_y;
    this->m_z = other.m_z;

    return *this;
}


double Point3D::getX() const
{
    return m_x;
}

double Point3D::getY() const
{
    return m_y;
}

double Point3D::getZ() const
{
    return m_z;
}

void Point3D::setX(double x)
{
    m_x = x;
}

void Point3D::setY(double y)
{
    m_y = y;
}

void Point3D::setZ(double z)
{
    m_z = z;
}

double Point3D::distanceTo(const Point3D &other) const
{
    double dx = m_x - other.m_x;
    double dy = m_y - other.m_y;
    double dz = m_z - other.m_z;

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
