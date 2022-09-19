// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qquickdatavissurface_p.h"
#include <QtCore/QMutexLocker>

#include "declarativescene_p.h"
#include "surface3dcontroller_p.h"
#include "utils_p.h"

#include <QtQuick3D/private/qquick3drepeater_p.h>
#include <QtQuick3D/private/qquick3dprincipledmaterial_p.h>

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

inline static float getDataValue(const QSurfaceDataArray &array, bool searchRow, int index)
{
    if (searchRow)
        return array.at(0)->at(index).x();
    else
        return array.at(index)->at(0).z();
}

inline static int binarySearchArray(const QSurfaceDataArray &array, int maxIndex, float limitValue, bool searchRow, bool lowBound, bool ascending)
{
    int min = 0;
    int max = maxIndex;
    int mid = 0;
    int retVal;

    while (max >= min) {
        mid = (min + max) / 2;
        float arrayValue = getDataValue(array, searchRow, mid);
        if (arrayValue == limitValue)
            return mid;
        if (ascending) {
            if (arrayValue < limitValue)
                min = mid + 1;
            else
                max = mid -1;
        } else {
            if (arrayValue > limitValue)
                min = mid + 1;
            else
                max = mid - 1;
        }
    }

    if (lowBound == ascending) {
        if (mid > max)
            retVal = mid;
        else
            retVal = min;
    } else {
        if (mid > max)
            retVal = max;
        else
            retVal = mid;
    }

    if (retVal < 0 || retVal > maxIndex) {
        retVal = -1;
    } else if (lowBound) {
        if (getDataValue(array, searchRow, retVal) < limitValue)
            retVal = -1;
    } else {
        if (getDataValue(array, searchRow, retVal) > limitValue)
            retVal = -1;
    }
    return retVal;
}

