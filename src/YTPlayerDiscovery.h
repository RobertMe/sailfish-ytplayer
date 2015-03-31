#ifndef YTPLAYERDISCOVERY_H
#define YTPLAYERDISCOVERY_H

#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QThread>

#include "YTListModel.h"

class YTPlayerDiscovery : public QThread
{
    Q_OBJECT

    Q_PROPERTY(YTListModel* model READ model WRITE setModel)

public:
    explicit YTPlayerDiscovery(QObject *parent = 0);

    void run() Q_DECL_OVERRIDE;
    Q_INVOKABLE void playIn(const QString &serviceName, const QString &videoId);

    Q_INVOKABLE void dispose();

private:
    void setModel(YTListModel *model) { _model = model; }
    YTListModel *model() const { return _model; }

    void handleServiceName(const QString &serviceName, QList<QVariant> &services);
    void appendServices(const QList<QVariant> &services);

    QDBusServiceWatcher _serviceWatcher;
    YTListModel *_model;

private slots:
    void onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner);
};

#endif // YTPLAYERDISCOVERY
