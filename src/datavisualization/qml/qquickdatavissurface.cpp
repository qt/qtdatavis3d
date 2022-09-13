// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavissurface_p.h"
#include <QtCore/QMutexLocker>

#include "declarativescene_p.h"
#include "surface3dcontroller_p.h"
#include "qsurface3dseries_p.h"
#include "utils_p.h"

#include <QtQuick3D/private/qquick3drepeater_p.h>

QT_BEGIN_NAMESPACE

QQuickDataVisSurface::QQuickDataVisSurface(QQuickItem *parent)
    : QQuickDataVisItem(parent),
      m_surfaceController(0)
{
    setAcceptedMouseButtons(Qt::AllButtons);

    // Create the shared component on the main GUI thread.
    m_surfaceController = new Surface3DController(boundingRect().toRect(), new Declarative3DScene);
    setSharedController(m_surfaceController);

    QObject::connect(m_surfaceController, &Surface3DController::selectedSeriesChanged,
                     this, &QQuickDataVisSurface::selectedSeriesChanged);
    QObject::connect(m_surfaceController, &Surface3DController::flipHorizontalGridChanged,
                     this, &QQuickDataVisSurface::flipHorizontalGridChanged);
}

QQuickDataVisSurface::~QQuickDataVisSurface()
{
    QMutexLocker locker(m_nodeMutex.data());
    const QMutexLocker locker2(mutex());
    delete m_surfaceController;
}

QValue3DAxis *QQuickDataVisSurface::axisX() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisX());
}

void QQuickDataVisSurface::setAxisX(QValue3DAxis *axis)
{
    m_surfaceController->setAxisX(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisY() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisY());
}

void QQuickDataVisSurface::setAxisY(QValue3DAxis *axis)
{
    m_surfaceController->setAxisY(axis);
}

QValue3DAxis *QQuickDataVisSurface::axisZ() const
{
    return static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
}

void QQuickDataVisSurface::setAxisZ(QValue3DAxis *axis)
{
    m_surfaceController->setAxisZ(axis);
}

QSurface3DSeries *QQuickDataVisSurface::selectedSeries() const
{
    return m_surfaceController->selectedSeries();
}

void QQuickDataVisSurface::setFlipHorizontalGrid(bool flip)
{
   m_surfaceController->setFlipHorizontalGrid(flip);
}

bool QQuickDataVisSurface::flipHorizontalGrid() const
{
    return m_surfaceController->flipHorizontalGrid();
}

QQmlListProperty<QSurface3DSeries> QQuickDataVisSurface::seriesList()
{
    return QQmlListProperty<QSurface3DSeries>(this, this,
                                              &QQuickDataVisSurface::appendSeriesFunc,
                                              &QQuickDataVisSurface::countSeriesFunc,
                                              &QQuickDataVisSurface::atSeriesFunc,
                                              &QQuickDataVisSurface::clearSeriesFunc);
}

void QQuickDataVisSurface::appendSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                          QSurface3DSeries *series)
{
    reinterpret_cast<QQuickDataVisSurface *>(list->data)->addSeries(series);
}

qsizetype QQuickDataVisSurface::countSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().size();
}

QSurface3DSeries *QQuickDataVisSurface::atSeriesFunc(QQmlListProperty<QSurface3DSeries> *list,
                                                   qsizetype index)
{
    return reinterpret_cast<QQuickDataVisSurface *>(list->data)->m_surfaceController->surfaceSeriesList().at(index);
}

void QQuickDataVisSurface::clearSeriesFunc(QQmlListProperty<QSurface3DSeries> *list)
{
    QQuickDataVisSurface *declSurface = reinterpret_cast<QQuickDataVisSurface *>(list->data);
    QList<QSurface3DSeries *> realList = declSurface->m_surfaceController->surfaceSeriesList();
    int count = realList.size();
    for (int i = 0; i < count; i++)
        declSurface->removeSeries(realList.at(i));
}

void QQuickDataVisSurface::addSeries(QSurface3DSeries *series)
{
    m_surfaceController->addSeries(series);
}

