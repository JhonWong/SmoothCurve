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

SmoothCurve::SmoothCurve(QQuickItem* parent)
    : QQuickItem(parent)
    , point_node_(nullptr)
    , curve_node_(nullptr)
    , texture_source_(nullptr)
    , source_changed_(false)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj, &RandomPointsData::sigDataChanged, this, &SmoothCurve::pointDataUpdate);
}

QSGNode* SmoothCurve::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* _data)
{
    if (!texture_source_ || !texture_source_->isTextureProvider()) {
        delete oldNode;
        return nullptr;
    }

    SmoothCurveNode* node = static_cast<SmoothCurveNode*>(oldNode);
    // If the sources have changed, recreate the nodes
    if (source_changed_) {
        delete node;
        node = nullptr;
        source_changed_ = false;
    }

    if (!node)
        node = new SmoothCurveNode(texture_source_->textureProvider());

    auto& origin_data_list = RandomPointsData::getInstance().get_pos_list();
    node->updateGeometry(origin_data_list);

    //point node
    //if (!point_node_)
    //{
    //    point_node_ = new CustomPointNode(10, QColor(Qt::red));
    //    node->appendChildNode(point_node_);
    //}
    //point_node_->updateGeometry(origin_data_list);

    return node;
}

void SmoothCurve::pointDataUpdate()
{
    //this->update();
}

void SmoothCurve::setSource(QQuickItem *i)
{
    if (i == texture_source_)
        return;
    texture_source_ = i;
    emit sourceChanged(texture_source_);
    source_changed_ = true;
    update();
}