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
#include <QStringList>

#include <QtZeroProps/QZeroPropsService.h>

#include <common/Types.h>

class BleCentralAdapter;

class IoModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList inputNames READ inputNames NOTIFY iosChanged)
    Q_PROPERTY(QStringList outputNames READ outputNames NOTIFY iosChanged)
    Q_PROPERTY(int activeInput READ activeInput WRITE setActiveInput NOTIFY activeInputChanged)
    Q_PROPERTY(int activeOutput READ activeOutput WRITE setActiveOutput NOTIFY activeOutputChanged)
    Q_PROPERTY(bool multiChannelAvailable READ multiChannelAvailable NOTIFY activeOutputChanged)

public:
    static IoModel* instance();

    Q_INVOKABLE void setService(QtZeroProps::QZeroPropsService* service);

    QStringList inputNames() const;
    QStringList outputNames() const;

    int     activeInput() const;
    Q_INVOKABLE void setActiveInput(int i);

    int     activeOutput() const;
    Q_INVOKABLE void setActiveOutput(int i);

    common::IoInterface input();
    common::IoInterface output();

    bool multiChannelAvailable() const;

    Q_INVOKABLE void startDemo();

signals:
    void iosChanged();

    void activeInputChanged();
    void activeOutputChanged();

private:
    IoModel(QObject* parent = nullptr);

    static QString toString(common::IoInterface interface);

    void onPropertyChangedRemotely(const QVariant& uuid, const QByteArray& value);
    void onIoCapsReceived(const std::vector<common::IoInterface>& inputs, const std::vector<common::IoInterface>& ouputs);
    void onIoConfReceived(const common::IoInterface& input, const common::IoInterface& ouput);

    void sendIoConf();

    static IoModel* s_instance;

    QtZeroProps::QZeroPropsService* m_zpService = nullptr;

    std::vector<common::IoInterface> m_inputs;
    std::vector<common::IoInterface> m_outputs;

    int m_activeInput = 0;
    int m_activeOutput = 0;
};