void QQuickDataVisSurface::updateGraph()
{
    m_gridModel->setVisible(m_surfaceController->visibleSeries()->drawMode().testFlag(QSurface3DSeries::DrawWireframe));
    m_model->setVisible(m_surfaceController->visibleSeries()->drawMode().testFlag(QSurface3DSeries::DrawSurface));

    const QSurfaceDataArray &array = *(m_surfaceController->visibleSeries()->dataProxy())->array();

    // calculateSampleRect
    QRect sampleSpace;
    if (array.size() > 0) {
        if (array.size() >= 2 && array.at(0)->size() >= 2) {
            const int maxRow = array.size() - 1;
            const int maxColumn = array.at(0)->size() - 1;

            const bool ascendingX = array.at(0)->at(0).x() < array.at(0)->at(maxColumn).x();
            const bool ascendingZ = array.at(0)->at(0).z() < array.at(maxRow)->at(0).z();

            int idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->min(), true, true, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setLeft(idx);
                else
                    sampleSpace.setRight(idx);
            } else {
                sampleSpace.setWidth(-1);
            }

            idx = binarySearchArray(array, maxColumn, m_surfaceController->axisX()->max(), true, false, ascendingX);
            if (idx != -1) {
                if (ascendingX)
                    sampleSpace.setRight(idx);
                else
                    sampleSpace.setLeft(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->min(), false, true, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setTop(idx);
                else
                    sampleSpace.setBottom(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }

            idx = binarySearchArray(array, maxRow, m_surfaceController->axisZ()->max(), false, false, ascendingZ);
            if (idx != -1) {
                if (ascendingZ)
                    sampleSpace.setBottom(idx);
                else
                    sampleSpace.setTop(idx);
            } else {
                sampleSpace.setWidth(-1); // to indicate nothing needs to be shown
            }
        }

        int rowCount = sampleSpace.height();
        int columnCount = sampleSpace.width();
        m_surfaceController->setRowCount(rowCount);
        m_surfaceController->setColumnCount(columnCount);

        int totalSize = rowCount * columnCount * 2;
        float uvX = 1.0f / float(columnCount - 1);
        float uvY = 1.0f / float(rowCount - 1);

        // checkDirection
        int dataDimensions = Surface3DController::BothAscending;
        if (array.at(0)->at(0).x() > array.at(0)->at(array.at(0)->size() - 1).x())
            dataDimensions |= Surface3DController::XDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisX())->reversed())
            dataDimensions ^= Surface3DController::XDescending;

        if (array.at(0)->at(0).z() > array.at(array.size() - 1)-> at(0).z())
            dataDimensions |= Surface3DController::ZDescending;
        if (static_cast<QValue3DAxis *>(m_surfaceController->axisZ())->reversed())
            dataDimensions ^= Surface3DController::ZDescending;

        m_surfaceController->setDataDimensions(static_cast<Surface3DController::DataDimensions>(dataDimensions));

        m_vertices.reserve(totalSize);

        float minY = qInf();
        float maxY = qInf();

        m_vertices.clear();
        m_height.clear();
        for (int i = 0 ; i < rowCount ; i++) {
            const QSurfaceDataRow &row = *array.at(i);
            for (int j = 0 ; j < columnCount ; j++) {
                // getNormalizedVertex
                SurfaceVertex vertex;
                vertex.position = getNormalizedVertex(row.at(j), false, false);
                if (qIsInf(maxY))
                    maxY = m_height.last();
                else
                    maxY = qMax(m_height.last(), maxY);
                if (qIsInf(minY))
                    minY = m_height.last();
                else
                    minY = qMin(m_height.last(), minY);
                vertex.normal = QVector3D(0, 0, 0);
                vertex.uv = QVector2D(j * uvX, i * uvY);
                m_vertices.push_back(vertex);
            }
        }

        //create normals
        int rowLimit = rowCount - 1;
        int colLimit = columnCount - 1;

        int totalIndex = 0;

        if (dataDimensions == Surface3DController::BothAscending || dataDimensions == Surface3DController::XDescending) {
            for (int row = 0 ; row < rowLimit ; row++)
                createSmoothNormalBodyLine(totalIndex, row * columnCount);
            createSmoothNormalUpperLine(totalIndex);
        }
        else {
            createSmoothNormalUpperLine(totalIndex);
            for (int row = 1 ; row < rowCount ; row++) {
                createSmoothNormalBodyLine(totalIndex, row * columnCount);
            }
        }

        createSmoothIndices(0, 0, colLimit, rowLimit);

        QQuick3DGeometry *geometry = new QQuick3DGeometry();
        QByteArray vertexBuffer(reinterpret_cast<char *>(m_vertices.data()), m_vertices.size() * sizeof(SurfaceVertex));
        geometry->setVertexData(vertexBuffer);
        geometry->setStride(sizeof(SurfaceVertex));
        QByteArray indexBuffer(reinterpret_cast<char *>(m_indices.data()), m_indices.size() * sizeof(quint32));
        geometry->setIndexData(indexBuffer);
        geometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);
        geometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                               0,
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::TexCoord0Semantic,
                               sizeof(QVector3D) * 2,
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                               sizeof(QVector3D),
                               QQuick3DGeometry::Attribute::F32Type);
        geometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                               0,
                               QQuick3DGeometry::Attribute::U32Type);
        m_model->setGeometry(geometry);

        // TODO :: create material regarding height value
        QQmlListReference materialRef(m_model, "materials");
        QQuick3DPrincipledMaterial* material = new QQuick3DPrincipledMaterial();
        material->setBaseColor(QColor::fromString("#ffffff"));
        material->setRoughness(0.3f);
        material->setSpecularAmount(0.6f);
        materialRef.append(material);

        createSmoothGridlineIndices(0, 0, colLimit, rowLimit);

        QQuick3DGeometry *gridGeometry = new QQuick3DGeometry();
        gridGeometry->setVertexData(vertexBuffer);
        gridGeometry->setStride(sizeof(SurfaceVertex));
        QByteArray gridIndexBuffer(reinterpret_cast<char *>(m_gridIndices.data()), m_gridIndices.size() * sizeof(quint32));
        gridGeometry->setIndexData(gridIndexBuffer);
        gridGeometry->setPrimitiveType(QQuick3DGeometry::PrimitiveType::Lines);
        gridGeometry->addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                                   0,
                                   QQuick3DGeometry::Attribute::F32Type);
        gridGeometry->addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                                   0,
                                   QQuick3DGeometry::Attribute::U32Type);
        m_gridModel->setGeometry(gridGeometry);

        // TODO :: create material regarding grid color value
        QQmlListReference gridMaterialRef(m_gridModel, "materials");
        QQuick3DPrincipledMaterial* gridMaterial = new QQuick3DPrincipledMaterial();
        gridMaterial->setBaseColor(QColor::fromString("#000000"));
        gridMaterial->setRoughness(0.3f);
        gridMaterial->setSpecularAmount(0.6f);
        gridMaterialRef.append(gridMaterial);
    }
}

