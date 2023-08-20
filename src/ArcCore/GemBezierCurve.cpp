#include "GemBezierCurve.h"
#include <cassert>

GemBezierCurve::GemBezierCurve()
{
}

GemBezierCurve::~GemBezierCurve()
{
}

void GemBezierCurve::SetCtrlPoint(Point2D& stPt)
{
    m_vecCtrlPt.push_back(stPt);
}

bool GemBezierCurve::CreateCurve()
{
    // 确保是二次曲线，2个顶点一个控制点
    assert(m_vecCtrlPt.size() == 3);

    // t的增量， 可以通过setp大小确定需要保存的曲线上点的个数
    float step = 0.01;
    for (float t = 0.0; t <= 1.0; t += step)
        {
            Point2D stPt;
            CalCurvePoint(t, stPt);
            m_vecCurvePt.push_back(stPt);
        }
    return false;
}

void GemBezierCurve::CalCurvePoint(float t, Point2D& stPt)
{
    // 确保是二次曲线，2个顶点一个控制点
    assert(m_vecCtrlPt.size() == 3);

    // 计算曲线点坐标，此为2次算法，改变此处可以实现多次曲线
    //float x = (float) m_vecCtrlPt[0].x * pow(1 - t, 2) +
    //          (float) m_vecCtrlPt[1].x * t * (1 - t) * 2 +
    //          (float) m_vecCtrlPt[2].x * pow(t, 2);
    //float y = (float) m_vecCtrlPt[0].y * pow(1 - t, 2) +
    //          (float) m_vecCtrlPt[1].y * t * (1 - t) * 2 +
    //          (float) m_vecCtrlPt[2].y * pow(t, 2);

    float x = 0, y = 0;

    stPt.setX(x);
    stPt.setY(y);
}
