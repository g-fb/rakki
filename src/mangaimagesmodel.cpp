/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mangaimagesmodel.h"
#include "mangaloader.h"

#include <QImage>
#include <QList>
#include <QUrl>

MangaImagesModel::MangaImagesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(MangaLoader::instance(), &MangaLoader::imagesReady, this, [=, this](QList<Image> images) {
        beginResetModel();
        m_images.clear();
        endResetModel();

        beginInsertRows(QModelIndex(), 0, images.count() - 1);
        m_images = images;
        endInsertRows();
        Q_EMIT updated();
    });
}

int MangaImagesModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }

    return m_images.count();
}

QVariant MangaImagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    QString path = m_images[index.row()].path;
    int width = m_images[index.row()].size.width();
    int height = m_images[index.row()].size.height();

    switch (role) {
    case MangaImagesModel::PathRole:
        return QVariant(path);
    case MangaImagesModel::WidthRole:
        return QVariant(width);
    case MangaImagesModel::HeightRole:
        return QVariant(height);
    }

    return QVariant();
}

QHash<int, QByteArray> MangaImagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MangaImagesModel::PathRole] = "path";
    roles[MangaImagesModel::WidthRole] = "width";
    roles[MangaImagesModel::HeightRole] = "height";
    return roles;
}

QString MangaImagesModel::path() const
{
    return m_path;
}

void MangaImagesModel::setPath(const QString &path)
{
    if (m_path == path) {
        return;
    }
    auto url = QUrl::fromUserInput(path);
    m_path = url.toLocalFile();
    Q_EMIT pathChanged();
    update();
}

void MangaImagesModel::update()
{
    MangaLoader::instance()->handlePath(m_path);
}
#include "moc_mangaimagesmodel.cpp"
