/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "extractor.h"

#include <QCollator>
#include <QFileInfo>
#include <QImage>
#include <QMimeDatabase>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <KArchive>
#include <KTar>
#include <KZip>
#ifdef WITH_K7ZIP
#include <K7Zip>
#endif

using namespace Qt::StringLiterals;

Extractor::Extractor(QObject *parent)
    : QObject{parent}
{
}

bool Extractor::open(const QString &path)
{
    QMimeDatabase db;
    m_archiveFile = path;
    m_archiveMimeType = db.mimeTypeForFile(path, QMimeDatabase::MatchContent);

    if (isZip()) {
        m_archive = std::make_unique<KZip>(path);
#ifdef WITH_K7ZIP
    } else if (is7Z()) {
        m_archive = std::make_unique<K7Zip>(path);
#endif
    } else if (isTar()) {
        m_archive = std::make_unique<KTar>(path);
    } else {
        return false;
    }

    if (!m_archive->open(QIODevice::ReadOnly)) {
        qDebug() << tr("Could not open archive: %1").arg(m_archiveFile) << "\n" << m_archive->errorString();
        return false;
    }

    return true;
}

void Extractor::extractArchive()
{
    if (m_archiveFile.isEmpty()) {
        qDebug() << tr("No archive file set");
        return;
    }
    qDebug() << "Extracting:" << m_archiveFile;
    // m_archive is passed to MangaLoader and deleted there
    // archive is passed to MangaLoader and deleted there
    if (m_archive == nullptr && isRar()) {
        return extractRarArchive();
    }
    if (m_archive == nullptr) {
        qDebug() << tr("Unknown archive: %1").arg(m_archiveFile);
        return;
    }

    const KArchiveDirectory *directory = m_archive->directory();
    if (!directory) {
        qDebug() << tr("Could not open archive: %1").arg(m_archiveFile);
        return;
    }

    getImagesInArchive(QString(), m_archive->directory());
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(m_entries.begin(), m_entries.end(), collator);

    Q_EMIT finishedMemory(m_entries, m_archive.release());
    m_entries.clear();
}

void Extractor::extractRarArchive()
{
    qDebug() << "Extracting:" << m_archiveFile;
    m_tmpFolder = std::make_unique<QTemporaryDir>();
    auto unrar = QStandardPaths::findExecutable(u"unrar"_s);
    if (unrar.isEmpty()) {
        return;
    }

    QStringList args;
    args << u"e"_s << m_archiveFile << m_tmpFolder->path() << u"-o+"_s;
    auto process = new QProcess();
    process->setProgram(unrar);
    process->setArguments(args);
    process->start();

    connect(process, &QProcess::started, this, &Extractor::started);
    connect(process, &QProcess::finished, this, [this, &process]() {
        Q_EMIT finished();
    });

    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        static QRegularExpression re(u"[0-9]+[%]"_s);
        QRegularExpressionMatch match = re.match(QString::fromUtf8(process->readAllStandardOutput()));
        if (match.hasMatch()) {
            QString matched = match.captured(0);
            Q_EMIT progress(matched.remove(u"%"_s).toInt());
        }
    });

    connect(process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError err) {
        QString errorMessage;
        switch (err) {
        case QProcess::FailedToStart:
            errorMessage = u"FailedToStart"_s;
            break;
        case QProcess::Crashed:
            errorMessage = u"Crashed"_s;
            break;
        case QProcess::Timedout:
            errorMessage = u"Timedout"_s;
            break;
        case QProcess::WriteError:
            errorMessage = u"WriteError"_s;
            break;
        case QProcess::ReadError:
            errorMessage = u"ReadError"_s;
            break;
        default:
            errorMessage = u"UnknownError"_s;
        }
        qDebug() << tr("Error: Could not open the archive. %1").arg(errorMessage);
    });

    return;
}

QImage Extractor::extractFirstImage()
{
    if (m_archiveFile.isEmpty()) {
        qDebug() << tr("No archive file set");
        return QImage();
    }
    qDebug() << "Extracting first image:" << m_archiveFile;
    // archive is passed to MangaLoader and deleted there
    if (m_archive == nullptr && isRar()) {
        return rarExtractFirstImage();
    }
    if (m_archive == nullptr) {
        qDebug() << tr("Unknown archive: %1").arg(m_archiveFile);
        return QImage();
    }

    const KArchiveDirectory *directory = m_archive->directory();
    if (!directory) {
        qDebug() << tr("Could not open archive: %1").arg(m_archiveFile);
        return QImage();
    }

    getImagesInArchive(QString(), m_archive->directory());
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(m_entries.begin(), m_entries.end(), collator);

    auto images = filterImages(m_entries);
    m_entries.clear();

    if (images.isEmpty()) {
        return QImage();
    }
    auto *file = directory->file(images.at(0));

    return QImage::fromData(file->data());
}

