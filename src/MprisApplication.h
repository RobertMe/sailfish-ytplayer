/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -v -c MprisApplication -p src/MprisApplication.h:src/MprisApplication.cpp dbus/org.mpris.MediaPlayer2.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef MPRISAPPLICATION_H_1427309959
#define MPRISAPPLICATION_H_1427309959

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.mpris.MediaPlayer2
 */
class MprisApplication: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.mpris.MediaPlayer2"; }

public:
    MprisApplication(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~MprisApplication();

    Q_PROPERTY(bool CanQuit READ canQuit)
    inline bool canQuit() const
    { return qvariant_cast< bool >(property("CanQuit")); }

    Q_PROPERTY(bool CanRaise READ canRaise)
    inline bool canRaise() const
    { return qvariant_cast< bool >(property("CanRaise")); }

    Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen)
    inline bool canSetFullscreen() const
    { return qvariant_cast< bool >(property("CanSetFullscreen")); }

    Q_PROPERTY(QString DesktopEntry READ desktopEntry)
    inline QString desktopEntry() const
    { return qvariant_cast< QString >(property("DesktopEntry")); }

    Q_PROPERTY(bool Fullscreen READ fullscreen WRITE setFullscreen)
    inline bool fullscreen() const
    { return qvariant_cast< bool >(property("Fullscreen")); }
    inline void setFullscreen(bool value)
    { setProperty("Fullscreen", QVariant::fromValue(value)); }

    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    inline bool hasTrackList() const
    { return qvariant_cast< bool >(property("HasTrackList")); }

    Q_PROPERTY(QString Identity READ identity)
    inline QString identity() const
    { return qvariant_cast< QString >(property("Identity")); }

    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)
    inline QStringList supportedMimeTypes() const
    { return qvariant_cast< QStringList >(property("SupportedMimeTypes")); }

    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
    inline QStringList supportedUriSchemes() const
    { return qvariant_cast< QStringList >(property("SupportedUriSchemes")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Quit()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Quit"), argumentList);
    }

    inline QDBusPendingReply<> Raise()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Raise"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void identityChanged();

private:
    void connectPropertyChanged();
    void disconnectPropertyChanged();

private slots:
    void onPropertiesChanged(const QString &interfaceName, const QMap<QString, QVariant> &changedProperties, const QStringList &invalidatedProperties);
};

namespace org {
  namespace mpris {
    typedef ::MprisApplication MediaPlayer2;
  }
}
#endif