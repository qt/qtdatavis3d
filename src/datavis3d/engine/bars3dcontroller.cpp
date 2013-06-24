/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDataVis3D module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "bars3dcontroller_p.h"
#include "bars3drenderer_p.h"
#include "camerahelper_p.h"
#include "qdataitem_p.h"
#include "qdatarow_p.h"
#include "qdataset_p.h"
#include "theme_p.h"
#include "utils_p.h"

#include <QMouseEvent>
#include <QThread>
#include <qmath.h>

#include <QDebug>

QT_DATAVIS3D_BEGIN_NAMESPACE

Bars3dController::Bars3dController(QRect rect)
    : m_renderer(new Bars3dRenderer(rect, this)),
      m_isInitialized(false),
      m_dataSet(new QDataSet()),
      m_sampleCount(0, 0)
{
    m_renderer->dataSetChangedNotify(m_dataSet->d_ptr.data());
}

Bars3dController::~Bars3dController()
{
    delete m_dataSet;
}

void Bars3dController::initializeOpenGL()
{
    // Initialization is called multiple times when Qt Quick components are used
    if (m_isInitialized)
        return;

    m_renderer->initializeOpenGL();
    m_isInitialized = true;
}

void Bars3dController::render(const GLuint defaultFboHandle)
{
    if (!m_isInitialized)
        return;

    LabelItem x;
    LabelItem z;
    LabelItem y;
    m_dataSet->d_ptr->axisLabelItems(&x, &z, &y);

    m_renderer->render(m_dataSet->d_ptr.data(), x, y, z, defaultFboHandle);
}

#if defined(Q_OS_ANDROID)
void Bars3dController::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_renderer->mouseDoubleClickEvent(event);
}

void Bars3dController::touchEvent(QTouchEvent *event)
{
    m_renderer->touchEvent(Qevent);
}
#endif

void Bars3dController::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
{
    m_renderer->mousePressEvent(event, mousePos);
}

void Bars3dController::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
{
    m_renderer->mouseReleaseEvent(event, mousePos);
}

void Bars3dController::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
{
    m_renderer->mouseMoveEvent(event, mousePos);
}

void Bars3dController::wheelEvent(QWheelEvent *event)
{
    m_renderer->wheelEvent(event);
}

void Bars3dController::resizeNotify()
{
    m_renderer->resizeNotify();
}

void Bars3dController::setBarSpecs(QSizeF thickness, QSizeF spacing, bool relative)
{
    m_renderer->setBarSpecs(thickness, spacing, relative);
}

void Bars3dController::setBarType(BarStyle style, bool smooth)
{
    m_renderer->setBarType(style, smooth);
}

void Bars3dController::setMeshFileName(const QString &objFileName)
{
    m_renderer->setMeshFileName(objFileName);
}

void Bars3dController::setupSampleSpace(int samplesRow, int samplesColumn, const QString &labelRow,
                                        const QString &labelColumn, const QString &labelHeight)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    m_renderer->closeZoomMode();

    // Recreate data set
    delete m_dataSet;
    m_dataSet = new QDataSet();
    m_renderer->dataSetChangedNotify(m_dataSet->d_ptr.data());

    m_sampleCount = qMakePair(samplesRow, samplesColumn);
    m_dataSet->setLabels(labelRow, labelColumn, labelHeight);

    m_renderer->sampleSpaceChangedNotify(samplesRow, samplesColumn);
}

void Bars3dController::setCameraPreset(CameraPreset preset)
{
    m_renderer->setCameraPreset(preset);
}

void Bars3dController::setCameraPosition(GLfloat horizontal, GLfloat vertical, GLint distance)
{
    m_renderer->setCameraPosition(horizontal, vertical, distance);
}

void Bars3dController::setTheme(ColorTheme theme)
{
    m_renderer->setTheme(theme);
}

void Bars3dController::setBarColor(QColor baseColor, QColor heightColor, QColor depthColor,
                                   bool uniform)
{
    m_renderer->setBarColor(baseColor, heightColor, depthColor, uniform);
}

void Bars3dController::setSelectionMode(SelectionMode mode)
{
    m_renderer->setSelectionMode(mode);
}

