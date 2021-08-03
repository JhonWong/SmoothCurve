#include "smooth_curve.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QTime>
#include "random_points_data.h"
#include "spline.h"

SmoothCurve::SmoothCurve()
    :last_pos_index_(-1)
    , last_pre_slope_(0)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj, &RandomPointsData::sigDataChanged, this, &SmoothCurve::calculateSmoothPoints);
}

QSGNode* SmoothCurve::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* _data)
{
    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;

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

    //auto& point_data_obj = RandomPointsData::getInstance();
    //const auto& pos_list = point_data_obj.get_pos_list();
    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0; i < point_count; i++)
    {
        auto& pos = smooth_data_list_[i];
        vertices[i].set(pos.x(), pos.y());
    }

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void SmoothCurve::calculateSmoothPoints()
{
    QTime tmp_timer;
    tmp_timer.start();

    const auto& origin_pos_list = RandomPointsData::getInstance().get_pos_list();

    //calculate smooth point in current interval
    double first_current_slope = 0.0;
    double last_current_slope = 0.0;
    std::vector<QPointF> current_inter_data;
    {
        //set origin data
        std::vector<double> x_pos_list;
        std::vector<double> y_pos_list;
        for (auto pos : origin_pos_list)
        {
            x_pos_list.push_back(pos.x());
            y_pos_list.push_back(pos.y());
        }
        tk::spline s_calculator(x_pos_list, y_pos_list);

        for (int i = last_pos_index_ + 1; i < origin_pos_list.size() - 1; i++)
        {
            const auto pos = origin_pos_list[i];
            current_inter_data.push_back(pos);

            const auto next_pos = origin_pos_list[i + 1];
            //Interpolation interval: 1
            for (float x_pos = pos.x() + 1; x_pos < next_pos.x(); x_pos++)
            {
                auto y_pos = s_calculator(x_pos);
                current_inter_data.push_back(QPointF(x_pos, y_pos));
            }
        }
        //add last point
        current_inter_data.push_back(origin_pos_list.back());
        first_current_slope = s_calculator.deriv(1, current_inter_data.front().x());
        last_current_slope = s_calculator.deriv(1, current_inter_data.back().x());

        last_pos_index_ = origin_pos_list.size() - 1;
    }

    //Interpolate between the endpoints of the current interval and the previous interval
    if (!smooth_data_list_.empty()) //Is it the first interval
    {
        auto pre_start = QPointF(smooth_data_list_[smooth_data_list_.size() - 2]);
        auto start = QPointF(smooth_data_list_.back());
        auto end = current_inter_data.front();
        std::vector<double> x_pos_list = { pre_start.x(),start.x(),end.x() };
        std::vector<double> y_pos_list = { pre_start.y(),start.y(),end.y() };

        tk::spline s(x_pos_list, y_pos_list, tk::spline::cspline_hermite, true,
            tk::spline::first_deriv, last_pre_slope_,
            tk::spline::first_deriv, first_current_slope);

        for (float x_pos = start.x() + 1; x_pos < end.x(); x_pos++)
        {
            auto y_pos = s(x_pos);
            smooth_data_list_.push_back(QPointF(x_pos, y_pos));
        }
    }

    smooth_data_list_.insert(smooth_data_list_.end(), current_inter_data.begin(), current_inter_data.end());

    last_pre_slope_ = last_current_slope;

    //print time expend
    auto sec_expend = tmp_timer.elapsed();
    qDebug() << "\ttime:" << sec_expend << "ms";

    this->update();
}
