#include "random_points_data.h"

const int INCREASE_SPEED = 3;
const unsigned int UPDATE_INTERVAL = 200;

RandomPointsData& RandomPointsData::getInstance()
{
    static RandomPointsData data;
    return data;
}

RandomPointsData::RandomPointsData(QObject* parent) : QObject(parent)
{
    srand((unsigned)time(0));

    pos_list_.reserve(MAX_POINT_COUNT);

    update_timer_.start(UPDATE_INTERVAL);
    connect(&update_timer_, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
}

RandomPointsData::~RandomPointsData()
{

}

std::vector<QPointF>& RandomPointsData::get_pos_list()
{
    return pos_list_;
}

void RandomPointsData::onTimerUpdate()
{
    if (pos_list_.size() >= MAX_POINT_COUNT)
    {
        update_timer_.stop();
        return;
    }

    for (int i = 0; i < INCREASE_SPEED; i++)
    {
        int x_pos = rand() % 1920;
        int y_pos = rand() % 1080;

        pos_list_.push_back(QPointF(x_pos, y_pos));
    }

    emit sigDataChanged();
}
