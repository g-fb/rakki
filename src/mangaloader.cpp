/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mangaloader.h"

#include <QCollator>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>

#include "extractor.h"

MangaLoader::MangaLoader()
    : QObject()
{
    m_extractor = new Extractor(this);
    connect(m_extractor, &Extractor::started, this, [=, this]() {
        setExtractionProgress(0);
    });
    connect(m_extractor, &Extractor::progress, this, [=, this](int p) {
        setExtractionProgress(p);
    });
    connect(m_extractor, &Extractor::finished, this, [=, this]() {
        setExtractionProgress(0);
        handlePath(m_extractor->extractionFolder());
    });
    connect(m_extractor, &Extractor::finishedMemory, this, &MangaLoader::setupImages);
}

MangaLoader *MangaLoader::instance()
{
    static MangaLoader *l = new MangaLoader();
    return l;
}

void MangaLoader::setupImages(const QStringList &images, KArchive *archive)
{
    setExtractionProgress(0);
    m_images.clear();
    delete m_archive;
    m_archive = archive;

    std::unique_ptr<QIODevice> dev;
    QFileInfo fi;
    QImageReader imageReader;
    imageReader.setAutoTransform(true);
    for (int i = 0; i < images.count(); ++i) {
        fi.setFile(images.at(i));
        if (archive != nullptr) {
            const KArchiveFile *entry = archive->directory()->file(images.at(i));

            imageReader.setFormat(fi.suffix().toUtf8());
            if (!entry) {
                continue;
            }
            dev.reset(entry->createDevice());
        } else {
            std::unique_ptr<QFile> file(new QFile(images.at(i)));
            if (!file->open(QIODevice::ReadOnly)) {
                continue;
            }
            dev.reset(file.release());
        }

        if (dev.get() == nullptr) {
            continue;
        }

        imageReader.setDevice(dev.get());
        if (!imageReader.canRead()) {
            continue;
        }

        QSize pageSize = imageReader.size();
        if (imageReader.transformation() & QImageIOHandler::TransformationRotate90) {
            pageSize.transpose();
        }
        if (!pageSize.isValid()) {
            const QImage i = imageReader.read();
            if (!i.isNull()) {
                pageSize = i.size();
            }
        }
        if (pageSize.isValid()) {
            m_images.append({images.at(i), pageSize});
        }
    }
    Q_EMIT imagesReady(m_images);
}

void MangaLoader::handlePath(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(path);

    if (fileInfo.isDir()) {
        QStringList images = dirImages(fileInfo.absoluteFilePath(), true);
        setupImages(images);
    } else {
        m_extractor->open(fileInfo.absoluteFilePath());
        m_extractor->extractArchive();
    }
}

QStringList MangaLoader::dirImages(QString path, bool recursive)
{
    QStringList images;
    QDirIterator *it = nullptr;
    if (recursive) {
        it = new QDirIterator(path, QDir::Files, QDirIterator::Subdirectories);
    } else {
        it = new QDirIterator(path, QDir::Files, QDirIterator::NoIteratorFlags);
    }
    while (it->hasNext()) {
        QString file = it->next();
        QMimeType type = m_mimeDB.mimeTypeForFile(file);
        // only get images
        if (type.name().startsWith(QStringLiteral("image/"))) {
            images.append(file);
        }
    }
    delete it;

    // natural sort images
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(images.begin(), images.end(), collator);

    if (images.count() < 1) {
        return QStringList();
    }

    return images;
}

KArchive *MangaLoader::archive() const
{
    return m_archive;
}

int MangaLoader::extractionProgress()
{
    return m_extractionProgress;
}

void MangaLoader::setExtractionProgress(int progress)
{
    if (progress == extractionProgress()) {
        return;
    }
    m_extractionProgress = progress;
    Q_EMIT extractionProgressChanged();
}

#include "moc_mangaloader.cpp"
