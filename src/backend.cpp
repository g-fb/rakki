#include "backend.h"

#include <QFileInfo>

Backend::Backend(QObject *parent)
    : QObject{parent}
{
}

QString Backend::parentFolder(QString path)
{
    QUrl url = QUrl::fromUserInput(path);
    QFileInfo fi{url.toLocalFile()};
    url = QUrl::fromUserInput(fi.absolutePath());

    return url.toString();
}
