/*-
 * Copyright (c) 2014 Peter Tworek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "YTListModel.h"

#include <QStringList>
#include <QDebug>

YTListModel::YTListModel(QObject *parent)
    : QAbstractListModel(parent)
    , _filter(new YTListModelFilter(this))
{
}

YTListModel::~YTListModel()
{
    clear();
    delete _filter;
}

int
YTListModel::rowCount(const QModelIndex&) const
{
    return _list.count();
}

QVariant
YTListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _list.size()) {
        return QVariant();
    }

    QByteArray roleName = _roles.value(role);
    if (roleName.isEmpty()) {
        qWarning() << "No item for role " << role << " found!";
        return QVariant();
    }

    QVariantMap map = _list.at(index.row()).toMap();
    Q_ASSERT(!map.isEmpty());
    return map.value(roleName);
}

QHash<int, QByteArray> YTListModel::roleNames() const
{
    Q_ASSERT(!_roles.isEmpty());
    return _roles;
}

void
YTListModel::append(QList<QVariant> list)
{
    if (list.isEmpty())
        return;

    if (_roles.isEmpty())
        initializeRoles(list);

    filter(list);

    beginInsertRows(QModelIndex(), _list.count(), _list.count() + list.count() - 1);
    _list.append(list);
    endInsertRows();

    emit countChanged(_list.size());
}

void
YTListModel::clear()
{
    if (_list.isEmpty())
        return;

    _roles.clear();

    beginRemoveRows(QModelIndex(), 0, _list.size() - 1);
    _list.clear();
    endRemoveRows();
    emit countChanged(0);
}

QVariant
YTListModel::get(int i) const
{
    return _list.at(i);
}

void
YTListModel::initializeRoles(QList<QVariant>& list)
{
    Q_ASSERT(_roles.isEmpty());
    QList<QString> keys = list.first().toMap().keys();
    int idx = Qt::UserRole + 1;
    foreach (QString key, keys) {
        _roles[idx++] = key.toLocal8Bit();
    }
}

int
YTListModel::find(const QString &property, const QVariant &value) const
{
    if (property.isEmpty()) {
        for (int i = 0; i < _list.size(); i++) {
            if (_list.at(i) == value) {
                return i;
            }
        }
    } else {
        for (int i = 0; i < _list.size(); i++) {
            if (_list.at(i).toMap()[property] == value) {
                return i;
            }
        }
    }
    return -1;
}

void
YTListModel::update(const QString &property, const QVariant &value, int row)
{
    if (row < 0 || row >= _list.size()) {
        return;
    }

    if (_list[row].type() == QMetaType::QVariantMap) {
        QVariantMap map = _list[row].toMap();
        map[property] = value;
        _list[row] = map;
        emit dataChanged(index(row, 0, QModelIndex()), index(row, 0, QModelIndex()));
    }
}

void
YTListModel::update(const QString &property, const QVariant &value, const QString &searchProperty, const QVariant &searchValue)
{
    int index = find(searchProperty, searchValue);
    if (index >= 0) {
        update(property, value, index);
    }
}

void
YTListModel::remove(int index)
{
    if (index < 0 || index >= _list.size()) {
        return;
    }

    emit beginRemoveRows(QModelIndex(), index, index);
    _list.removeAt(index);
    emit endRemoveRows();
}

void
YTListModel::remove(const QString &property, const QVariant &value)
{
    int index = find(property, value);
    if (index >= 0) {
        remove(index);
    }
}

void
YTListModel::filter(QList<QVariant>& list)
{
    if (_filter->key().isEmpty() || !_filter->value().isValid()) {
        qDebug() << "No filter specified";
        return;
    }

    QStringList tokens = _filter->key().split(".");
    QList<QVariant>::iterator it = list.begin();
    while (it != list.end()) {
        if (shouldFilterOut(*it, tokens)) {
            it = list.erase(it);
        } else {
            it++;
        }
    }
}

bool
YTListModel::shouldFilterOut(QVariant item, QStringList tokens)
{
    foreach (QString tok, tokens) {
        if (item.canConvert(QVariant::Map)) {
            QVariantMap map = item.toMap();
            item = map[tok];
            if (!item.isValid()) {
                return true;
            }
        } else {
            return true;
        }
    }
    return (item != _filter->value());
}
