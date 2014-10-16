/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the QtDataVisualization module.
**
** Licensees holding valid Qt Enterprise licenses may use this file in
** accordance with the Qt Enterprise License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
**
****************************************************************************/

import QtQuick 2.0
import QtDataVisualization 1.2
import QtTest 1.0

Item {
    id: top
    height: 150
    width: 150

    ItemModelScatterDataProxy {
        id: initial
    }

    ItemModelScatterDataProxy {
        id: initialized

        itemModel: ListModel { objectName: "model1" }
        rotationRole: "rot"
        rotationRolePattern: /-/
        rotationRoleReplace: "\\1"
        xPosRole: "x"
        xPosRolePattern: /^.*-(\d\d)$/
        xPosRoleReplace: "\\1"
        yPosRole: "y"
        yPosRolePattern: /^(\d\d\d\d).*$/
        yPosRoleReplace: "\\1"
        zPosRole: "z"
        zPosRolePattern: /-/
        zPosRoleReplace: "\\1"
    }

    ItemModelScatterDataProxy {
        id: change
    }

    TestCase {
        name: "ItemModelScatterDataProxy Initial"

        function test_initial() {
            verify(!initial.itemModel)
            compare(initial.rotationRole, "")
            verify(initial.rotationRolePattern)
            compare(initial.rotationRoleReplace, "")
            compare(initial.xPosRole, "")
            verify(initial.xPosRolePattern)
            compare(initial.xPosRoleReplace, "")
            compare(initial.yPosRole, "")
            verify(initial.yPosRolePattern)
            compare(initial.yPosRoleReplace, "")
            compare(initial.zPosRole, "")
            verify(initial.zPosRolePattern)
            compare(initial.zPosRoleReplace, "")

            compare(initial.itemCount, 0)
            verify(!initial.series)

            compare(initial.type, AbstractDataProxy.DataTypeScatter)
        }
    }

    TestCase {
        name: "ItemModelScatterDataProxy Initialized"

        function test_initialized() {
            compare(initialized.itemModel.objectName, "model1")
            compare(initialized.rotationRole, "rot")
            compare(initialized.rotationRolePattern, /-/)
            compare(initialized.rotationRoleReplace, "\\1")
            compare(initialized.xPosRole, "x")
            compare(initialized.xPosRolePattern, /^.*-(\d\d)$/)
            compare(initialized.xPosRoleReplace, "\\1")
            compare(initialized.yPosRole, "y")
            compare(initialized.yPosRolePattern, /^(\d\d\d\d).*$/)
            compare(initialized.yPosRoleReplace, "\\1")
            compare(initialized.zPosRole, "z")
            compare(initialized.zPosRolePattern, /-/)
            compare(initialized.zPosRoleReplace, "\\1")
        }
    }

    TestCase {
        name: "ItemModelScatterDataProxy Change"

        ListModel { id: model1; objectName: "model1" }

        function test_change() {
            change.itemModel = model1
            change.rotationRole = "rot"
            change.rotationRolePattern = /-/
            change.rotationRoleReplace = "\\1"
            change.xPosRole = "x"
            change.xPosRolePattern = /^.*-(\d\d)$/
            change.xPosRoleReplace = "\\1"
            change.yPosRole = "y"
            change.yPosRolePattern = /^(\d\d\d\d).*$/
            change.yPosRoleReplace = "\\1"
            change.zPosRole = "z"
            change.zPosRolePattern = /-/
            change.zPosRoleReplace = "\\1"

            compare(change.itemModel.objectName, "model1")
            compare(change.rotationRole, "rot")
            compare(change.rotationRolePattern, /-/)
            compare(change.rotationRoleReplace, "\\1")
            compare(change.xPosRole, "x")
            compare(change.xPosRolePattern, /^.*-(\d\d)$/)
            compare(change.xPosRoleReplace, "\\1")
            compare(change.yPosRole, "y")
            compare(change.yPosRolePattern, /^(\d\d\d\d).*$/)
            compare(change.yPosRoleReplace, "\\1")
            compare(change.zPosRole, "z")
            compare(change.zPosRolePattern, /-/)
            compare(change.zPosRoleReplace, "\\1")
        }
    }
}
