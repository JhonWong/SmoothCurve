#include "smoothcurve.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

SmoothCurve::SmoothCurve()
{
    setFlag(ItemHasContents, true);

    srand((unsigned) time(0));

    update_timer_.start(500);
    connect(&update_timer_,SIGNAL(timeout()),this,SLOT(onTimerUpdate()));
}

QSGNode *SmoothCurve::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *_data)
{
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 1);
        geometry->setLineWidth(20);
        geometry->setDrawingMode(QSGGeometry::DrawPoints);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(0, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

        qDebug() << "new node!!!!!";
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(1);
        qDebug() << "old node!!!!!";
    }

    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();

    vertices[0].set(pos_.x(),pos_.y());

    qDebug() << "node pos:(" << pos_.x() << "," << pos_.y()<<")";

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void SmoothCurve::onTimerUpdate()
{
    int x_pos = rand() % 800;
    int y_pos = rand() % 800;

    pos_ = QPointF(x_pos,y_pos);

    emit posChanged();
    update();
}

QPointF SmoothCurve::getPos()
{
    return QPointF(200,300);
}
