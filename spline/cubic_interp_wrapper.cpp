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
    auto ret = 3 * a_ * SQUARE(pos) + 2.0 * b_ * pos + c_;
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
                y_pos = (*spline_calculator_)(x_pos);
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

std::vector<std::tuple<int, std::vector<QPointF>>> MonotonicHelper::makeMonotonic(const std::vector<QPointF>& origin_pos_list, const int start_index)
{
    assert(origin_pos_list.size() > 1);

    std::vector<QPointF> target_pos_list(origin_pos_list.begin() + start_index, origin_pos_list.end());
    removeAdjustRepeatPoint(target_pos_list, 0);

    std::vector<std::tuple<int, std::vector<QPointF>>> mono_list;

    int last_type = None;
    int interval_start_index = 0;
    for (int i = 0; i < target_pos_list.size() - 1; i++)
    {
        auto current_type = twoPointMonoType(target_pos_list[i], target_pos_list[i + 1]);
        assert(current_type != None);
        if (last_type == None) last_type = current_type;

        if (current_type != last_type)
        {
            assert(i > interval_start_index);

            std::vector<QPointF> current_list(target_pos_list.begin() + interval_start_index, target_pos_list.begin() + i + 1);
            current_list = convertMonotonic(current_list, last_type);
            mono_list.push_back(std::make_tuple(last_type, current_list));

            interval_start_index = i;
        }

        last_type = current_type;
    }

    //push last interval
    std::vector<QPointF> current_list(target_pos_list.begin() + interval_start_index, target_pos_list.end());
    current_list = convertMonotonic(current_list, last_type);
    mono_list.push_back(std::make_tuple(last_type, current_list));

    return mono_list;
}

int MonotonicHelper::twoPointMonoType(const QPointF start, const QPointF end)
{
    if (start.x() < end.x())
    {
        return 0;
    }
    else if (start.x() > end.x())
    {
        return Descending;
    }
    else
    {
        if (start.y() < end.y())
        {
            return YIndependent;
        }
        else if (start.y() > end.y())
        {
            return YIndependent | Descending;
        }
    }

    return None;
}

std::vector<QPointF> MonotonicHelper::convertMonotonic(const std::vector<QPointF>& origin_data_list, const int mono_type)
{
    std::vector<QPointF> convert_vec = origin_data_list;

    if (mono_type & Descending)
    {
        for (int i = 0; i < origin_data_list.size() / 2; i++)
        {
            std::swap(convert_vec[i], convert_vec[origin_data_list.size() - i - 1]);
        }
    }

    if (mono_type & YIndependent)
    {
        for (int i = 0; i < convert_vec.size(); i++)
        {
            auto tmp_value = convert_vec[i];
            convert_vec[i].setX(tmp_value.y());
            convert_vec[i].setY(tmp_value.x());
        }
    }

    return convert_vec;
}

std::tuple<double, double> MonotonicHelper::convertSlope(const double left_slope, const double right_slope, const int mono_type)
{
    auto tmp_left = left_slope;
    auto tmp_right = right_slope;

    //left right convert
    if (mono_type & Descending)
    {
        std::swap(tmp_left, tmp_right);
    }

    if (mono_type & YIndependent)
    {
        if (std::abs(tmp_left) > 1e-5)
        {
            tmp_left = 1.0 / tmp_left;
        }

        if (std::abs(tmp_right) > 1e-5)
        {
            tmp_right = 1.0 / tmp_right;
        }
    }
    
    return std::make_tuple(tmp_left, tmp_right);
}

void MonotonicHelper::removeAdjustRepeatPoint(std::vector<QPointF>& pos_list, const int begin_index)
{
    const double repeat_precision = 0.1;
    for (int i = 0; i < pos_list.size() - 1; i++)
    {
        auto current_pos = pos_list[i];
        auto next_pos = pos_list[i + 1];

        if (std::abs(current_pos.x() - next_pos.x()) < repeat_precision
            && std::abs(current_pos.y() - next_pos.y()) < repeat_precision)
        {
            pos_list.erase(pos_list.begin() + i);
            i--;
        }
    }
}

void testMonotonicHelper()
{
    //srand((unsigned)time(0));

    std::vector<QPointF> pos_list;
    pos_list.push_back(QPointF(44, 0));
    pos_list.push_back(QPointF(87, 94));
    pos_list.push_back(QPointF(87, 20));
    pos_list.push_back(QPointF(46, 93));
    pos_list.push_back(QPointF(89, 142));
    pos_list.push_back(QPointF(198, 13));
    //for (int i = 0; i < 5; i++)
    //{
    //    pos_list.push_back(QPointF(rand() % 200, rand() % 200));
    //}

    auto mono_vec = MonotonicHelper::makeMonotonic(pos_list, 0);

    for (int i = 0; i < mono_vec.size(); i++)
    {
        auto type = std::get<0>(mono_vec[i]);
        auto mono = std::get<1>(mono_vec[i]);
        auto origin = MonotonicHelper::convertMonotonic(mono, type);
        auto mono1 = MonotonicHelper::convertMonotonic(origin, type);

        int a = 10;
        a++;
    }

    double left_slope = 2.0, right_slope = 0.0;
    for (int type = 0; type < 7; type++)
    {
        auto tmp_slope = MonotonicHelper::convertSlope(left_slope, right_slope, type);

        int a = 10;
        a++;
    }

    int a = 10;
    a++;
}