SelectionMode Bars3dController::selectionMode()
{
    return m_renderer->selectionMode();
}

void Bars3dController::setFontSize(float fontsize)
{
    m_renderer->setFontSize(fontsize);
}

float Bars3dController::fontSize()
{
    return m_renderer->fontSize();
}

void Bars3dController::setFont(const QFont &font)
{
    m_renderer->setFont(font);
}

QFont Bars3dController::font()
{
    return m_renderer->font();
}

void Bars3dController::setLabelTransparency(LabelTransparency transparency)
{
    m_renderer->setLabelTransparency(transparency);
}

LabelTransparency Bars3dController::labelTransparency()
{
    return m_renderer->labelTransparency();
}

void Bars3dController::setGridEnabled(bool enable)
{
    m_renderer->setGridEnabled(enable);
}

bool Bars3dController::gridEnabled()
{
    return m_renderer->gridEnabled();
}

void Bars3dController::setBackgroundEnabled(bool enable)
{
    m_renderer->setBackgroundEnabled(enable);
}

bool Bars3dController::backgroundEnabled()
{
    return m_renderer->backgroundEnabled();
}

void Bars3dController::setShadowQuality(ShadowQuality quality)
{
    m_renderer->setShadowQuality(quality);
}

ShadowQuality Bars3dController::shadowQuality()
{
    return m_renderer->shadowQuality();
}

void Bars3dController::setTickCount(GLint tickCount, GLfloat step, GLfloat minimum)
{
    m_renderer->setTickCount(tickCount, step, minimum);
}

int Bars3dController::getColumnCount()
{
    return m_sampleCount.first;
}

int Bars3dController::getRowCount()
{
    return m_sampleCount.second;
}

void Bars3dController::handleLimitChange()
{
    // Get the limits
    QPair<GLfloat, GLfloat> limits = m_dataSet->d_ptr->limitValues();

    m_renderer->limitsChangedNotify(limits);
}

void Bars3dController::addDataRow(const QVector<float> &dataRow, const QString &labelRow,
                                  const QVector<QString> &labelsColumn)
{
    // Copy axis labels. TODO: Separate axis labels from data
    QString xAxis;
    QString zAxis;
    QString yAxis;
    m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);

    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(new QDataItem(dataRow.at(i)));
    row->d_ptr->verifySize(m_sampleCount.first);
    m_dataSet->addRow(row);
    handleLimitChange();

    // TODO: Separate axis labels from data
    m_dataSet->setLabels(xAxis, zAxis, yAxis,
                         QVector<QString>(), labelsColumn);
    m_dataSet->d_ptr->verifySize(m_sampleCount.second);
}

void Bars3dController::addDataRow(const QVector<QDataItem*> &dataRow, const QString &labelRow,
                                  const QVector<QString> &labelsColumn)
{
    // Copy axis labels
    QString xAxis;
    QString zAxis;
    QString yAxis;
    m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);

    // Convert to QDataRow and add to QDataSet
    QDataRow *row = new QDataRow(labelRow);
    for (int i = 0; i < dataRow.size(); i++)
        row->addItem(dataRow.at(i));
    row->d_ptr->verifySize(m_sampleCount.first);
    m_dataSet->addRow(row);
    handleLimitChange();

    // TODO: Separate axis labels from data
    m_dataSet->setLabels(xAxis, zAxis, yAxis,
                         QVector<QString>(), labelsColumn);
    m_dataSet->d_ptr->verifySize(m_sampleCount.second);
}

void Bars3dController::addDataRow(QDataRow *dataRow)
{
    // Check that the input data fits into sample space, and resize if it doesn't
    dataRow->d_ptr->verifySize(m_sampleCount.first);
    // With each new row, the previous data row must be moved back
    // ie. we need as many vectors as we have rows in the sample space
    m_dataSet->addRow(dataRow);
    // if the added data pushed us over sample space, remove the oldest data set
    m_dataSet->d_ptr->verifySize(m_sampleCount.second);
    handleLimitChange();
}

