#include "shader_point_node.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGSimpleMaterialShader>
#include "random_points_data.h"

struct PointState
{
    QColor color;
};

class PointShader : public QSGSimpleMaterialShader<PointState>
{
    QSG_DECLARE_SIMPLE_SHADER(PointShader, PointState);

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

    void updateState(const PointState *state, const PointState *) override
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

CustomPointNode::CustomPointNode(const int size, const QColor color)
    : geometry_(QSGGeometry::defaultAttributes_TexturedPoint2D(), 1)
{
    setGeometry(&geometry_);
    geometry_.setDrawingMode(GL_POINTS);
    geometry_.setLineWidth(3);

    QSGSimpleMaterial<PointState> *material = PointShader::createMaterial();
    material->setFlag(QSGMaterial::Blending);
    material->state()->color = color;
    setMaterial(material);
    setFlag(OwnsMaterial);
}

void CustomPointNode::updateGeomety(const std::vector<QPointF>& pos_list)
{
    const int point_count = pos_list.size();
    geometry_.allocate(point_count);

    auto vertices = geometry_.vertexDataAsTexturedPoint2D();
    for (int i = 0; i < point_count; i++)
    {
        auto& pos = pos_list[i];
        double tx = pos.x() / 1920.0;
        double ty = pos.y() / 1080.0;
        vertices[i].set(pos.x(), pos.y(), tx, ty);
    }

    markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
}
