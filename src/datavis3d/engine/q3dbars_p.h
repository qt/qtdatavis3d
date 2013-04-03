#ifndef Q3DBARS_p_H
#define Q3DBARS_p_H

#include "qdatavis3dglobal.h"
#include <QOpenGLFunctions>

class QOpenGLPaintDevice;
class QPoint;
class QPointF;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DBars;
class SampleData;
class ShaderHelper;
class ObjectHelper;

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

    GLuint m_framebufferSelection;

    Q3DBars *q_ptr;

    QOpenGLPaintDevice *m_paintDevice;
    ShaderHelper *m_barShader;
    ShaderHelper *m_selectionShader;
    ShaderHelper *m_backgroundShader;
    ObjectHelper *m_barObj;
    ObjectHelper *m_backgroundObj;
    QPoint m_sampleCount;
    QString m_objFile;
    bool m_mousePressed;
    QPoint m_mousePos;
    int m_zoomLevel;
    float m_horizontalRotation;
    float m_verticalRotation;
    QPointF m_barThickness;
    QPointF m_barSpacing;
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
    // TODO: move to themes/theme class..
    QColor m_baseColor;
    QColor m_heightColor;
    QColor m_depthColor;
    QColor m_backgroundColor;
    QColor m_windowColor;
    QColor m_textColor;
    QColor m_textBackgroundColor;
    QColor m_highlightBarColor;
    QColor m_highlightRowColor;
    QColor m_highlightColumnColor;
    float m_lightStrength;
    float m_ambientStrength;
    float m_highlightLightStrength;
    bool m_uniformColor;
    // TODO: Perhaps different shaders are required for different themes
    // ..
    bool m_isInitialized;
    Q3DBars::SelectionMode m_selectionMode;
    SampleData *m_selectedBar;
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
