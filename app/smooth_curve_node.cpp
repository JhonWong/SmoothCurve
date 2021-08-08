#include "smooth_curve_node.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleMaterial>
#include <QTime>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include "bezier/bezier.h"
#include "cubic_interp_wrapper.h"

struct LineState
{
    QSGTexture *texture_;
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
            "varying highp vec2 vTexCoord;                               \n"
            "void main() {                                              \n"
            "    gl_Position = qt_Matrix * aVertex;                     \n"
            "    vTexCoord = aTexCoord;                                  \n"
            "}";
    }

    const char *fragmentShader() const override {
        return
            "uniform lowp float qt_Opacity;                             \n"
            "uniform lowp sampler2D uSource;                                           \n"
            "varying highp vec2 vTexCoord;                               \n"
            "void main ()                                               \n"
            "{                                                          \n"
            "    lowp vec4 p1 = texture2D(uSource, vTexCoord);                         \n"
            "    gl_FragColor = p1 * qt_Opacity;  \n"
            "}";
    }

    QList<QByteArray> attributes() const override
    {
        return QList<QByteArray>() << "aVertex" << "aTexCoord";
    }

    void updateState(const LineState *state, const LineState *) override
    {
        //program()->setUniformValue(id_opacity_, state->opacity_);

        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glActiveTexture(GL_TEXTURE0);
        state->texture_->bind();
    }

    void resolveUniforms() override
    {
        program()->setUniformValue("uSource", 0); // GL_TEXTURE0
        //id_opacity_ = program()->uniformLocation("qt_Opacity");
    }

private:
    //int id_opacity_;
};

SmoothCurveNode::SmoothCurveNode(QSGTextureProvider *p)
    : geometry_(QSGGeometry::defaultAttributes_TexturedPoint2D(), 3)
    , current_pos_index_(0)
    , texture_provider_(p)
{
    setFlag(QSGNode::UsePreprocess, true);

    QSGSimpleMaterial<LineState> *material = LineShader::createMaterial();
    material->state()->texture_ = texture_provider_->texture();
    material->setFlag(QSGMaterial::Blending);

    node_.setMaterial(material);
    node_.setFlag(QSGNode::OwnsMaterial);

    geometry_.setDrawingMode(QSGGeometry::DrawTriangles);

    node_.setGeometry(&geometry_);
    node_.setFlag(QSGNode::OwnsGeometry);

    connect(texture_provider_.data(), &QSGTextureProvider::textureChanged, this, &SmoothCurveNode::textureChange, Qt::DirectConnection);
}

const int POINT_SIZE = 20;
void SmoothCurveNode::updateGeometry(const std::vector<QPointF>& pos_list)
{
    //if (pos_list.empty()) return;

    //update data
    //calculateSmoothPoints(pos_list);
    smooth_data_list_.clear();
    smooth_data_list_.push_back(QPointF(10,20));
    smooth_data_list_.push_back(QPointF(50,100));
    smooth_data_list_.push_back(QPointF(90,150));
    smooth_data_list_.push_back(QPointF(130,200));

    //curve node
    const int point_count = smooth_data_list_.size();
    unsigned int data_size = std::max(point_count, 1);
    geometry_.allocate(data_size * 6);

    auto vertices = geometry_.vertexDataAsTexturedPoint2D();
    for (int i = 0; i < smooth_data_list_.size(); i++)
    {
        auto pos1 = smooth_data_list_[i];
        vertices[i * 6 + 0].set(pos1.x() - POINT_SIZE / 2.0, pos1.y() - POINT_SIZE / 2.0, 0, 0);
        vertices[i * 6 + 1].set(pos1.x() + POINT_SIZE / 2.0, pos1.y() - POINT_SIZE / 2.0, 1, 0);
        vertices[i * 6 + 2].set(pos1.x() - POINT_SIZE / 2.0, pos1.y() + POINT_SIZE / 2.0, 0, 1);

        vertices[i * 6 + 3].set(pos1.x() + POINT_SIZE / 2.0, pos1.y() - POINT_SIZE / 2.0, 1, 0);
        vertices[i * 6 + 4].set(pos1.x() + POINT_SIZE / 2.0, pos1.y() + POINT_SIZE / 2.0, 1, 1);
        vertices[i * 6 + 5].set(pos1.x() - POINT_SIZE / 2.0, pos1.y() + POINT_SIZE / 2.0, 0, 1);
    }
}

void SmoothCurveNode::preprocess()
{
    auto m_material = dynamic_cast<QSGSimpleMaterial<LineState>*>(node_.material());
    if (!m_material) return;

    LineState *state = m_material->state();
    // Update the textures from the providers, calling into QSGDynamicTexture if required
    if (texture_provider_) {
        state->texture_ = texture_provider_->texture();
        if (QSGDynamicTexture *dt1 = qobject_cast<QSGDynamicTexture *>(state->texture_))
            dt1->updateTexture();
    }

    // Remove node from the scene graph if it is there and either texture is missing...
    if (node_.parent() && !state->texture_)
        removeChildNode(&node_);
    else if (!node_.parent() && state->texture_)
        appendChildNode(&node_);
}

void SmoothCurveNode::calculateSmoothPoints(const std::vector<QPointF>& origin_pos_list)
{
    smooth_data_list_ = origin_pos_list;
    return;

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

    //auto distance = std::floor(std::abs(end.x() - start.x()));
    auto distance = std::floor(cubicBezier.length() / (POINT_SIZE / 2.0));
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

void SmoothCurveNode::textureChange()
{
    markDirty(QSGNode::DirtyMaterial);
}
