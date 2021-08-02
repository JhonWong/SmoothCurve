#ifndef SMOOTHCURVE_H
#define SMOOTHCURVE_H

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
    int old_point_count_;
    std::vector<QPointF> smooth_data_list_;
};

#endif // SMOOTHCURVE_H
