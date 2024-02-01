/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MANGAIMAGEPROVIDER_H
#define MANGAIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>

class MangaImageProvider : public QQuickAsyncImageProvider
{
public:
    MangaImageProvider();
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};

class MangaResponse : public QQuickImageResponse
{
public:
    MangaResponse(const QString &id, const QSize &requestedSize);

    QQuickTextureFactory *textureFactory() const override;

    QImage m_image;
    void getPreview(const QString &id, const QSize &requestedSize);
};
#endif // MANGAIMAGEPROVIDER_H
