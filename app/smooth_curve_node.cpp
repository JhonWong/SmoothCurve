#include "smooth_curve_node.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleMaterial>
#include <QTime>
#include "bezier/bezier.h"

struct LineState
{
    QColor color;
};

class LineShader : public QSGSimpleMaterialShader<LineState>
{
    QSG_DECLARE_SIMPLE_SHADER(LineShader, LineState);

public:

    const char *vertexShader() const override {
        return
            "attribute highp vec4 aVertex;                              \n"
            "attribute highp vec2 aTexCoord;                            \n"
            "uniform highp mat4 qt_Matrix;                              \n"
            "varying highp vec2 texCoord;                               \n"
            "void main() {                                              \n"
            "    gl_Position = qt_Matrix * aVertex;                     \n"
            "    texCoord = aTexCoord;                                  \n"
            "}";
    }

    const char *fragmentShader() const override {
        return
            "uniform lowp float qt_Opacity;                             \n"
            "uniform lowp vec4 color;                                   \n"
            "varying highp vec2 texCoord;                               \n"
            "void main ()                                               \n"
            "{                                                          \n"
            "    gl_FragColor = color * qt_Opacity;  \n"
            "}";
    }

    QList<QByteArray> attributes() const override
    {
        return QList<QByteArray>() << "aVertex" << "aTexCoord";
    }

    void updateState(const LineState *state, const LineState *) override
    {
        program()->setUniformValue(id_color, state->color);
    }

    void resolveUniforms() override
    {
        id_color = program()->uniformLocation("color");
    }

private:
    int id_color;
};

SmoothCurveNode::SmoothCurveNode()
    : geometry_(QSGGeometry::defaultAttributes_TexturedPoint2D(), 1)
    , current_pos_index_(0)
{
    setGeometry(&geometry_);
    geometry_.setDrawingMode(QSGGeometry::DrawLineStrip);
    geometry_.setLineWidth(3);

    QSGSimpleMaterial<LineState> *material = LineShader::createMaterial();
    material->setFlag(QSGMaterial::Blending);
    material->state()->color = QColor(Qt::darkBlue);
    setMaterial(material);
    setFlag(OwnsMaterial);
}

void SmoothCurveNode::updateGeometry(const std::vector<QPointF>& pos_list)
{
    if (pos_list.empty()) return;

    //update data
    calculateSmoothPoints(pos_list);

    //curve node
    const int point_count = smooth_data_list_.size();
    unsigned int data_size = std::max(point_count, 1);
    geometry_.allocate(data_size);

    auto vertices = geometry_.vertexDataAsTexturedPoint2D();
    for (int i = 0; i < point_count; i++)
    {
        auto& pos = smooth_data_list_[i];
        double tx = pos.x() / 1920.0;
        double ty = pos.y() / 1080.0;
        vertices[i].set(pos.x(), pos.y(), tx, ty);
    }

    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}

void SmoothCurveNode::calculateSmoothPoints(const std::vector<QPointF>& origin_pos_list)
{
    if (current_pos_index_ == ((int)origin_pos_list.size() - 1)) return;

    for (int i = current_pos_index_; i < origin_pos_list.size() - 1; i++)
    {
        bool topleft = i % 2 == 0;
        auto interp_pos_list = bezierInterp(origin_pos_list[i], origin_pos_list[i + 1], topleft);

        smooth_data_list_.insert(smooth_data_list_.end(), interp_pos_list.begin(), interp_pos_list.end());
    }
    current_pos_index_ = origin_pos_list.size() - 1;
}

const double CONTROL_POINT_DISS = 100;
std::vector<QPointF> SmoothCurveNode::bezierInterp(const QPointF start, const QPointF end, const bool topleft)
{
    std::vector<Bezier::Vec2> pos_list;
    pos_list.push_back({ (float)start.x(),(float)start.y() });

    auto p1 = getControlPoint(start, SLOPE_DEFAULT, CONTROL_POINT_DISS, topleft);
    pos_list.push_back({ (float)p1.x(),(float)p1.y() });

    auto p2 = getControlPoint(end, SLOPE_DEFAULT, CONTROL_POINT_DISS, topleft);
    pos_list.push_back({ (float)p2.x(),(float)p2.y() });

    pos_list.push_back({ (float)end.x(),(float)end.y() });

    Bezier::Bezier<3> cubicBezier(pos_list);
    std::vector<QPointF> interp_list;

    auto distance = std::floor(std::abs(end.x() - start.x()));
    for (int i = 0; i < distance; i++)
    {
        auto t = i * (1.0 / distance);
        auto pos = cubicBezier.valueAt(t);
        interp_list.push_back(QPointF(pos.x, pos.y));
    }
    interp_list.push_back(end);

    return interp_list;
}

QPointF SmoothCurveNode::getControlPoint(QPointF another_pos, const double k, const double distance, const double topleft)
{
    auto b = another_pos.y() - k * another_pos.x();
    auto x_dis = std::sqrt((std::pow(distance, 2) / (1 + k)));
    if (!topleft)
    {
        x_dis = -x_dis;
    }

    auto x_pos = another_pos.x() + x_dis;
    auto y_pos = k * x_pos + b;

    return QPointF(x_pos, y_pos);
}
