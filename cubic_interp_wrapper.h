#pragma once
#include "spline.h"
#include <tuple>
#include <QPointF>

#define SQUARE(x) std::pow((x),2)

class TwoPointInterpolation
{
public:
    TwoPointInterpolation(const QPointF start, const QPointF end, const double left_slope = 0.0, const double right_slope = 0.0);

    double get_interp_value(const double pos) const;
    double get_slope_value(const double pos) const;
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
};

//Interpolate monotonic intervals
class InterpolationWrapper
{
public:
    InterpolationWrapper(std::vector<QPointF> pos_list, const double left_slope = 0.0, const double right_slope = 0.0, double interp_step = 1.0);
    ~InterpolationWrapper();

    std::vector<QPointF> getInterpPosList();
    double get_slope(const double pos) const;

private:
    void generateCalcualtor();

private:
    std::vector<QPointF> origin_pos_list_;
    std::vector<QPointF> interp_pos_list_;
    double left_slope_;
    double right_slope_;
    double step_;

    tk::spline* spline_calculator_;
    TwoPointInterpolation* two_point_calculator_;
};

class MonotonicHelper
{
public:
    enum Type
    {
        None = -1,
        Descending = 1,
        YIndependent = 2,//Y is the independent variable
    };

    //Make sure there are at least two points
    static std::vector<std::tuple<int /*origin monotonic*/, std::vector<QPointF>>> makeMonotonic(const std::vector<QPointF>& origin_pos_list, const int start_index);
    static int twoPointMonoType(const QPointF start, const QPointF end);

    static std::vector<QPointF> convertMonotonic(const std::vector<QPointF>& origin_data_list, const int mono_type);

    static std::tuple<double /*left*/, double /*right*/> convertSlope(const double left_slope,const double right_slope, const int mono_type);

    static void removeAdjustRepeatPoint(std::vector<QPointF>& pos_list, const int begin_index);
};

void testMonotonicHelper();