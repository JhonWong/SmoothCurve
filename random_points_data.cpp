#include "random_points_data.h"

RandomPointsData &RandomPointsData::getInstance()
{
    static RandomPointsData data;
    return data;
}

RandomPointsData::RandomPointsData(QObject *parent) : QObject(parent)
{
    srand((unsigned) time(0));

    pos_list_.reserve(MAX_POINT_COUNT);

    update_timer_.start(20);
    connect(&update_timer_,SIGNAL(timeout()),this,SLOT(onTimerUpdate()));
}

RandomPointsData::~RandomPointsData()
{

}

std::vector<QPointF> &RandomPointsData::get_pos_list()
{
    return pos_list_;
}

void RandomPointsData::onTimerUpdate()
{
    if(pos_list_.size() >= MAX_POINT_COUNT)
    {
        update_timer_.stop();
        return;
    }

    for(int i = 0;i< 20;i++)
    {
        int x_pos = rand() % 800;
        int y_pos = rand() % 800;

        pos_list_.push_back(QPointF(x_pos,y_pos));
    }
    emit sigDataChanged();
}
