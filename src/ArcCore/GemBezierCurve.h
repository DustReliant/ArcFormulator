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
//    文件名称：GemBezierCurve                                                   //
//    作   者：Dust                                                             //
//    时   间：2023年8月20日                                                     //
//                                                                             //
//*****************************************************************************//


#ifndef GEMBEZIERCURVE_H
#define GEMBEZIERCURVE_H

#include <vector>
#include "Point2D.h"

class GemBezierCurve
{
public:
    GemBezierCurve();

    ~GemBezierCurve();

    void SetCtrlPoint(Point2D& stPt);

    bool CreateCurve();

private:
    // 主要算法，计算曲线各个点坐标
    void CalCurvePoint(double t, Point2D& stPt);

private:
    // 顶点和控制点数组
    std::vector<Point2D> m_vecCtrlPt;
    // 曲线上各点坐标数组
    std::vector<Point2D> m_vecCurvePt;
};
#endif //GEMBEZIERCURVE_H
