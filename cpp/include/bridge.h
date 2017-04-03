#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>

// The SocketServer class will allow us to listen to and
// send messages to the backend server.
//
// This class is exposed to the QML environment so
// it can be used and tied into the GUI easily.

// Forward declares
class QUdpSocket;
class QTcpSocket;

class Bridge : public QObject
{
    // This Q_OBJECT macro is required by Qt, because we subclass the QObject class.
    Q_OBJECT
    Q_PROPERTY(QVariantList inports MEMBER m_inports)
    Q_PROPERTY(QVariantList outports MEMBER m_outports)
public:
    // Define a basic contructor for a QObject
    explicit Bridge( QObject *parent = nullptr );
signals:
    void tcpSocketConnected();
    void lostConnection();

public slots:
    void beginUDPBroadcast();
    void tcpSend(QByteArray);
    void udpRecvBackendIpAndConnect();
    void getPorts();

private slots:

private:
    QUdpSocket *m_udpSocket;
    QAbstractSocket *m_tcpSocket;
    QVariantList m_inports, m_outports;
};
