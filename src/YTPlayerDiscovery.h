#ifndef YTPLAYERDISCOVERY_H
#define YTPLAYERDISCOVERY_H

#include <QDBusConnectionInterface>
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

private:
    void setModel(YTListModel *model) { _model = model; }
    YTListModel *model() const { return _model; }

    void handleServiceName(const QString &serviceName, QList<QVariant> &services);

    YTListModel *_model;
};

#endif // YTPLAYERDISCOVERY
