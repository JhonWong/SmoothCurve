#pragma once
#include <QObject>
#include <QTimer>
#include <QPointF>

const int MAX_POINT_COUNT = 6;

class RandomPointsData : public QObject
{
    Q_OBJECT
public:
    static RandomPointsData& getInstance();

    std::vector<QPointF>& get_pos_list();
private:
    explicit RandomPointsData(QObject *parent = nullptr);
    ~RandomPointsData();

signals:
    void sigDataChanged();

private slots:
    void onTimerUpdate();

private:
    QTimer update_timer_;
    std::vector<QPointF> pos_list_;
};
