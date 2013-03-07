#ifndef Q3DBARS_H
#define Q3DBARS_H

#include "qdatavis3dglobal.h"
#include "q3dwindow.h"

class QOpenGLShaderProgram;

QTCOMMERCIALDATAVIS3D_BEGIN_NAMESPACE

class Q3DBarsPrivate;

class QTCOMMERCIALDATAVIS3D_EXPORT Q3DBars : public Q3DWindow
{
    Q_OBJECT
public:
    enum BarStyle {
        Bars = 0,
        Pyramids,
        Cones,
        Cylinders,
        Apes // TODO: remove; inserted now just for fun
    };

public:
    explicit Q3DBars();
    ~Q3DBars();

    void initialize();
    void render();

    void addDataSet(QVector<float> data);
    // bar thickness, spacing between bars, and is spacing relative to thickness or absolute
    // y -component sets the thickness/spacing of z -direction
    // With relative 0.0f means side-to-side, 1.0f = one thickness in between
    void setBarSpecs(QPointF thickness = QPointF(1.0f, 1.0f)
            , QPointF spacing = QPointF(1.0f, 1.0f)
            , bool relative = true);
    // bar type; bars (=cubes), pyramids, cones, cylinders, etc.
    void setBarType(BarStyle style, bool smooth = false);
    // override bar type with own mesh
    void setMeshFileName(const QString &objFileName);
    // how many samples per row and column
    void setupSampleSpace(QPoint sampleCount);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
