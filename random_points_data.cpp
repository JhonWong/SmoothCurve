#include "random_points_data.h"

const int INCREASE_SPEED = 5;
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

int current_x_pos = 0;
void RandomPointsData::onTimerUpdate()
{
    if (pos_list_.size() >= MAX_POINT_COUNT)
    {
        update_timer_.stop();
        return;
    }

    for (int i = 0; i < INCREASE_SPEED; i++)
    {
        auto x_pos = current_x_pos;
        current_x_pos += 20;
        x_pos = rand() % 800;
        int y_pos = rand() % 800;

        pos_list_.push_back(QPointF(x_pos, y_pos));
    }
    emit sigDataChanged();
}