QImage Extractor::rarExtractFirstImage()
{
    m_tmpFolder = std::make_unique<QTemporaryDir>();
    auto unrar = QStandardPaths::findExecutable(u"unrar"_s);
    if (unrar.isEmpty()) {
        return QImage();
    }

    // get list od files in the archive
    QStringList args;
    args << u"lb"_s << m_archiveFile;
    QProcess process;
    process.setProgram(unrar);
    process.setArguments(args);
    process.start();
    process.waitForFinished();
    auto files = QString::fromLocal8Bit(process.readAllStandardOutput()).split(u"\n"_s);
    process.terminate();

    if (files.isEmpty()) {
        return QImage();
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(files.begin(), files.end(), collator);

    auto images = filterImages(files);

    if (images.isEmpty()) {
        return QImage();
    }

    args.clear();
    // extract file from archive
    args << u"x"_s << u"-n%1"_s.arg(images.first()) << m_archiveFile << m_tmpFolder->path();
    process.setProgram(unrar);
    process.setArguments(args);
    process.start();
    process.waitForFinished();

    return QImage(m_tmpFolder->path() + u"/"_s + images.first());
}

QStringList Extractor::filterImages(const QStringList &files)
{
    QStringList images;
    // clang-format off
    static const QStringList extensions{
        u".jpg"_s, u".jpeg"_s, u".png"_s, u".gif"_s,
        u".jxl"_s, u".webp"_s, u".heif"_s, u".avif"_s
    };
    for (const auto &file : files) {
        if (file.startsWith(u"__MACOSX"_s, Qt::CaseInsensitive) || file.startsWith(u".DS_Store"_s, Qt::CaseInsensitive)) {
            continue;
        }

        for (const auto &extension : extensions) {
            if (file.endsWith(extension)) {
                images.append(file);
            }
        }
    }
    // clang-format on
    return images;
}

void Extractor::getImagesInArchive(const QString &prefix, const KArchiveDirectory *dir)
{
    const QStringList entryList = dir->entries();
    for (const QString &file : entryList) {
        const KArchiveEntry *e = dir->entry(file);
        if (e->isDirectory()) {
            getImagesInArchive(prefix + file + u"/"_s, static_cast<const KArchiveDirectory *>(e));
        } else if (e->isFile()) {
            m_entries.append(prefix + file);
        }
    }
}

// clang-format off
bool Extractor::isZip()
{
    return m_archiveMimeType.inherits(u"application/x-cbz"_s)
        || m_archiveMimeType.inherits(u"application/zip"_s)
        || m_archiveMimeType.inherits(u"application/vnd.comicbook+zip"_s);
}

bool Extractor::isRar()
{
    return m_archiveMimeType.inherits(u"application/x-cbr"_s)
        || m_archiveMimeType.inherits(u"application/x-rar"_s)
        || m_archiveMimeType.inherits(u"application/vnd.rar"_s)
        || m_archiveMimeType.inherits(u"application/vnd.comicbook-rar"_s);
}

bool Extractor::isTar()
{
    return m_archiveMimeType.inherits(u"application/x-cbt"_s)
        || m_archiveMimeType.inherits(u"application/x-tar"_s);
}

bool Extractor::is7Z()
{
    return m_archiveMimeType.inherits(u"application/x-cb7"_s)
        || m_archiveMimeType.inherits(u"application/x-7z-compressed"_s);
}
// clang-format on

QString Extractor::extractionFolder()
{
    return m_tmpFolder->path();
}

QString Extractor::unrarNotFoundMessage()
{
#ifdef Q_OS_WIN32
    return u"UnRAR executable was not found.\n"
           "It can be installed through WinRAR or independent. "
           "When installed with WinRAR just restarting the application "
           "should be enough to find the executable.\n"
           "If installed independently you have to manually "
           "set the path to the UnRAR executable in the settings."_s;
#else
    return u"UnRAR executable was not found.\n"
           "Install the unrar package and restart the application, "
           "unrar should be picked up automatically.\n"
           "If unrar is still not found you can set "
           "the path to the unrar executable manually in the settings."_s;
#endif
}

#include "moc_extractor.cpp"