QVector3D QQuickDataVisSurface::getNormalizedVertex(const QSurfaceDataItem &data, bool polar, bool flipXZ)
{
    Q_UNUSED(polar);
    Q_UNUSED(flipXZ);

    float normalizedX;
    float normalizedY;
    float normalizedZ;
    QValue3DAxis* axisX = static_cast<QValue3DAxis *>(m_surfaceController->axisX());
    QValue3DAxis* axisY = static_cast<QValue3DAxis *>(m_surfaceController->axisY());
    QValue3DAxis* axisZ = static_cast<QValue3DAxis *>(m_surfaceController->axisZ());
    // TODO : Need to handle polar, flipXZ
    float scale = this->scale().x() * scaleWithBackground().x() + scaleOffset().x();
    float translate = scaleOffset().x();
    normalizedX = axisX->positionAt(data.x()) * scale - translate;
    scale = this->scale().y() * scaleWithBackground().y() + scaleOffset().y();
    translate = scaleOffset().y();
    m_height.push_back(axisY->positionAt(data.y()));
    normalizedY = axisY->positionAt(data.y()) * scale - translate;
    scale = this->scale().z() * scaleWithBackground().z() + scaleOffset().z();
    translate = scaleOffset().z();
    normalizedZ = axisZ->positionAt(data.z()) * -scale + translate;
    return QVector3D(normalizedX, normalizedY, normalizedZ);
}

inline static QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c)
{
    QVector3D v1 = b - a;
    QVector3D v2 = c - a;
    QVector3D normal = QVector3D::crossProduct(v1, v2);

    return normal;
}

void QQuickDataVisSurface::createSmoothNormalBodyLine(int &totalIndex, int column)
{
    int columnCount = m_surfaceController->columnCount();
    int colLimit = columnCount - 1;
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();
    if (dataDimensions == Surface3DController::BothAscending) {
        int end = colLimit + column;
        for (int j = column ; j < end ; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j + 1).position,
                                   m_vertices.at(j + columnCount).position);
        }
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(end).position,
                               m_vertices.at(end + columnCount).position,
                               m_vertices.at(end - 1).position);
    } else if (dataDimensions == Surface3DController::XDescending) {
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(column).position,
                               m_vertices.at(column + columnCount).position,
                               m_vertices.at(column + 1).position);
        int end = column + columnCount;
        for (int j = column + 1 ; j < end ; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j - 1).position,
                                   m_vertices.at(j + columnCount).position);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int end = colLimit + column;
        for (int j = column; j < end ; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j + 1).position,
                                   m_vertices.at(j - columnCount).position);
        }
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(end).position,
                               m_vertices.at(end - columnCount).position,
                               m_vertices.at(end - 1).position);
    } else {
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(column).position,
                               m_vertices.at(column - columnCount).position,
                               m_vertices.at(column + 1).position);
        int end = column + columnCount;
        for (int j = 0 ; j < end ; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j-1).position,
                                   m_vertices.at(j - columnCount).position);
        }
    }
}

