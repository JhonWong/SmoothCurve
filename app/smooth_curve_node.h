#pragma once
#include <QQuickItem>
#include <QSGNode>

#include "cubic_interp_wrapper.h"

class SmoothCurveNode : public QSGGeometryNode
{
public:
    SmoothCurveNode();

    void updateGeometry(const std::vector<QPointF>& pos_list);

private:
    void calculateSmoothPoints(const std::vector<QPointF>& pos_list);

    std::vector<QPointF> bezierInterp(const QPointF start,const QPointF end,const bool topleft = true);
    QPointF getControlPoint(QPointF another_pos, const double slope,const double distance,const double topleft);

private:
    QSGGeometry geometry_;
    std::vector<QPointF> smooth_data_list_;
    int current_pos_index_;
};