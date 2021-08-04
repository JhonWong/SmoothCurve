#include "cubic_interp_wrapper.h"

TwoPointInterpolation::TwoPointInterpolation(const QPointF start, const QPointF end, const double left_slope, const double right_slope)
    :left_slope_bound_(0.0)
    , right_slope_bound_(0.0)
{
    assert(start.x() < end.x());

    start_ = start;
    end_ = end;

    left_slope_bound_ = left_slope;
    right_slope_bound_ = right_slope;
    configFunc();
}

double TwoPointInterpolation::get_interp_value(const double pos) const
{
    auto ret = a_ * std::pow(pos, 3) + b_ * SQUARE(pos) + c_ * pos + d_;
    return ret;
}

double TwoPointInterpolation::get_slope_value(const double pos) const
{
    auto ret = 3 * a_*SQUARE(pos) + 2.0*b_*pos + c_;
    return ret;
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
    ba_b_factor = (right_slope_bound_ - left_slope_bound_) / (2.0 * (x2 - x1));

    double tmp_factor = x1 / x2;
    double ca_k_factor = 0.0;
    double ca_b_factor = 0.0;
    ca_k_factor = (-3.0 * (tmp_factor * SQUARE(x2) - SQUARE(x1))) / (tmp_factor - 1);
    ca_b_factor = (tmp_factor * right_slope_bound_ - left_slope_bound_) / (tmp_factor - 1);

    a_ = (y2 - y1 - ba_b_factor * (SQUARE(x2) - SQUARE(x1)) + ca_b_factor * (x1 - x2)) / (std::pow(x2, 3) - std::pow(x1, 3) + ba_k_factor * (SQUARE(x2) - SQUARE(x1)) + ca_k_factor * (x2 - x1));
    b_ = ba_k_factor * a_ + ba_b_factor;
    c_ = ca_k_factor * a_ + ca_b_factor;
    d_ = y1 - a_ * std::pow(x1, 3) - b_ * SQUARE(x1) - c_ * (x1);
}

InterpolationWrapper::InterpolationWrapper(std::vector<QPointF> pos_list, const double left_slope, const double right_slope, double interp_step /*= 1.0*/)
    :origin_pos_list_(pos_list)
    , step_(1.0)
    , left_slope_(left_slope)
    , right_slope_(right_slope)
    , spline_calculator_(nullptr)
    , two_point_calculator_(nullptr)
{
    assert(interp_step > 0);
    step_ = interp_step;

    assert(pos_list.size() > 1);
    for (int i = 0; i < pos_list.size() - 1; i++)
    {
        assert(pos_list[i + 1].x() > pos_list[i].x());
    }

    origin_pos_list_ = pos_list;
    generateCalcualtor();
}

InterpolationWrapper::~InterpolationWrapper()
{
    if (spline_calculator_) delete spline_calculator_;
    if (two_point_calculator_) delete two_point_calculator_;
}

std::vector<QPointF> InterpolationWrapper::getInterpPosList()
{
    interp_pos_list_.clear();

    for (int i = 0; i < origin_pos_list_.size() - 1; i++)
    {
        const auto pos = origin_pos_list_[i];
        interp_pos_list_.push_back(pos);

        const auto next_pos = origin_pos_list_[i + 1];
        for (float x_pos = pos.x() + step_; x_pos < next_pos.x(); x_pos += step_)
        {
            double y_pos = 0.0;
            if (two_point_calculator_)
            {
                y_pos = two_point_calculator_->get_interp_value(x_pos);
            }
            else
            {
                y_pos = two_point_calculator_->get_interp_value(x_pos);
            }

            interp_pos_list_.push_back(QPointF(x_pos, y_pos));
        }
    }
    interp_pos_list_.push_back(origin_pos_list_.back());

    return interp_pos_list_;
}

double InterpolationWrapper::get_slope(const double pos) const
{
    if (spline_calculator_)
    {
        return spline_calculator_->deriv(tk::spline::bd_type::first_deriv, pos);
    }

    return two_point_calculator_->get_slope_value(pos);
}

void InterpolationWrapper::generateCalcualtor()
{
    if (origin_pos_list_.size() == 2)
    {
        two_point_calculator_ = new TwoPointInterpolation(origin_pos_list_[0], origin_pos_list_[1], left_slope_, right_slope_);
    }
    else
    {
        std::vector<double> x_pos_list;
        std::vector<double> y_pos_list;
        for (auto pos : origin_pos_list_)
        {
            x_pos_list.push_back(pos.x());
            y_pos_list.push_back(pos.y());
        }

        spline_calculator_ = new tk::spline(x_pos_list, y_pos_list, tk::spline::cspline, false, tk::spline::bd_type::first_deriv, left_slope_, tk::spline::bd_type::first_deriv, right_slope_);
    }
}
