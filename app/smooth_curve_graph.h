#pragma once
#include <QQuickItem>
#include <QSGNode>

#include "cubic_interp_wrapper.h"

class CustomPointNode;
class SmoothCurveNode;

class SmoothCurve : public QQuickItem
{
    Q_OBJECT
public:
    SmoothCurve();

    QSGNode* updatePaintNode(QSGNode *old_node, UpdatePaintNodeData *_data) override;

private slots:
    void pointDataUpdate();

private:
    CustomPointNode *point_node_;
    SmoothCurveNode *curve_node_;
};