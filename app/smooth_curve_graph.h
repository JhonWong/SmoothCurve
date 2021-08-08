#pragma once
#include <QQuickItem>

class CustomPointNode;
class SmoothCurveNode;

class SmoothCurve : public QQuickItem
{
    Q_OBJECT
        Q_PROPERTY(QQuickItem *source WRITE setSource NOTIFY sourceChanged)
public:
    SmoothCurve(QQuickItem* parent = nullptr);

    QQuickItem *source() const { return texture_source_; }
    void setSource(QQuickItem *i);

protected:
    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private slots:
    void pointDataUpdate();

signals:
    void sourceChanged(QQuickItem* item);

private:
    SmoothCurveNode *curve_node_;
    CustomPointNode *point_node_;

    QQuickItem *texture_source_;

    bool source_changed_;
};