void QQuickDataVisSurface::removeSeries(QSurface3DSeries *series)
{
    m_surfaceController->removeSeries(series);
    series->setParent(this); // Reparent as removing will leave series parentless
}

void QQuickDataVisSurface::handleAxisXChanged(QAbstract3DAxis *axis)
{
    emit axisXChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisYChanged(QAbstract3DAxis *axis)
{
    emit axisYChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::handleAxisZChanged(QAbstract3DAxis *axis)
{
    emit axisZChanged(static_cast<QValue3DAxis *>(axis));
}

void QQuickDataVisSurface::componentComplete()
{
    QQuickDataVisItem::componentComplete();

    m_model = new QQuick3DModel();
    m_model->setParent(QQuick3DViewport::scene());
    m_model->setParentItem(QQuick3DViewport::scene());

    m_gridModel = new QQuick3DModel();
    m_gridModel->setParent(QQuick3DViewport::scene());
    m_gridModel->setParentItem(QQuick3DViewport::scene());

    setScaleWithBackground({2.1f, 1.1f, 2.1f});
    setScaleOffset({2.0f, 1.0f, 2.0f});
    setLineLengthScaleFactor(0.02f);

    setLabelMargin(0.3f);
}

void QQuickDataVisSurface::updateGrid()
{
    int gridLineCountX = segmentLineRepeaterX()->count() / 2;
    int subGridLineCountX = subsegmentLineRepeaterX()->count() / 2;
    int gridLineCountY = segmentLineRepeaterY()->count() / 2;
    int subGridLineCountY = subsegmentLineRepeaterY()->count() / 2;
    int gridLineCountZ = segmentLineRepeaterZ()->count() / 2;
    int subGridLineCountZ = subsegmentLineRepeaterZ()->count() / 2;

    QVector3D scaleX(scale().x() * lineLengthScaleFactor() * scaleWithBackground().x(), lineWidthScaleFactor(), lineWidthScaleFactor());
    QVector3D scaleY(lineWidthScaleFactor(), scale().y() * lineLengthScaleFactor() * scaleWithBackground().y(), lineWidthScaleFactor());
    QVector3D scaleZ(scale().z() * lineLengthScaleFactor() * scaleWithBackground().z(), lineWidthScaleFactor(), lineWidthScaleFactor());

    QVector3D lineBackRotationX(0, 0, 0);
    QVector3D lineBackRotationY(0, 0, 0);

    auto axisX = static_cast<QValue3DAxis *>(m_surfaceController->axisX());
    auto axisY = static_cast<QValue3DAxis *>(m_surfaceController->axisY());
    auto axisZ = static_cast<QValue3DAxis *>(m_surfaceController->axisZ());

    auto lineFloorRotation = Utils::calculateRotation(QVector3D(0, -90, 0));
    float linePosX = scaleX.x() / 2.0f;
    float linePosY = -scaleWithBackground().y() + 0.01f;
    float linePosZ = 0;

    if (!flipped().z()) {
        linePosZ = -scaleWithBackground().z() + gridOffset();
    } else {
        linePosZ = scaleWithBackground().z() - gridOffset();
        lineBackRotationX = QVector3D(0, 180, 0);
        lineBackRotationY = QVector3D(0, 180, 0);
    }

    auto scale = this->scale().x() * scaleWithBackground().x() + scaleOffset().x();

    // Floor horizontal line
    for (int i = 0; i < subGridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i));
        linePosZ = axisX->subGridPositionAt(i) * -scale + scaleOffset().x();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
    }

    for (int i  = 0; i < gridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i));
        linePosZ = axisX->gridPositionAt(i) * -scale + scaleOffset().x();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
    }

    linePosY = scaleY.y() / 2.0f;
    linePosZ += 0.01f;
    scale = this->scale().z() * scaleWithBackground().z() + scaleOffset().z();
    // Side vertical line
    for (int i  = 0; i < subGridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i + subGridLineCountZ));
        linePosX = axisZ->subGridPositionAt(i) * scale - scaleOffset().z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
    }
    for (int i  = 0; i < gridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i + gridLineCountZ));
        linePosX = axisZ->gridPositionAt(i) * scale - scaleOffset().z();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
    }

    // Side horizontal line
    linePosX = scaleX.x() / 2.0f;
    scale = this->scale().y() * scaleWithBackground().y() + scaleOffset().y();
    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->gridPositionAt(i) * scale - scaleOffset().y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
    }

    for (int i = 0; i < subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i));
        linePosY = axisY->subGridPositionAt(i) * scale - scaleOffset().y();
        positionAndScaleLine(lineNode, scaleX, QVector3D(linePosX, linePosY, linePosZ));
    }

    // Floor vertical line
    linePosZ = scaleZ.z() / -2.0f;
    linePosY = -scaleWithBackground().y() + 0.01f;
    scale = this->scale().z() * scaleWithBackground().z() + scaleOffset().z();
    for (int i = 0; i < subGridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterZ()->objectAt(i));
        linePosX = axisZ->subGridPositionAt(i) * scale - scaleOffset().z();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineFloorRotation);
    }
    for (int i  = 0; i < gridLineCountZ; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterZ()->objectAt(i));
        linePosX = axisZ->gridPositionAt(i) * scale - scaleOffset().z();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineFloorRotation);
    }

    // Back horizontal line
    scale = this->scale().y() * scaleWithBackground().y() + scaleOffset().y();
    linePosX -= 0.01f;
    for (int i = 0; i < subGridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterY()->objectAt(i + subGridLineCountY));
        linePosY = axisY->subGridPositionAt(i) * scale - scaleOffset().y();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineFloorRotation);
    }

    for (int i  = 0; i < gridLineCountY; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterY()->objectAt(i + gridLineCountY));
        linePosY = axisY->gridPositionAt(i) * scale - scaleOffset().y();
        positionAndScaleLine(lineNode, scaleZ, QVector3D(linePosX, linePosY, linePosZ));
        lineNode->setRotation(lineFloorRotation);
    }

    // Back vertical line
    linePosY = scaleY.y() / 2.0f;
    scale = this->scale().x() * scaleWithBackground().x() + scaleOffset().x();
    for (int i  = 0; i < gridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(segmentLineRepeaterX()->objectAt(i + gridLineCountX));
        linePosZ = axisX->gridPositionAt(i) * -scale + scaleOffset().x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
    }

    for (int i  = 0; i < subGridLineCountX; i++) {
        QQuick3DNode *lineNode = static_cast<QQuick3DNode *>(subsegmentLineRepeaterX()->objectAt(i + subGridLineCountX));
        linePosZ = axisX->subGridPositionAt(i) * -scale + scaleOffset().x();
        positionAndScaleLine(lineNode, scaleY, QVector3D(linePosX, linePosY, linePosZ));
    }
}

