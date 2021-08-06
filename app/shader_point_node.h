#pragma once
#include <QQuickItem>
#include <QSGNode>

class CustomPointNode : public QSGGeometryNode
{
public:
    CustomPointNode(const int size, const QColor color);

    void updateGeomety(const std::vector<QPointF>& pos_list);

private:
    QSGGeometry geometry_;
    QColor color_;
};