#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTcpSocket>
#include <QJsonDocument>
#include "bridge.h"

// Sets up the TCP and UDP sockets, connecting to every signal that is useful to us.
Bridge::Bridge(QObject *parent) : QObject(parent),
    m_udpSocket( new QUdpSocket( this ) ),
    m_tcpSocket( new QTcpSocket( this ) )
{

    connect(m_tcpSocket, &QTcpSocket::connected,this, [this] {
        qDebug() << "has connected";
    });

    // Connect to signals
    connect(m_udpSocket, &QUdpSocket::readyRead,this, &Bridge::udpRecvBackendIpAndConnect );
    connect(m_tcpSocket, &QTcpSocket::connected, this, &Bridge::tcpSocketConnected);
    //connect(m_tcpSocket, &QTcpSocket::readyRead, this, &Bridge::tcpRecv );
}

// Tell the listening backend device that we are hearing them!
void Bridge::beginUDPBroadcast() {
    // TODO: Replace this sample JSON with live effect data + params.
    /* TODO: See if we _want_ to just broadcast the data or make a TCP
     * connection after like Younes suggests to send data over reliably. This can be done
     * of course but before we start work on it we gotta make sure its what we want to do.
     */
    // QByteArray datagram = "{\"delay\":{\"delay\": 1,\"feedback\": 0.5}}";
    QByteArray message = "1";
    m_udpSocket->writeDatagram( message, QHostAddress::Broadcast, 10000);
    qDebug() << "Started UDP server";
}

//
void Bridge::tcpSend(QByteArray message)
{
    if ( m_tcpSocket->write(message) == -1 ) {
        qWarning( "There was an error sending the TCP message %s", qPrintable( message ) );
        qDebug() << "Connection with backend interrupted.";
        emit(lostConnection());
        return;
    }

    m_tcpSocket->flush();
}

void Bridge::getPorts() {
    tcpSend("{\"intent\":\"REQPORT\"}");
    auto data = m_tcpSocket->readAll();
    QJsonObject jsobj (QJsonDocument::fromBinaryData(data).object());
    m_inports = jsobj["inports"].toArray().toVariantList();
    m_outports = jsobj["outports"].toArray().toVariantList();
}

void Bridge::sendPorts() {
    QByteArray msg = "{\"intent\":\"UPDATEPORT\", \"in\":\"";
    msg += inport +"\", \"out\":\"";
    msg += outport+"\"}";
    tcpSend(msg);
}

void Bridge::udpRecvBackendIpAndConnect() {
    while(m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram networkDatagram = m_udpSocket->receiveDatagram(1024);
        QByteArray receivedData = networkDatagram.data();

        QHostAddress address = networkDatagram.senderAddress();
        int port = receivedData.toInt();        

        qDebug() << "Host:" << address << "Port:" << port;

        m_tcpSocket->connectToHost(address, static_cast<quint16>( port ) );
    }
}
