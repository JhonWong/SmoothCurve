#include "smooth_curve_graph.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QTime>
#include "random_points_data.h"
#include "spline.h"
#include "bezier/bezier.h"
#include "shader_point_node.h"

SmoothCurve::SmoothCurve()
    :current_pos_index_(0)
    , last_pre_slope_(SLOPE_DEFAULT)
    , point_node_(nullptr)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj, &RandomPointsData::sigDataChanged, this, &SmoothCurve::calculateSmoothPoints);
}

QSGNode* SmoothCurve::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* _data)
{
    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;

    //curve node
    const int point_count = smooth_data_list_.size();
    unsigned int data_size = std::max(point_count, 1);
    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), data_size);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(QColor(0, 0, 255));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }
    else {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        geometry->allocate(data_size);
    }

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0; i < point_count; i++)
    {
        auto& pos = smooth_data_list_[i];
        vertices[i].set(pos.x(), pos.y());
    }

    node->markDirty(QSGNode::DirtyGeometry);

    //point node
    if (!point_node_)
    {
        point_node_ = new CustomPointNode(10, QColor(Qt::red));
        node->appendChildNode(point_node_);
    }
    auto& origin_data_list = RandomPointsData::getInstance().get_pos_list();
    point_node_->updateGeomety(origin_data_list);

    return node;
}

void SmoothCurve::calculateSmoothPoints()
{
    const auto& origin_pos_list = RandomPointsData::getInstance().get_pos_list();
    if (current_pos_index_ == ((int)origin_pos_list.size() - 1)) return;

    for (int i = current_pos_index_; i < origin_pos_list.size() - 1; i++)
    {
        bool topleft = i % 2 == 0;
        auto interp_pos_list = bezierInterp(origin_pos_list[i], origin_pos_list[i + 1], topleft);

        smooth_data_list_.insert(smooth_data_list_.end(), interp_pos_list.begin(), interp_pos_list.end());
    }
    current_pos_index_ = origin_pos_list.size() - 1;

    //=========================================
/*
    auto old_smooth_size = smooth_data_list_.size();

    //interpolation with last interval
    if (current_pos_index_ > 0)
    {
        std::vector<QPointF> pos_list = { origin_pos_list[current_pos_index_ - 1],origin_pos_list[current_pos_index_] };
        auto mono_list_bt = MonotonicHelper::makeMonotonic(pos_list, 0);

        double last_slope = SLOPE_DEFAULT;
        auto interp_list = interpSingleMono(mono_list_bt[0], last_pre_slope_, last_slope);
        smooth_data_list_.insert(smooth_data_list_.end(), interp_list.begin(), interp_list.end());

        last_pre_slope_ = last_slope;
    }

    //interpolation current interval
    //make vector monotonic
    auto mono_vec = MonotonicHelper::makeMonotonic(origin_pos_list, current_pos_index_);
    for (int i = 0; i < mono_vec.size(); i++)
    {
        double last_slope = SLOPE_DEFAULT;
        auto interp_list = interpSingleMono(mono_vec[i], last_pre_slope_, last_slope);
        smooth_data_list_.insert(smooth_data_list_.end(), interp_list.begin(), interp_list.end());

        last_pre_slope_ = last_slope;
    }
    current_pos_index_ = origin_pos_list.size();

    //MonotonicHelper::removeAdjustRepeatPoint(smooth_data_list_, old_smooth_size);*/

    this->update();
}

std::vector<QPointF> SmoothCurve::interpSingleMono(const std::tuple<int, std::vector<QPointF>> mono_inter, const double left_slope_bound, double& slope_last_origin_pos)
{
    auto mono_type = std::get<0>(mono_inter);
    auto pos_list = std::get<1>(mono_inter);

    auto slope_bound = MonotonicHelper::convertSlope(left_slope_bound, SLOPE_DEFAULT, mono_type);

    InterpolationWrapper interp(pos_list, std::get<0>(slope_bound), std::get<1>(slope_bound));
    auto interp_list = interp.getInterpPosList();
    interp_list = MonotonicHelper::convertMonotonic(interp_list, mono_type);

    auto origin_pos_list = MonotonicHelper::convertMonotonic(pos_list, mono_type);
    auto last_pos = MonotonicHelper::convertMonotonic({ origin_pos_list.back() }, mono_type);

    auto slope = interp.get_slope(last_pos[0].x());
    auto new_bound_slope = MonotonicHelper::convertSlope(slope, 0.0, mono_type & 0b10/*Remove ascending descending order*/);
    slope_last_origin_pos = std::get<0>(new_bound_slope);

    return interp_list;
}

const double CONTROL_POINT_DISS = 100;
std::vector<QPointF> SmoothCurve::bezierInterp(const QPointF start, const QPointF end, const bool topleft)
{
    std::vector<Bezier::Vec2> pos_list;
    pos_list.push_back({ (float)start.x(),(float)start.y() });

    auto p1 = getControlPoint(start, SLOPE_DEFAULT, CONTROL_POINT_DISS, topleft);
    pos_list.push_back({ (float)p1.x(),(float)p1.y() });

    auto p2 = getControlPoint(end, SLOPE_DEFAULT, CONTROL_POINT_DISS, topleft);
    pos_list.push_back({ (float)p2.x(),(float)p2.y() });

    pos_list.push_back({ (float)end.x(),(float)end.y() });

    Bezier::Bezier<3> cubicBezier(pos_list);
    std::vector<QPointF> interp_list;

    auto distance = std::floor(std::abs(end.x() - start.x()));
    for (int i = 0; i < distance; i++)
    {
        auto t = i * (1.0 / distance);
        auto pos = cubicBezier.valueAt(t);
        interp_list.push_back(QPointF(pos.x, pos.y));
    }
    interp_list.push_back(end);

    return interp_list;
}

QPointF SmoothCurve::getControlPoint(QPointF another_pos, const double k, const double distance, const double topleft)
{
    auto b = another_pos.y() - k * another_pos.x();
    auto x_dis = std::sqrt((std::pow(distance, 2) / (1 + k)));
    if (!topleft)
    {
        x_dis = -x_dis;
    }

    auto x_pos = another_pos.x() + x_dis;
    auto y_pos = k * x_pos + b;

    return QPointF(x_pos, y_pos);
}
