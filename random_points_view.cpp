#include "random_points_view.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include "random_points_data.h"

RandomPointsView::RandomPointsView()
    :old_point_count_(0)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj,&RandomPointsData::sigDataChanged,this,[&](){
        this->update();
    });
}

QSGNode *RandomPointsView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *_data)
{
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), MAX_POINT_COUNT);
        geometry->setLineWidth(3);
        geometry->setDrawingMode(QSGGeometry::DrawPoints);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
    }

    auto& point_data_obj = RandomPointsData::getInstance();
    const auto& pos_list = point_data_obj.get_pos_list();
    const int point_count = pos_list.size();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    for(int i = old_point_count_;i<point_count;i++)
    {
        auto& pos = pos_list[i];
        vertices[i].set(pos.x(),pos.y());
    }
    old_point_count_ = point_count;

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
