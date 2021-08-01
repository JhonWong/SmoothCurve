#ifndef SMOOTHCURVE_H
#define SMOOTHCURVE_H

#include <QQuickItem>
#include <QTimer>
#include <QSGNode>

class SmoothCurve : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ getPos NOTIFY posChanged)

public:
    SmoothCurve();

    QPointF getPos();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

signals:
    void posChanged();

private slots:
    void onTimerUpdate();

private:
    QTimer update_timer_;
    std::vector<QPointF> pos_list_;
    int old_point_count_;
};

#endif // SMOOTHCURVE_H
