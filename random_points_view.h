#ifndef RANDOMPOINTSVIEW_H
#define RANDOMPOINTSVIEW_H

#include <QQuickItem>
#include <QSGNode>

class RandomPointsView : public QQuickItem
{
    Q_OBJECT
public:
    RandomPointsView();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private:
    int old_point_count_;
};

#endif // RANDOMPOINTSVIEW_H