void QQuickDataVisSurface::createSmoothNormalUpperLine(int &totalIndex)
{
    int columnCount = m_surfaceController->columnCount();
    int rowCount = m_surfaceController->rowCount();
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (dataDimensions == Surface3DController::BothAscending) {
        int lineEnd = rowCount * columnCount - 1;
        for (int j = (rowCount - 1) * columnCount; j < lineEnd; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j - columnCount).position,
                                   m_vertices.at(j + 1).position);
        }
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(lineEnd).position,
                               m_vertices.at(lineEnd - 1).position,
                               m_vertices.at(lineEnd - columnCount).position);
    } else if (dataDimensions == Surface3DController::XDescending) {
        int lineStart = (rowCount - 1) * columnCount;
        int lineEnd = rowCount * columnCount;
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(lineStart).position,
                               m_vertices.at(lineStart + 1).position,
                               m_vertices.at(lineStart - columnCount).position);
        for (int j = lineStart + 1; j < lineEnd; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j - columnCount).position,
                                   m_vertices.at(j - 1).position);
        }
    } else if (dataDimensions == Surface3DController::ZDescending) {
        int colLimit = columnCount - 1;
        for (int j = 0; j < colLimit; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j + columnCount).position,
                                   m_vertices.at(j + 1).position);
        }
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(colLimit).position,
                               m_vertices.at(colLimit - 1).position,
                               m_vertices.at(colLimit + columnCount).position);
    } else { // BothDescending
        SurfaceVertex vertex = m_vertices.at(totalIndex++);
        vertex.normal = normal(m_vertices.at(0).position,
                               m_vertices.at(1).position,
                               m_vertices.at(columnCount).position);
        for (int j = 1; j < columnCount; j++) {
            SurfaceVertex vertex = m_vertices.at(totalIndex++);
            vertex.normal = normal(m_vertices.at(j).position,
                                   m_vertices.at(j + columnCount).position,
                                   m_vertices.at(j - 1).position);
        }
    }
}

void QQuickDataVisSurface::createSmoothIndices(int x, int y, int endX, int endY)
{
    int columnCount = m_surfaceController->columnCount();
    int rowCount = m_surfaceController->rowCount();
    Surface3DController::DataDimensions dataDimensions = m_surfaceController->dataDimensions();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int indexCount = 6 * (endX - x) * (endY - y);
    m_indices.resize(indexCount);

    int rowEnd = endY * columnCount;
    for (int row = y * columnCount ; row < rowEnd ; row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            if (dataDimensions == Surface3DController::BothAscending
                    || dataDimensions == Surface3DController::BothDescending) {
                m_indices.push_back(row + j + 1);
                m_indices.push_back(row + columnCount + j);
                m_indices.push_back(row + j);

                m_indices.push_back(row + columnCount + j + 1);
                m_indices.push_back(row + columnCount + j);
                m_indices.push_back(row + j + 1);
            } else if (dataDimensions == Surface3DController::XDescending) {
                m_indices.push_back(row + columnCount + j);
                m_indices.push_back(row + columnCount + j + 1);
                m_indices.push_back(row + j);

                m_indices.push_back(row + j);
                m_indices.push_back(row + columnCount + j + 1);
                m_indices.push_back(row + j + 1);
            } else {
                m_indices.push_back(row + columnCount + j);
                m_indices.push_back(row + columnCount + j + 1);
                m_indices.push_back(row + j + 1);

                m_indices.push_back(row + j);
                m_indices.push_back(row + columnCount + j + 1);
                m_indices.push_back(row + j + 1);
            }
        }
    }
}

void QQuickDataVisSurface::createSmoothGridlineIndices(int x, int y, int endX, int endY)
{
    int columnCount = m_surfaceController->columnCount();
    int rowCount = m_surfaceController->rowCount();

    if (endX >= columnCount)
        endX = columnCount - 1;
    if (endY >= rowCount)
        endY = rowCount - 1;
    if (x > endX)
        x = endX - 1;
    if (y > endY)
        y = endY - 1;

    int nColumns = endX - x + 1;
    int nRows = endY - y + 1;

    int m_gridIndexCount = 2 * nColumns * (nRows - 1) + 2 * nRows * (nColumns - 1);
    m_gridIndices.resize(m_gridIndexCount);
    for (int i = y, row = columnCount * y ; i <= endY ; i++, row += columnCount) {
        for (int j = x ; j < endX ; j++) {
            m_gridIndices.push_back(row + j);
            m_gridIndices.push_back(row + j + 1);
        }
    }
    for (int i = y, row = columnCount * y ; i < endY ; i++, row += columnCount) {
        for (int j = x ; j <= endX ; j++) {
            m_gridIndices.push_back(row + j);
            m_gridIndices.push_back(row + j + columnCount);
        }
    }
}

QT_END_NAMESPACE
