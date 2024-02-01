/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MANGALOADER_H
#define MANGALOADER_H

#include "mangaimagesmodel.h"

#include <QMimeDatabase>
#include <QObject>

class KArchive;
class QQmlEngine;
class QJSEngine;
class Extractor;

class MangaLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int extractionProgress MEMBER m_extractionProgress READ extractionProgress WRITE setExtractionProgress NOTIFY extractionProgressChanged)
public:
    int extractionProgress();
    void setExtractionProgress(int extractionProgressArg);

    static MangaLoader *instance();
    static MangaLoader *create(QQmlEngine *, QJSEngine *)
    {
        return instance();
    }

    KArchive *archive() const;

Q_SIGNALS:
    void extractionProgressChanged();
    void imagesReady(QList<Image> images);

public Q_SLOTS:
    void handlePath(const QString &path);

private:
    explicit MangaLoader();
    ~MangaLoader() = default;
    MangaLoader(const MangaLoader &) = delete;
    MangaLoader &operator=(const MangaLoader &) = delete;
    MangaLoader(MangaLoader &&) = delete;
    MangaLoader &operator=(MangaLoader &&) = delete;

    QStringList dirImages(QString path, bool recursive);
    void setupImages(const QStringList &images, KArchive *archive = nullptr);

    QString m_tmpFolder;
    QMimeDatabase m_mimeDB;
    Extractor *m_extractor{};
    int m_extractionProgress{0};
    KArchive *m_archive{};
    QList<Image> m_images;
};

#endif // MANGALOADER_H
