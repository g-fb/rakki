/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <QMimeType>
#include <QObject>
#include <QTemporaryDir>

#include <KArchive>

class KArchiveDirectory;

class Extractor : public QObject
{
    Q_OBJECT
public:
    explicit Extractor(QObject *parent = nullptr);

    bool open(const QString &path);
    void extractArchive();
    void extractRarArchive();
    /*
     * Extracts the first image from an archive, before image is extracted
     * files are natural sorted and filtered to have only images
     */
    QImage extractFirstImage();
    /*
     * Extracts the first image from a rar archive, before image is extracted
     * files are natural sorted and filtered to have only images
     */
    QImage rarExtractFirstImage();
    /*
     * Takes all files from an archive and returns only supported images
     */
    QStringList filterImages(const QStringList &files);
    QString extractionFolder();
    QString unrarNotFoundMessage();

    bool isZip();
    bool isRar();
    bool isTar();
    bool is7Z();

Q_SIGNALS:
    void started();
    void finished();
    void finishedMemory(const QStringList &, KArchive *);
    void progress(int);
    void unrarNotFound();

private:
    void getImagesInArchive(const QString &prefix, const KArchiveDirectory *dir);
    QString m_archiveFile;
    std::unique_ptr<KArchive> m_archive;
    std::unique_ptr<QTemporaryDir> m_tmpFolder;
    QMimeType m_archiveMimeType;
    QStringList m_entries;
};

#endif // EXTRACTOR_H
