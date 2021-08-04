#include "cubic_interp_wrapper.h"

TwoPointInterpolation::TwoPointInterpolation(const QPointF start, const QPointF end, const double left_slope, const double right_slope/*, const double left_curvature, const double right_curvature*/)
    :left_slope_bound_(0.0)
    , right_slope_bound_(0.0)
    //, left_curvature_bound_(left_curvature)
    //, right_curvature_bound_(right_curvature)
{
    assert(start.x() < end.x());

    start_ = start;
    end_ = end;

    left_slope_bound_ = left_slope;
    right_slope_bound_ = right_slope;
    configFunc();
}

double TwoPointInterpolation::get_interp_value(const double value) const
{

}

void TwoPointInterpolation::configFunc()
{
    const double x1 = start_.x();
    const double y1 = start_.y();
    const double x2 = end_.x();
    const double y2 = end_.y();

    double ba_k_factor = 0.0;
    double ba_b_factor = 0.0; // b = ba_k_factor * a + ba_b_factor
    ba_k_factor = (-3.0 * (SQUARE(x2) - SQUARE(x1))) / (2.0 * (x2 - x1));
    ba_b_factor = (SQUARE(y2) - SQUARE(y1)) / (2.0 * (x2 - x1));

    double tmp_factor = x1 / x2;
    double ca_k_factor = 0.0;
    double ca_b_factor = 0.0;
    ca_k_factor = (tmp_factor * right_slope_bound_ - left_slope_bound_) / (tmp_factor - 1);
    ca_b_factor = (-3.0 * (tmp_factor * SQUARE(x2) - SQUARE(x1))) / (tmp_factor - 1);

    a_ = (y2 - y1) / (std::pow(x2,3) - std::pow(x1,3));
}
