/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mangaimageprovider.h"
#include "mangaloader.h"

#include <KArchive>
#include <QImageReader>

MangaImageProvider::MangaImageProvider()
{
}

QQuickImageResponse *MangaImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto response = new MangaResponse(QUrl::fromPercentEncoding(id.toUtf8()), requestedSize);
    return response;
}

MangaResponse::MangaResponse(const QString &id, const QSize &requestedSize)
{
    getPreview(id, requestedSize);
}

void MangaResponse::getPreview(const QString &id, const QSize &requestedSize)
{
    KArchive *archive = MangaLoader::instance()->archive();
    QImageReader imageReader;
    if (archive == nullptr) {
        imageReader.setFileName(id);
    } else {
        auto file = archive->directory()->file(id);
        if (file == nullptr) {
            return;
        }
        imageReader.setDevice(file->createDevice());
    }
    m_image = imageReader.read().scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    Q_EMIT finished();
}

QQuickTextureFactory *MangaResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(QImage(m_image));
}
