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

#include <QObject>

namespace QtZeroProps
{

/// QZeroPropsService provides access to the properties of a service instance.
/// The class permits modifying of the contained data and notifies about remote
/// changes.
///
/// Instances are created either from QZeroPropsClient or QZeroPropsServer and
/// behave slighty different dependent on their role (client or server).
class QZeroPropsService : public QObject
{
    Q_OBJECT

    /// This property holds the host/device name the service is running on.
    Q_PROPERTY(QString name READ name)
    /// This property holds the type of this service.
    Q_PROPERTY(ServiceType type READ type)

public:
    enum class ServiceType : uint8_t {
        Invalid = 0,
        BluetoothLe = 0x1,
        WebSocket = 0x2,
        All = BluetoothLe | WebSocket
    };
    Q_ENUM(ServiceType)

    /// Destroys this QZeroPropsService.
    virtual ~QZeroPropsService();

    /// Returns the host/device name the service is running on.
    QString name() const;

    /// Returns the type of this service.
    ServiceType type() const;

    /// This property holds the debounce time in milliseconds that is applied to
    /// any call to setProperty().
    ///
    /// If this object is created on server side, the default value is 0, which
    /// means a changed value will immediatly be sent. On client side this value
    /// defaults to 200 milliseconds.
    int debounceTime() const;
    void setDebounceTime(int msec);

    /// Set property values and emit to remote QZeroPropsService object. Calling
    /// this function does not trigger the a propertyChanged() signal unless the
    /// remote QZeroPropsService itself changes the value again afterwards.
    /// Values can be set and identified using QUuids and quint32.
    ///
    /// If we are using the BluetoothLe backend all properties set with quint32
    /// are extended to a QUuid using the base Uuid from this service.
    void setProperty(quint32 id, const QByteArray& value);
    void setProperty(const QUuid& uuid, const QByteArray& value);

signals:
    /// This signal is emitted when the value of a property is changed remotely.
    ///
    /// Each property is uniquely identified by the uuid parameter, which can
    /// carry a quint32 or QUuid, depending which method was used on remote side
    /// to change the property's value.
    /// The value parameter contains the updated value of the property.
    void propertyChanged(const QVariant& uuid, const QByteArray& value);

private:
    QZeroPropsService(QObject* parent = nullptr);

    // make non-const to later add custom impl
    class QZeroPropsServicePrivate* d;

    friend class QZeroPropsClient;
    friend class QZeroPropsClientPrivate;
    friend class QZeroPropsServer;
    friend class QZeroPropsServerPrivate;
};

} // namespace QZeroProps
