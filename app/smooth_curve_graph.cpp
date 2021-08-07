#include "smooth_curve_graph.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QTime>
#include "random_points_data.h"
#include "spline.h"
#include "bezier/bezier.h"
#include "shader_point_node.h"
#include "smooth_curve_node.h"

SmoothCurve::SmoothCurve()
    : point_node_(nullptr)
    , curve_node_(nullptr)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj, &RandomPointsData::sigDataChanged, this, &SmoothCurve::pointDataUpdate);
}

QSGNode* SmoothCurve::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* _data)
{
    SmoothCurveNode* node = static_cast<SmoothCurveNode*>(oldNode);
    if (!node)
    {
        node = new SmoothCurveNode();
    }
    auto& origin_data_list = RandomPointsData::getInstance().get_pos_list();
    node->updateGeometry(origin_data_list);

    //point node
    if (!point_node_)
    {
        point_node_ = new CustomPointNode(10, QColor(Qt::red));
        node->appendChildNode(point_node_);
    }
    point_node_->updateGeometry(origin_data_list);

    return node;
}

void SmoothCurve::pointDataUpdate()
{
    this->update();
}
