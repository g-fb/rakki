/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <QObject>

#include <KArchive>

class QTemporaryDir;
class KArchiveDirectory;
class Extractor : public QObject
{
    Q_OBJECT
public:
    explicit Extractor(QObject *parent = nullptr);
    ~Extractor();

    void extractArchive();
    void extractRarArchive();
    QString extractionFolder();
    QString unrarNotFoundMessage();

    const QString &archiveFile() const;
    void setArchiveFile(const QString &archiveFile);

Q_SIGNALS:
    void started();
    void finished();
    void finishedMemory(const QStringList &, KArchive *);
    void error(const QString &);
    void progress(int);
    void unrarNotFound();

private:
    void getImagesInArchive(const QString &prefix, const KArchiveDirectory *dir);
    QString m_archiveFile;
    QTemporaryDir *m_tmpFolder{};
    QStringList m_entries;
};

#endif // EXTRACTOR_H
