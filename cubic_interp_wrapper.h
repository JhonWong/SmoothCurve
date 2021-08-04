#pragma once
#include "spline.h"
#include <tuple>
#include <QPointF>

#define SQUARE(x) std::pow((x),2)

class TwoPointInterpolation
{
public:
    TwoPointInterpolation(const QPointF start, const QPointF end, const double left_slope, const double right_slope/*,const double left_curvature,const double right_curvature*/);

    double get_interp_value(const double value) const;
private:
    void configFunc();

private:
    QPointF start_;
    QPointF end_;

    //y = ax^3 + bx^2 + cx + d
    double a_;
    double b_;
    double c_;
    double d_;

    double left_slope_bound_;
    double right_slope_bound_;

    //double left_curvature_bound_;
    //double right_curvature_bound_;;
};