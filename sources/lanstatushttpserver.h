#ifndef LANSTATUSHTTPSERVER_H
#define LANSTATUSHTTPSERVER_H

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTcpServer>

class QTcpSocket;
class SystemStatusProvider;

class LanStatusHttpServer : public QObject
{
    Q_OBJECT

public:
    explicit LanStatusHttpServer(SystemStatusProvider *provider, QObject *parent = nullptr);

    bool start(quint16 port = 18765);
    void stop();
    bool isRunning() const;
    quint16 port() const;
    QString statusUrl() const;
    QString errorString() const;

private:
    void handleNewConnection();
    void handleReadyRead(QTcpSocket *socket);
    QByteArray makeHttpResponse(int statusCode, const QByteArray &statusText,
                                const QByteArray &body, const QByteArray &contentType) const;

    SystemStatusProvider *m_provider;
    QTcpServer m_server;
    quint16 m_port = 18765;
    QString m_errorString;
};

#endif // LANSTATUSHTTPSERVER_H
