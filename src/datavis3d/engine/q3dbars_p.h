#ifndef Q3DBARS_p_H
#define Q3DBARS_p_H

#include "qdatavis3dglobal.h"
#include <QOpenGLFunctions>

class QOpenGLShaderProgram;
class QPoint;
class QPointF;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DBars;

class Q3DBarsPrivate
{
public:
    enum SelectionType {
        None = 0,
        Bar,
        Row,
        Column
    };

public:
    Q3DBarsPrivate(Q3DBars *q);
    ~Q3DBarsPrivate();

    void loadBarMesh();
    void loadBackgroundMesh();
    void initShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionShader();
    void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
    void initSelectionBuffer();
    void calculateSceneScalingFactors();
    SelectionType isSelected(int row, int bar, const QVector3D &selection);

    GLuint m_positionAttr;
    GLuint m_uvAttr;
    GLuint m_normalAttr;
    GLuint m_colorUniform;
    GLuint m_viewMatrixUniform;
    GLuint m_modelMatrixUniform;
    GLuint m_invTransModelMatrixUniform;
    GLuint m_mvpMatrixUniform;
    GLuint m_lightPositionUniform;
    GLuint m_lightStrengthUniform;

    GLuint m_positionAttrSelection;
    GLuint m_mvpMatrixUniformSelection;
    GLuint m_colorUniformSelection;

    GLuint m_positionAttrBackground;
    GLuint m_uvAttrBackground;
    GLuint m_normalAttrBackground;
    GLuint m_colorUniformBackground;
    GLuint m_viewMatrixUniformBackground;
    GLuint m_modelMatrixUniformBackground;
    GLuint m_invTransModelMatrixUniformBackground;
    GLuint m_mvpMatrixUniformBackground;
    GLuint m_lightPositionUniformBackground;
    GLuint m_lightStrengthUniformBackground;

    GLuint m_vertexbuffer;
    GLuint m_uvbuffer;
    GLuint m_normalbuffer;
    GLuint m_elementbuffer;

    GLuint m_framebufferSelection;

    GLuint m_vertexbufferBackground;
    GLuint m_uvbufferBackground;
    GLuint m_normalbufferBackground;
    GLuint m_elementbufferBackground;

    GLuint m_selectionTexture;
    GLuint m_depthTexture;

    Q3DBars *q_ptr;

    QOpenGLShaderProgram *m_barShader;
    QOpenGLShaderProgram *m_selectionShader;
    QOpenGLShaderProgram *m_backgroundShader;
    QPoint m_sampleCount;
    QString m_objFile;
    int m_vertexCount;
    int m_indexCount;
    int m_indexCountBackground;
    bool m_mousePressed;
    QPoint m_mousePos;
    int m_zoomLevel;
    float m_horizontalRotation;
    float m_verticalRotation;
    QPointF m_barThickness;
    QPointF m_barSpacing;
    bool m_meshDataLoaded;
    bool m_background;
    QVector< QVector<float> > m_dataSet;
    float m_rowWidth;
    float m_columnDepth;
    float m_maxDimension;
    float m_scaleX;
    float m_scaleZ;
    float m_scaleFactorX;
    float m_scaleFactorZ;
    float m_sceneScale;
    float m_maxSceneSize;
    QColor m_baseColor;
    QColor m_heightColor;
    QColor m_depthColor;
    bool m_uniformColor;
    bool m_isInitialized;
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
