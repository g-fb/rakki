#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QQmlEngine>

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE QString parentFolder(QString path);
};

#endif // BACKEND_H
