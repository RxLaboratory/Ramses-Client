/****************************************************************************
**
** Copyright (C) 2018 Luca Beldi <v.ronin@yahoo.it>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "duqf-utils/qtransposeproxymodel.h"
#include <QtCore/qlist.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qsize.h>

#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))

QModelIndex QTransposeProxyModel::uncheckedMapToSource(const QModelIndex &proxyIndex) const
{
    if (!this->sourceModel() || !proxyIndex.isValid())
        return QModelIndex();
    return this->sourceModel()->index(proxyIndex.column(), proxyIndex.row(), proxyIndex.parent());
}

QModelIndex QTransposeProxyModel::uncheckedMapFromSource(const QModelIndex &sourceIndex) const
{
    if (!this->sourceModel() || !sourceIndex.isValid())
        return QModelIndex();
    return this->sourceModel()->index(sourceIndex.column(), sourceIndex.row(), sourceIndex.parent());
}

/*!
    \class QTransposeProxyModel
    \inmodule QtCore
    \brief This proxy transposes the source model.

    This model will make the rows of the source model become columns of the proxy model and vice-versa.

    If the model is a tree, the parents will be transposed as well. For example, if an index in the source model had parent `index(2,0)`, it will have parent `index(0,2)` in the proxy.
*/

/*!
    Constructs a new proxy model with the given \a parent.
*/
QTransposeProxyModel::QTransposeProxyModel(QObject* parent)
    : QAbstractProxyModel(parent)
{}

/*!
    \reimp
*/
int QTransposeProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!this->sourceModel())
        return 0;
    Q_ASSERT(checkIndex(parent));
    return this->sourceModel()->columnCount(mapToSource(parent));
}

/*!
    \reimp
*/
int QTransposeProxyModel::columnCount(const QModelIndex &parent) const
{
    if (!this->sourceModel())
        return 0;
    Q_ASSERT(checkIndex(parent));
    return this->sourceModel()->rowCount(mapToSource(parent));
}

/*!
    \reimp
*/
QVariant QTransposeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!this->sourceModel())
        return QVariant();
    return this->sourceModel()->headerData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, role);
}

/*!
    \reimp
*/
bool QTransposeProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->setHeaderData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, value, role);
}

/*!
    \reimp
*/
bool QTransposeProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    Q_ASSERT(checkIndex(index));
    if (!this->sourceModel() || !index.isValid())
        return false;
    return this->sourceModel()->setItemData(mapToSource(index), roles);
}

/*!
    \reimp
*/
QSize QTransposeProxyModel::span(const QModelIndex &index) const
{
    Q_ASSERT(checkIndex(index));
    if (!this->sourceModel() || !index.isValid())
        return QSize();
    return this->sourceModel()->span(mapToSource(index)).transposed();
}

/*!
    \reimp
*/
QMap<int, QVariant> QTransposeProxyModel::itemData(const QModelIndex &index) const
{
    if (!this->sourceModel())
        return QMap<int, QVariant>();
    Q_ASSERT(checkIndex(index));
    return this->sourceModel()->itemData(mapToSource(index));
}

/*!
    \reimp
*/
QModelIndex QTransposeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!this->sourceModel() || !sourceIndex.isValid())
        return QModelIndex();
    Q_ASSERT(this->sourceModel()->checkIndex(sourceIndex));
    return uncheckedMapFromSource(sourceIndex);
}

/*!
    \reimp
*/
QModelIndex QTransposeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_ASSERT(checkIndex(proxyIndex));
    if (!this->sourceModel() || !proxyIndex.isValid())
        return QModelIndex();
    return uncheckedMapToSource(proxyIndex);
}

/*!
    \reimp
*/
QModelIndex QTransposeProxyModel::parent(const QModelIndex &index) const
{
    Q_ASSERT(checkIndex(index, CheckIndexOption::DoNotUseParent));
    if (!this->sourceModel() || !index.isValid())
        return QModelIndex();
    return uncheckedMapFromSource(uncheckedMapToSource(index).parent());
}

/*!
    \reimp
*/
QModelIndex QTransposeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(checkIndex(parent));
    if (!this->sourceModel())
        return QModelIndex();
    return mapFromSource(this->sourceModel()->index(column, row, mapToSource(parent)));
}

/*!
    \reimp
*/
bool QTransposeProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(checkIndex(parent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->insertColumns(row, count, mapToSource(parent));
}

/*!
    \reimp
*/
bool QTransposeProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(checkIndex(parent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->removeColumns(row, count, mapToSource(parent));
}

/*!
    \reimp
*/
bool QTransposeProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_ASSERT(checkIndex(sourceParent));
    Q_ASSERT(checkIndex(destinationParent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->moveColumns(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
    \reimp
*/
bool QTransposeProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(checkIndex(parent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->insertRows(column, count, mapToSource(parent));
}

/*!
    \reimp
*/
bool QTransposeProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(checkIndex(parent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->removeRows(column, count, mapToSource(parent));
}

/*!
    \reimp
*/
bool QTransposeProxyModel::moveColumns(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_ASSERT(checkIndex(sourceParent));
    Q_ASSERT(checkIndex(destinationParent));
    if (!this->sourceModel())
        return false;
    return this->sourceModel()->moveRows(mapToSource(sourceParent), sourceRow, count, mapToSource(destinationParent), destinationChild);
}

/*!
    \reimp
    This method will perform no action. Use a QSortFilterProxyModel on top of this one if you require sorting.
*/
void QTransposeProxyModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    Q_UNUSED(order);
    return;
}

#endif
