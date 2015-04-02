/*-
 * Copyright (c) 2014 Peter Tworek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QQuickView>
#include <QSqlError>
#include <QtQml>
#include <QDebug>
#include <QFontDatabase>
#include <QSqlDatabase>
#include <qtconcurrentrun.h>
#include <sailfishapp.h>

// third party code
#include <notification.h>

#include "YTPlayer.h"
#include "YTListModel.h"
#include "YTNetworkManager.h"
#include "YTLocalVideoManager.h"
#include "YTLocalVideoListModel.h"
#include "YTVideoDownloadNotification.h"
#include "YTSuggestionEngine.h"
#include "YTVideoUrlFetcher.h"
#include "YTTranslations.h"
#include "YTLocalVideo.h"
#include "YTWatchedRecently.h"
#include "YTUtils.h"
#include "YTRequest.h"
#include "YTLogger.h"
#include "YTPrefs.h"
#include "YTPlayerDiscovery.h"

namespace {
const QString kApplicationDBFileName = "YTPlayer.sqlite";

void
InitApplicationDatabase()
{
    QSqlDatabase db;
    QString dbdir = QStandardPaths::writableLocation((QStandardPaths::DataLocation));
    if (!QDir(dbdir).exists())
        QDir().mkpath(dbdir);
    db = QSqlDatabase::addDatabase("QSQLITE");
    Q_ASSERT(db.isValid());
    db.setDatabaseName(dbdir + QDir::separator() + kApplicationDBFileName);
    qDebug() << "Application database path: " <<
                dbdir + QDir::separator() + kApplicationDBFileName;
}

} // namespace

QThread*
GetBackgroundTaskThread()
{
    static QThread* thread = NULL;
    if (thread == NULL) {
        thread = new QThread();
        thread->start();
        thread->setPriority(QThread::LowPriority);
    }
    return thread;
}

QNetworkDiskCache*
GetImageDiskCache()
{
    static QNetworkDiskCache* cache = NULL;
    if (cache == NULL) {
        cache = new QNetworkDiskCache();
        QString datadir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        datadir += "/ImageCache";
        cache->setCacheDirectory(datadir);
        int size = QSettings().value("Cache/ImageSize").toInt();
        cache->setMaximumCacheSize(size * 1024*1024);
        qDebug() << "QML/Image network disk cache location: " << datadir;
        qDebug() << "QML/Image network disk cache size: " << size << "MB";
    }
    return cache;
}

QNetworkDiskCache*
GetAPIResponseDiskCache()
{
    static QNetworkDiskCache* cache = NULL;
    if (cache == NULL) {
        cache = new QNetworkDiskCache();
        QString datadir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        datadir += "/APIRequestCache";
        cache->setCacheDirectory(datadir);
        int size = QSettings().value("Cache/YouTubeApiResponseSize").toInt();
        cache->setMaximumCacheSize(size * 1024*1024);
        qDebug() << "API request disk cache location: " << datadir;
        qDebug() << "API request disk cache size: " << size << "MB";
    }
    return cache;
}

class YTPNetworkAccessManagerFactory: public QQmlNetworkAccessManagerFactory
{
public:
    QNetworkAccessManager *create(QObject *parent)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager(parent);
        YTNetworkManager::instance().manageSessionFor(manager);
        manager->setCache(GetImageDiskCache());
        return manager;
    }
};

int
main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    YTTranslations translations;
    YTUtils utils;
    YTPrefs prefs;

    InitApplicationDatabase();

    YTWatchedRecently watched_recently;

    YTPrefs::initialize();

    // Make sure the logger is initialized
    YTLogger::instance();

    if (!YTTranslations::initialize()) {
        qCritical() << "Failed to initialize YTTranslations!";
        return -1;
    }

    if (QFontDatabase::addApplicationFont(":/fonts/youtube-icons.ttf") < 0) {
        qCritical() << "Failed to install youtube-icons font!";
        return -1;
    }

    qmlRegisterType<Notification>("harbour.ytplayer.notifications", 1, 0, "Notification");
    qmlRegisterType<YTRequest>("harbour.ytplayer", 1, 0, "YTRequest");
    qmlRegisterType<YTListModel>("harbour.ytplayer", 1, 0, "YTListModel");
    qmlRegisterType<YTListModelFilter>("harbour.ytplayer", 1, 0, "YTListModelFilter");
    qmlRegisterType<YTLocalVideo>("harbour.ytplayer", 1, 0, "YTLocalVideo");
    qmlRegisterType<YTLocalVideoListModel>("harbour.ytplayer", 1, 0, "YTLocalVideoListModel");
    qmlRegisterType<YTVideoDownloadNotification>("harbour.ytplayer", 1, 0, "YTVideoDownloadNotification");
    qmlRegisterType<YTSuggestionEngine>("harbour.ytplayer", 1, 0, "YTSuggestionEngine");
    qmlRegisterType<YTPlayerDiscovery>("harbour.ytplayer", 1, 0, "YTPlayerDiscovery");

    qmlRegisterUncreatableType<YTLogger>("harbour.ytplayer", 1, 0, "YTLogger",
                                         "Please use global Log instance");

    view->rootContext()->setContextProperty("Log", &YTLogger::instance());
    view->rootContext()->setContextProperty("YTUtils", &utils);
    view->rootContext()->setContextProperty("YTPrefs", &prefs);
    view->rootContext()->setContextProperty("YTNetworkManager", &YTNetworkManager::instance());
    view->rootContext()->setContextProperty("YTTranslations", &translations);
    view->rootContext()->setContextProperty("YTWatchedRecently", &watched_recently);

    view->engine()->addImportPath("qrc:/ui/qml/");
    view->setSource(QUrl("qrc:/ui/qml/YTPlayer.qml"));
    view->engine()->setNetworkAccessManagerFactory(new YTPNetworkAccessManagerFactory());

    view->showFullScreen();

    // Make sure old downloads are restored
    YTLocalVideoManager::instance();

    QtConcurrent::run(YTVideoUrlFetcher::runInitialCheck);

    int result = app->exec();

    qDebug() << "Application terminating";

    YTNetworkManager::instance().shutdown();
    GetBackgroundTaskThread()->exit();
    GetBackgroundTaskThread()->wait();

    return result;
}
