#include "smoothcurve.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QTime>
#include "random_points_data.h"
#include "spline.h"

SmoothCurve::SmoothCurve()
    :old_point_count_(0)
{
    setFlag(ItemHasContents, true);

    auto& point_data_obj = RandomPointsData::getInstance();
    connect(&point_data_obj,&RandomPointsData::sigDataChanged,this,&SmoothCurve::calculateSmoothPoints);
}

QSGNode *SmoothCurve::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *_data)
{
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), MAX_POINT_COUNT);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
    }

    auto& point_data_obj = RandomPointsData::getInstance();
    const auto& pos_list = point_data_obj.get_pos_list();
    const int point_count = pos_list.size();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    for(int i = old_point_count_;i<point_count;i++)
    {
        auto& pos = pos_list[i];
        vertices[i].set(pos.x(),pos.y());
    }
    old_point_count_ = point_count;

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

int sagement_count = 5;
void SmoothCurve::calculateSmoothPoints()
{
    QTime tmp_timer;
    tmp_timer.start();

    const auto& origin_pos_list = RandomPointsData::getInstance().get_pos_list();
    smooth_data_list_.clear();

    std::vector<double> x_pos_list;
    std::vector<double> y_pos_list;
    for(auto pos : origin_pos_list)
    {
        x_pos_list.push_back(pos.x());
        y_pos_list.push_back(pos.y());
    }

    tk::spline s_calculator(x_pos_list,y_pos_list);

    for(int i = 0;i< origin_pos_list.size();i++)
    {
        const auto pos = origin_pos_list[i];
        smooth_data_list_.push_back(pos);
        if(i < (origin_pos_list.size()-1))
        {
            const auto next_pos = origin_pos_list[i + 1];
            auto dis = (next_pos.x() - pos.x())/sagement_count;
            for(int j = 0;j<sagement_count;j++)
            {
                auto x_inter = pos.x() + j * dis;
                auto y_inter = s_calculator(x_inter);
                smooth_data_list_.push_back(QPointF(x_inter,y_inter));
            }
        }
    }

    auto sec_expend = tmp_timer.elapsed()/1000.0;
    qDebug() << "Point count:" << origin_pos_list.size() << "\ttime:" << sec_expend;

    this->update();
}
