#include "YTPlayerDiscovery.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QSettings>
#include <QFile>

YTPlayerDiscovery::YTPlayerDiscovery(QObject *parent) :
    QThread(parent),
    _connection(QDBusConnection::sessionBus()),
    _serviceWatcher(QString(), _connection)
{
    moveToThread(this);
}

void
YTPlayerDiscovery::dispose()
{
    exit();
    delete this;
}

void
YTPlayerDiscovery::run()
{
    connect(&_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &YTPlayerDiscovery::onServiceOwnerChanged);
    QList<QVariant> services;
    QDBusReply<QStringList> registeredServiceNames = _connection.interface()->registeredServiceNames();
    foreach (QString serviceName, registeredServiceNames.value()) {
        handleServiceName(serviceName, services);
    }
    appendServices(services);

    exec();
}

void
YTPlayerDiscovery::playIn(const QString &serviceName, const QString &videoId)
{
    QDBusMessage message = QDBusMessage::createMethodCall(serviceName, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", "OpenUri");
    QList<QVariant> args;
    args.append(QString("youtube:%1").arg(videoId));
    message.setArguments(args);
    _connection.send(message);
}

void
YTPlayerDiscovery::handleServiceName(const QString &serviceName, QList<QVariant> &services)
{
    if (!serviceName.startsWith("org.mpris.MediaPlayer2."))
        return;

    MprisApplication *app = new MprisApplication(serviceName, "/org/mpris/MediaPlayer2", _connection, this);
    if (!app->supportedUriSchemes().contains("youtube")) {
        return;
    }

    QString name;
    QString iconPath;

    QString desktopEntry = app->desktopEntry();
    if (!desktopEntry.isEmpty()) {
        QSettings desktopFile(QString("/usr/share/applications/%1.desktop").arg(desktopEntry), QSettings::IniFormat);
        desktopFile.beginGroup("Desktop Entry");
        QString iconName = desktopFile.value("Icon").toString();
        name = desktopFile.value("Name").toString();
        if (!iconName.isEmpty()) {
            iconPath = QString("/usr/share/icons/hicolor/86x86/apps/%1.png").arg(iconName);
            if (!QFile(iconPath).exists()) {
                qDebug() << "non existent icon";
                iconPath = QString();
            }
        }
    }

    if (!app->identity().isEmpty()) {
        name = app->identity();
    }

    connect(app, &MprisApplication::identityChanged, this, &YTPlayerDiscovery::onIdentityChanged);

    _services.insert(serviceName, app);

    QVariantMap service;
    service.insert("serviceName", serviceName);
    service.insert("name", name);
    service.insert("icon", iconPath);
    services.append(service);
}

void
YTPlayerDiscovery::appendServices(const QList<QVariant> &services)
{
    if (services.size() > 0) {
        QMetaObject::invokeMethod(_model, "append", Qt::QueuedConnection, Q_ARG(QList<QVariant>, services));
    }
}

void
YTPlayerDiscovery::onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner)
{
    if (!oldOwner.isEmpty()) {
        MprisApplication *app = 0;
        if (service.startsWith("org.mpris.MediaPlayer2.") && (app = _services.take(service))) {
            QMetaObject::invokeMethod(_model, "remove", Qt::QueuedConnection, Q_ARG(QString, "serviceName"), Q_ARG(QVariant, QVariant(service)));

            app->deleteLater();
        }
    }

    if (!newOwner.isEmpty()) {
        QList<QVariant> services;
        handleServiceName(service, services);
        appendServices(services);
    }
}

void
YTPlayerDiscovery::onIdentityChanged()
{
    MprisApplication *app = qobject_cast<MprisApplication*>(sender());
    if (!app) {
        return;
    }

    QMetaObject::invokeMethod(_model, "update", Qt::QueuedConnection, Q_ARG(QString, "name"), Q_ARG(QVariant, QVariant(app->identity())), Q_ARG(QString, "serviceName"), Q_ARG(QVariant, QVariant(app->service())));
}
