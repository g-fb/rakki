/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MANGAIMAGESMODEL_H
#define MANGAIMAGESMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QSize>
#include <QtQml/qqmlregistration.h>

struct Image {
    QString path;
    QSize size;
};

class MangaImagesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MangaImagesModel)

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit MangaImagesModel(QObject *parent = nullptr);

    enum Roles {
        PathRole = Qt::UserRole,
        WidthRole,
        HeightRole,
        ScaleRole,
        TypeRole,
    };
    Q_ENUM(Roles)

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int, QByteArray> roleNames() const override;

    QString path() const;
    void setPath(const QString &path);

    Q_INVOKABLE void update();

Q_SIGNALS:
    void updated();
    void pathChanged();

private:
    QList<Image> m_images;
    QMap<int, double> m_imgScales;
    QStringList m_acceptedMimeTypes;
    QString m_path;
};

#endif // MANGAIMAGESMODEL_H
