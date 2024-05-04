/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>
#include <QQuickStyle>

#include "mangaimageprovider.h"

int main(int argc, char *argv[])
{
    auto startTime = QDateTime::currentMSecsSinceEpoch();
    qSetMessagePattern(QStringLiteral("%{time [hh:mm:ss.zzz]}: %{message}"));

    QGuiApplication::setDesktopFileName(QStringLiteral("com.georgefb.rakki"));

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("georgefb"));
    app.setApplicationName(QStringLiteral("rakki"));
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("rakki")));

    QCommandLineParser clParser;
    clParser.process(app);
    QString file;
    if (clParser.positionalArguments().size() > 0) {
        file = clParser.positionalArguments().first();
    }

    QQmlApplicationEngine engine(&app);
    const QUrl url(QStringLiteral("qrc:/qt/qml/com/georgefb/rakki/qml/main.qml"));
    auto onObjectCreated = [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, onObjectCreated, Qt::QueuedConnection);

    engine.addImageProvider(QStringLiteral("manga"), new MangaImageProvider());

    engine.rootContext()->setContextProperty(QStringLiteral("startupTime"), startTime);
    engine.rootContext()->setContextProperty(QStringLiteral("ctxFile"), file);

    engine.load(url);

    qDebug() << "execution time:" << QDateTime::currentMSecsSinceEpoch() - startTime;

    return app.exec();
}
