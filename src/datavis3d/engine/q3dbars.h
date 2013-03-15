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
        Cylinders
    };

public:
    explicit Q3DBars();
    ~Q3DBars();

    void initialize();
    void render();

    // Add a row of data. Each new row is added to the front of the sample space, moving previous
    // rows back (if sample space is more than one row deep)
    void addDataRow(const QVector<float> &dataRow);
    // Add complete data set at a time, as a vector of data rows
    void addDataSet(const QVector< QVector<float> > &data);
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
    // Set camera rotation (in horizontal (-180...180) and vertical (0...90) angles and distance in percentage (10...500))
    void setCameraPosition(float horizontal, float vertical, int distance = 100);
    // TODO: värien asetus (vai themeillä?). Set uniform to false if you want the (height) color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform = true);
    // TODO: shaderien vaihto (themet?)
    // TODO: valon siirto / asetus

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