void Bars3dController::addDataSet(const QVector< QVector<float> > &data,
                                  const QVector<QString> &labelsRow,
                                  const QVector<QString> &labelsColumn)
{
    // Copy axis labels
    QString xAxis;
    QString zAxis;
    QString yAxis;
    m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);

    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    m_renderer->closeZoomMode();
    // Delete old data set
    delete m_dataSet;
    m_dataSet = new QDataSet();
    m_renderer->dataSetChangedNotify(m_dataSet->d_ptr.data());

    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        if (labelsRow.size() >= (rowNr + 1))
            row = new QDataRow(labelsRow.at(rowNr));
        else
            row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(new QDataItem(data.at(rowNr).at(colNr)));
        row->d_ptr->verifySize(m_sampleCount.first);
        m_dataSet->addRow(row);
        row++;
    }
    handleLimitChange();
    m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    m_dataSet->d_ptr->verifySize(m_sampleCount.second);
}

void Bars3dController::addDataSet(const QVector< QVector<QDataItem*> > &data,
                                  const QVector<QString> &labelsRow,
                                  const QVector<QString> &labelsColumn)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    m_renderer->closeZoomMode();

    // Copy axis labels
    QString xAxis;
    QString zAxis;
    QString yAxis;
    m_dataSet->d_ptr->axisLabels(&xAxis, &zAxis, &yAxis);

    // Delete old data set
    delete m_dataSet;
    m_dataSet = new QDataSet();
    m_renderer->dataSetChangedNotify(m_dataSet->d_ptr.data());

    // Convert to QDataRow and add to QDataSet
    QDataRow *row;
    for (int rowNr = 0; rowNr < data.size(); rowNr++) {
        if (labelsRow.size() >= (rowNr + 1))
            row = new QDataRow(labelsRow.at(rowNr));
        else
            row = new QDataRow();
        for (int colNr = 0; colNr < data.at(rowNr).size(); colNr++)
            row->addItem(data.at(rowNr).at(colNr));
        row->d_ptr->verifySize(m_sampleCount.first);
        m_dataSet->addRow(row);
        row++;
    }
    handleLimitChange();
    m_dataSet->setLabels(xAxis, zAxis, yAxis, labelsRow, labelsColumn);
    m_dataSet->d_ptr->verifySize(m_sampleCount.second);
}

void Bars3dController::addDataSet(QDataSet* dataSet)
{
    // Disable zoom mode if we're in it (causes crash if not, as zoom selection is deleted)
    m_renderer->closeZoomMode();

    // Check sizes
    dataSet->d_ptr->verifySize(m_sampleCount.second, m_sampleCount.first);

    // Delete old data set
    delete m_dataSet;
    // Take ownership of given set
    m_dataSet = dataSet;
    handleLimitChange();

    m_renderer->dataSetChangedNotify(m_dataSet->d_ptr.data());
}

const QSize Bars3dController::size()
{
    return m_renderer->size();
}

const QRect Bars3dController::boundingRect()
{
    return m_renderer->boundingRect();
}

void Bars3dController::setBoundingRect(const QRect boundingRect)
{
    m_renderer->setBoundingRect(boundingRect);
}

void Bars3dController::setWidth(const int width)
{
    m_renderer->setWidth(width);
}

int Bars3dController::width()
{
    return m_renderer->width();
}

void Bars3dController::setHeight(const int height)
{
    m_renderer->setHeight(height);
}

int Bars3dController::height()
{
    return m_renderer->height();
}

void Bars3dController::setX(const int x)
{
    m_renderer->setX(x);
}

int Bars3dController::x()
{
    return m_renderer->x();
}

void Bars3dController::setY(const int y)
{
    m_renderer->setY(y);
}

int Bars3dController::y()
{
    return m_renderer->y();
}

void Bars3dController::updateTextures()
{
    // Drawer has changed; this flag needs to be checked when checking if we need to update labels
    m_renderer->updateTextures();
}

Bars3dRenderer::SelectionType Bars3dController::isSelected(GLint row, GLint bar,
                                                             const QVector3D &selection)
{
    return m_renderer->isSelected(row, bar, selection);
}

QT_DATAVIS3D_END_NAMESPACE
