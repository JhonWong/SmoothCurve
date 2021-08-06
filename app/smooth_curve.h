#pragma once
#include <QQuickItem>
#include <QSGNode>

#include "cubic_interp_wrapper.h"

class SmoothCurve : public QQuickItem
{
    Q_OBJECT
public:
    SmoothCurve();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private slots:
    void calculateSmoothPoints();

private:
    //return:origin interpolation list
    std::vector<QPointF> interpSingleMono(const std::tuple<int,std::vector<QPointF>> mono_inter,const double left_slope_bound,double& slope_last_origin_pos);

    std::vector<QPointF> bezierInterp(const QPointF start,const QPointF end,const bool topleft = true);
    QPointF getControlPoint(QPointF another_pos, const double slope,const double distance,const double topleft);

private:
    std::vector<QPointF> smooth_data_list_;

    int current_pos_index_;
    float last_pre_slope_;
};