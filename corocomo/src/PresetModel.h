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

#include <common/Types.h>

namespace QtZeroProps {
class QZeroPropsService;
}

class PresetModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList presetNames READ presetNames NOTIFY presetsChanged)
    Q_PROPERTY(int activePreset READ activePreset WRITE setActivePreset NOTIFY activePresetChanged)

public:
    static PresetModel* instance();

    Q_INVOKABLE void setService(QtZeroProps::QZeroPropsService* service);

    QStringList presetNames() const;

    int     activePreset() const;
    Q_INVOKABLE void setActivePreset(int i);

    Q_INVOKABLE void startDemo();

signals:
    void presetsChanged();
    void activePresetChanged();

private:
    PresetModel(QObject* parent = nullptr);

    static QString toString(common::IoInterface interface);

    void onPresetReceived(uint8_t index, uint8_t total, uint8_t active, const QString& name);

    static PresetModel* s_instance;

    QtZeroProps::QZeroPropsService* m_zpService = nullptr;

    QStringList m_presetNames;
    int m_activePreset = 0;
};
