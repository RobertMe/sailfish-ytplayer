/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -c MprisApplication -p src/MprisApplication.h:src/MprisApplication.cpp dbus/org.mpris.MediaPlayer2.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "src/MprisApplication.h"

/*
 * Implementation of interface class MprisApplication
 */

MprisApplication::MprisApplication(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    connectPropertyChanged();
}

MprisApplication::~MprisApplication()
{
    disconnectPropertyChanged();
}

void
MprisApplication::connectPropertyChanged()
{
    QStringList matchArgs;
    matchArgs << interface();
    connection().connect(service(), path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", matchArgs, QString(), this, SLOT(onPropertiesChanged(QString,QMap<QString,QVariant>,QStringList)));
}

void
MprisApplication::disconnectPropertyChanged()
{
    QStringList matchArgs;
    matchArgs << interface();
    connection().disconnect(service(), path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", matchArgs, QString(), this, SLOT(onPropertiesChanged(QString,QMap<QString,QVariant>,QStringList)));
}

void
MprisApplication::onPropertiesChanged(const QString &interfaceName, const QMap<QString, QVariant> &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interfaceName)
    Q_UNUSED(invalidatedProperties)
    if (changedProperties.contains("Identity")) {
        emit identityChanged();
    }
}
