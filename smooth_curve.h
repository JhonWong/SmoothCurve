#pragma once

#include <QQuickItem>
#include <QSGNode>

class SmoothCurve : public QQuickItem
{
    Q_OBJECT
public:
    SmoothCurve();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private:
    void calculateSmoothPoints();

private:
    std::vector<QPointF> smooth_data_list_;

    int last_pos_index_;
    float last_pre_slope_;
};