void QQuickDataVisSurface::updateLabels()
{
    auto axisX = static_cast<QValue3DAxis *>(m_surfaceController->axisX());
    auto labels = axisX->labels();
    float labelAutoAngle = axisX->labelAutoRotation();
    float labelAngleFraction = labelAutoAngle / 90.0f;
    float fractionCamX = m_surfaceController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    float fractionCamY = m_surfaceController->scene()->activeCamera()->yRotation() * labelAngleFraction;

    QVector3D labelRotation;

    float xPos;
    float yPos;
    float zPos;

    if (labelAutoAngle == 0.0f) {
        labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
        if (flipped().x())
            labelRotation.setY(-90.0f);
        if (flipped().y()) {
            if (flipped().x())
                labelRotation.setY(-90.0f);
            else
                labelRotation.setY(90.0f);
            labelRotation.setX(90.0f);
        }
    } else {
        if (flipped().x())
            labelRotation.setY(-90.0f);
        else
            labelRotation.setY(90.0f);
        if (flipped().y()) {
            if (flipped().z()) {
                if (flipped().x()) {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (2.0f * labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            } else {
                if (flipped().x()) {
                    labelRotation.setX(90.0f + fractionCamX
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f - fractionCamX
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            }
        } else {
            if (flipped().z()) {
                if (flipped().x()) {
                    labelRotation.setX(-90.0f + (2.0f * labelAutoAngle - fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + (2.0f * labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                }
            } else {
                if (flipped().x()) {
                    labelRotation.setX(-90.0f - fractionCamX
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + fractionCamX
                                       * -(labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            }
        }
    }
    if (!flipped().x()) {
        if (!flipped().y())
            yPos = -scaleWithBackground().y();
        else
            yPos = scaleWithBackground().y();
    } else {
        if (!flipped().y())
            yPos = -scaleWithBackground().y();
        else
            yPos = scaleWithBackground().y();
    }

    if (flipped().z())
        zPos = -scaleWithBackground().z() - labelMargin();
    else
        zPos = scaleWithBackground().z() + labelMargin();

    auto scale = this->scale().x() * scaleWithBackground().x() + scaleOffset().x();
    auto totalRotation = Utils::calculateRotation(labelRotation);
    auto labelTrans = QVector3D(0.0f, yPos, zPos);
    float labelsMaxWidth = 0.0f;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisX->labels())));

    for (int i = 0; i < repeaterX()->count(); i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterX()->objectAt(i));
        labelTrans.setX(axisX->labelPositionAt(i) * scale - scaleOffset().x());
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    if (titleLabelX()->visible()) {
        float x = labelTrans.x();
        labelTrans.setX(0.0f);
        updateXTitle(labelRotation, labelTrans, totalRotation,labelsMaxWidth);
        labelTrans.setX(x);
    }

    auto axisY = static_cast<QValue3DAxis *>(m_surfaceController->axisY());
    labels = axisY->labels();
    labelAutoAngle = axisY->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_surfaceController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_surfaceController->scene()->activeCamera()->yRotation() * labelAngleFraction;

    QVector3D sideLabelRotation(0.0f, -90.0f, 0.0f);
    QVector3D backLabelRotation(0.0f, 0.0f, 0.0f);

    if (labelAutoAngle == 0.0f) {
        if (!flipped().x())
            sideLabelRotation.setY(90.0f);
        if (flipped().z())
            backLabelRotation.setY(180.f);
    } else {
        // Orient side labels somewhat towards the camera
        if (flipped().x()) {
            if (flipped().z())
                backLabelRotation.setY(180.0f + (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(-90.0f + labelAutoAngle - fractionCamX);
        } else {
            if (flipped().z())
                backLabelRotation.setY(180.0f - (2.0f * labelAutoAngle) - fractionCamX);
            else
                backLabelRotation.setY(-fractionCamX);
            sideLabelRotation.setY(90.0f - labelAutoAngle - fractionCamX);
        }
    }

    if (flipped().x())
        xPos = -scaleWithBackground().x() - labelMargin();
    else
        xPos = scaleWithBackground().x() + labelMargin();

    if (flipped().z())
        zPos = scaleWithBackground().z() + labelMargin();
    else
        zPos = -scaleWithBackground().z() - labelMargin();

    backLabelRotation.setX(-fractionCamY);
    sideLabelRotation.setX(-fractionCamY);

    totalRotation = Utils::calculateRotation(sideLabelRotation);
    scale = this->scale().y() * scaleWithBackground().y() + scaleOffset().y();

    labelsMaxWidth = 0;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(m_surfaceController->axisY()->labels())));

    for (int i = 0; i < repeaterY()->count()/2; i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i));
        labelTrans.setY(axisY->labelPositionAt(i) * scale - scaleOffset().y());
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    auto sideLabelTrans = labelTrans;
    auto totalSideLabelRotation = totalRotation;

    auto axisZ = static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
    labels = axisZ->labels();

    labelAutoAngle = axisZ->labelAutoRotation();
    labelAngleFraction = labelAutoAngle / 90.0f;
    fractionCamX = m_surfaceController->scene()->activeCamera()->xRotation() * labelAngleFraction;
    fractionCamY = m_surfaceController->scene()->activeCamera()->yRotation() * labelAngleFraction;

    if (labelAutoAngle == 0.0f) {
        labelRotation = QVector3D(90.0f, 0.0f, 0.0f);
        if (flipped().z())
            labelRotation.setY(180.0f);
        if (flipped().y()) {
            if (flipped().z())
                labelRotation.setY(180.0f);
            else
                labelRotation.setY(0.0f);
            labelRotation.setX(90.0f);
        } else {
            labelRotation.setX(-90.0f);
        }
    } else {
        if (flipped().z())
            labelRotation.setY(180.0f);
        if (flipped().y()) {
            if (flipped().z()) {
                if (flipped().x()) {
                    labelRotation.setX(90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                }
            } else {
                if (flipped().x()) {
                    labelRotation.setX(90.0f + (labelAutoAngle - fractionCamX)
                                       * -(labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle + fractionCamY);
                }
            }
        } else {
            if (flipped().z()) {
                if (flipped().x()) {
                    labelRotation.setX(-90.0f + (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                } else {
                    labelRotation.setX(-90.0f - (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                }
            } else {
                if (flipped().x()) {
                    labelRotation.setX(-90.0f - (labelAutoAngle - fractionCamX)
                                       * (-labelAutoAngle + fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(labelAutoAngle - fractionCamY);
                } else {
                    labelRotation.setX(-90.0f + (labelAutoAngle + fractionCamX)
                                       * (labelAutoAngle - fractionCamY) / labelAutoAngle);
                    labelRotation.setZ(-labelAutoAngle + fractionCamY);
                }
            }
        }
    }

    if (flipped().x())
        xPos = -scaleWithBackground().x() - labelMargin();
    else
        xPos = scaleWithBackground().x() + labelMargin();

    if (!flipped().z()) {
        if (!flipped().y())
            yPos = -scaleWithBackground().y();
        else
            yPos = scaleWithBackground().y();
    }
    else {
        if (!flipped().y())
            yPos = -scaleWithBackground().y();
        else
            yPos = scaleWithBackground().y();
    }
    totalRotation = Utils::calculateRotation(labelRotation);
    labelTrans = QVector3D(xPos, yPos, 0);

    scale = -this->scale().z() * scaleWithBackground().z() - scaleOffset().z();
    labelsMaxWidth = 0;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisZ->labels())));

    for (int i = 0; i < repeaterZ()->count(); i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterZ()->objectAt(i));
        labelTrans.setZ(axisZ->labelPositionAt(i) * scale + scaleOffset().z());
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    if (titleLabelZ()->visible()) {
        float z = labelTrans.z();
        labelTrans.setZ(0.0f);
        updateZTitle(labelRotation, labelTrans, totalRotation,labelsMaxWidth);
        labelTrans.setZ(z);
    }

    labels = axisY->labels();
    totalRotation = Utils::calculateRotation(backLabelRotation);
    scale = this->scale().y() * scaleWithBackground().y() + scaleOffset().y();
    labelsMaxWidth = 0;
    labelsMaxWidth = qMax(labelsMaxWidth, float(findLabelsMaxWidth(axisY->labels())));

    for (int i = 0; i < repeaterY()->count() / 2; i++) {
        auto obj = static_cast<QQuick3DNode *>(repeaterY()->objectAt(i + (repeaterY()->count() / 2)));
        labelTrans.setY(axisY->labelPositionAt(i) * scale - scaleOffset().y());
        obj->setPosition(labelTrans);
        obj->setRotation(totalRotation);
        obj->setProperty("labelText", labels[i]);
        obj->setProperty("labelWidth", labelsMaxWidth);
    }

    auto backLabelTrans = labelTrans;
    auto totalBackLabelRotation = totalRotation;
    if (titleLabelY()->visible()) {
        updateYTitle(sideLabelRotation, backLabelRotation,
                     sideLabelTrans,    backLabelTrans,
                     totalSideLabelRotation, totalBackLabelRotation, labelsMaxWidth);
    }

}

QT_END_NAMESPACE
