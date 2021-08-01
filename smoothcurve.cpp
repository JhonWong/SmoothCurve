#include "smoothcurve.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

const int MAX_POINT_COUNT = 1000;

SmoothCurve::SmoothCurve()
    :old_point_count_(0)
{
    setFlag(ItemHasContents, true);

    srand((unsigned) time(0));

    pos_list_.reserve(MAX_POINT_COUNT);

    update_timer_.start(200);
    connect(&update_timer_,SIGNAL(timeout()),this,SLOT(onTimerUpdate()));
}

QSGNode *SmoothCurve::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *_data)
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

    const int point_count = pos_list_.size();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    for(int i = old_point_count_;i<point_count;i++)
    {
        auto& pos = pos_list_[i];
        vertices[i].set(pos.x(),pos.y());
    }
    old_point_count_ = point_count;

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void SmoothCurve::onTimerUpdate()
{
    if(pos_list_.size() >= MAX_POINT_COUNT)
    {
        update_timer_.stop();
        return;
    }

    for(int i = 0;i< 20;i++)
    {
        int x_pos = rand() % 800;
        int y_pos = rand() % 800;

        pos_list_.push_back(QPointF(x_pos,y_pos));
    }
    emit posChanged();
    update();
}

QPointF SmoothCurve::getPos()
{
    return QPointF(200,300);
}
