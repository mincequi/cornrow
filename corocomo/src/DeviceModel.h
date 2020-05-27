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

#include <QtZeroProps/QZeroPropsClient.h>

class DeviceModel : public QObject
{
	Q_OBJECT

    Q_PROPERTY(QtZeroProps::QZeroPropsClient::State status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusLabel READ statusLabel NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

    // @TODO(Qt): QObjectList is not accepted. So, we must use QList<QObject*>.
    Q_PROPERTY(QList<QObject*> services READ services NOTIFY servicesChanged)
    Q_PROPERTY(QObject* connectedService READ connectedService NOTIFY connectedServiceChanged)

public:
    static DeviceModel* init(QtZeroProps::QZeroPropsClient* netClient);
    static DeviceModel* instance();

    Q_INVOKABLE void startDiscovering();
	Q_INVOKABLE void startDemo();

    QtZeroProps::QZeroPropsClient::State status() const;
    QString     statusLabel() const;
    QString     statusText() const;

    QObjectList services() const;
    Q_INVOKABLE void connectToService(QtZeroProps::QZeroPropsService* device);

    QObject* connectedService() const;

signals:
    void statusChanged();
    void servicesChanged();
    void connectedServiceChanged();

private:
    explicit DeviceModel(QtZeroProps::QZeroPropsClient* m_zpClient, QObject* parent = nullptr);
    
    void stopDiscovering();

    void onAppStateChanged(Qt::ApplicationState state);
    void onDeviceStatus(QtZeroProps::QZeroPropsClient::State status, const QString& errorString = QString());
    void onDevicesChanged();

    static DeviceModel* s_instance;

    QtZeroProps::QZeroPropsClient::State m_status = QtZeroProps::QZeroPropsClient::State::Discovering;
    QString         m_statusLabel = "Discovering";
    QString         m_statusText;
    bool            m_demoMode = false;

    // Net
    QtZeroProps::QZeroPropsClient* m_zpClient = nullptr;
};
