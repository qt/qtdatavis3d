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

    enum CameraPreset {
        PresetFrontLow = 0,
        PresetFront,
        PresetFrontHigh,
        PresetLeftLow,
        PresetLeft,
        PresetLeftHigh,
        PresetRightLow,
        PresetRight,
        PresetRightHigh,
        PresetBehindLow,
        PresetBehind,
        PresetBehindHigh,
        PresetIsometricLeft,
        PresetIsometricLeftHigh,
        PresetIsometricRight,
        PresetIsometricRightHigh,
        PresetDirectlyAbove,
        PresetDirectlyAboveCW45,
        PresetDirectlyAboveCCW45
    };

    enum SelectionMode {
        None = 0,
        Bar,
        BarAndRow,
        BarAndColumn,
        BarRowAndColumn
    };

public:
    explicit Q3DBars();
    ~Q3DBars();

    void initialize();
    void render();
    void render(QPainter *painter);

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
    // Select preset camera placement
    void setCameraPreset(CameraPreset preset);
    // Set camera rotation if you don't want to use the presets (in horizontal (-180...180) and
    // vertical (0...90) angles and distance in percentage (10...500))
    void setCameraPosition(float horizontal, float vertical, int distance = 100);
    // Set theme (bar colors, shaders, background colors and text colors are affected)
    void setTheme(); // TODO: joku enum kai
    // Set color if you don't want to use themes. Set uniform to false if you want the (height) color to change from bottom to top
    void setBarColor(QColor baseColor, QColor heightColor, QColor depthColor, bool uniform = true);
    // TODO: shaderien vaihto (themet?)
    // TODO: valon siirto / asetus
    // Change selection mode; single bar, bar and row, bar and column, or all
    void setSelectionMode(SelectionMode mode);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawScene();
    QScopedPointer<Q3DBarsPrivate> d_ptr;
    Q_DISABLE_COPY(Q3DBars)
};

QTCOMMERCIALDATAVIS3D_END_NAMESPACE

#endif
