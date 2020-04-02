/*
 * Copyright (C) 2020 Manuel Weichselbaumer <mincequi@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// @TODO(mawe): remove bluetooth dependency
#include <QBluetoothDeviceInfo>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

#include <ble/BleClient.h>

#include <QZeroProps/QZeroPropsTypes.h>

class QZeroConf;
class QZeroConfServiceData;
typedef QSharedPointer<QZeroConfServiceData> QZeroConfService;

namespace QZeroProps
{

class QZeroPropsClient : public QObject
{
    Q_OBJECT

public:
    enum State : uint8_t {
        Idle,
        Discovering,
        Connecting,
        Connected,
        Disconnected,
        Error
    };
    Q_ENUM(State)

    /*
    enum class Backend {
        None = 0x0,
        BluetoothLe = 0x01,
        WebSocket = 0x02,
        All = BluetoothLe | WebSocket
    };
    Q_DECLARE_FLAGS(Backends, Backend)
    Q_FLAG(Backends)
    */

    explicit QZeroPropsClient(QObject* parent = nullptr);
    ~QZeroPropsClient();

    //Backends supportedBackends() const;

    QList<QZeroPropsServicePtr> discoveredServices() const;

public slots:
    void startDiscovery(const Configuration& config);
    void stopDiscovery();

    void connectToService(QZeroPropsService* device);
    void disconnectFromService();

signals:
    void stateChanged(QZeroPropsClient::State state, const QString& errorString = QString());
    void deviceDiscovered(QZeroPropsServicePtr device);
    void deviceDisappeared(const QHostAddress& ip);

private:
    // Device related event handlers
    void onServiceDiscovered(QZeroConfService);
    void onServiceRemoved(QZeroConfService);
    void onStatus(QZeroPropsClient::State state, QString errorString = QString());

    QZeroPropsService* m_currentService = nullptr;

    QZeroConf* m_zeroConf = nullptr;
};

} // namespace QZeroProps


//Q_DECLARE_OPERATORS_FOR_FLAGS(QZeroProps::QZeroPropsClient::Backends)
