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
    void randomValueInterp(const std::vector<QPointF> pos_list);

private:
    std::vector<QPointF> smooth_data_list_;

    int current_pos_index_;
    float last_pre_slope_;
};
