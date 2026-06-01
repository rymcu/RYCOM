#include "lanstatushttpserver.h"

#include "systemstatusprovider.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QTcpSocket>

LanStatusHttpServer::LanStatusHttpServer(SystemStatusProvider *provider, QObject *parent)
    : QObject(parent),
      m_provider(provider)
{
    connect(&m_server, &QTcpServer::newConnection, this, &LanStatusHttpServer::handleNewConnection);
}

bool LanStatusHttpServer::start(quint16 port)
{
    if (m_server.isListening()) {
        return true;
    }

    m_port = port;
    m_errorString.clear();
    if (!m_server.listen(QHostAddress::Any, m_port)) {
        m_errorString = m_server.errorString();
        return false;
    }

    return true;
}

void LanStatusHttpServer::stop()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    m_server.close();
}

bool LanStatusHttpServer::isRunning() const
{
    return m_server.isListening();
}

quint16 LanStatusHttpServer::port() const
{
    return m_port;
}

QString LanStatusHttpServer::statusUrl() const
{
    return SystemStatusProvider::statusUrl(m_port);
}

QString LanStatusHttpServer::errorString() const
{
    return m_errorString;
}

void LanStatusHttpServer::handleNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket *socket = m_server.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            handleReadyRead(socket);
        });
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}

void LanStatusHttpServer::handleReadyRead(QTcpSocket *socket)
{
    const QByteArray request = socket->readAll();
    const QList<QByteArray> lines = request.split('\n');
    if (lines.isEmpty()) {
        socket->write(makeHttpResponse(400, "Bad Request", "Bad Request",
                                       "text/plain; charset=utf-8"));
        socket->disconnectFromHost();
        return;
    }

    const QList<QByteArray> requestParts = lines.first().trimmed().split(' ');
    if (requestParts.size() < 2 || requestParts.first() != "GET") {
        socket->write(makeHttpResponse(405, "Method Not Allowed", "Method Not Allowed",
                                       "text/plain; charset=utf-8"));
        socket->disconnectFromHost();
        return;
    }

    QByteArray path = requestParts.at(1);
    const int queryIndex = path.indexOf('?');
    if (queryIndex >= 0) {
        path = path.left(queryIndex);
    }

    if (path == "/" || path == "/status") {
        const QByteArray body = QJsonDocument(m_provider->statusJson()).toJson(QJsonDocument::Compact);
        socket->write(makeHttpResponse(200, "OK", body, "application/json; charset=utf-8"));
    } else {
        socket->write(makeHttpResponse(404, "Not Found", "Not Found", "text/plain; charset=utf-8"));
    }

    socket->disconnectFromHost();
}

QByteArray LanStatusHttpServer::makeHttpResponse(int statusCode, const QByteArray &statusText,
                                                 const QByteArray &body,
                                                 const QByteArray &contentType) const
{
    QByteArray response;
    response += "HTTP/1.1 " + QByteArray::number(statusCode) + " " + statusText + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Cache-Control: no-store\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    return response;
}
