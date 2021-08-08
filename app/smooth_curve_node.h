#pragma once
#include <QObject>
#include <QSGSimpleTextureNode>
#include <QSGTextureProvider>
#include <QPointer>

class SmoothCurveNode :public QObject, public QSGNode
{
    Q_OBJECT
public:
    SmoothCurveNode(QSGTextureProvider *p);

    void updateGeometry(const std::vector<QPointF>& pos_list);

    void preprocess() override;

private:
    void calculateSmoothPoints(const std::vector<QPointF>& pos_list);

    std::vector<QPointF> bezierInterp(const QPointF start, const QPointF end, const bool topleft = true);
    QPointF getControlPoint(QPointF another_pos, const double slope, const double distance, const double topleft);

public slots:
    void textureChange();

private:
    QSGGeometryNode node_;
    QSGGeometry geometry_;
    QPointer<QSGTextureProvider> texture_provider_;

    std::vector<QPointF> smooth_data_list_;
    int current_pos_index_;;
};