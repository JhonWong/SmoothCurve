#ifndef SMOOTHCURVE_H
#define SMOOTHCURVE_H

#include <QQuickItem>
#include <QTimer>
#include <QSGNode>

class SmoothCurve : public QQuickItem
{
    Q_OBJECT
public:
    SmoothCurve();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private:
    int old_point_count_;
};

#endif // SMOOTHCURVE_H
