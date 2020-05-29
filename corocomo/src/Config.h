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

class Config : public QObject
{
    Q_OBJECT

    Q_PROPERTY(std::vector<double> freqTable MEMBER freqTable CONSTANT)
    Q_PROPERTY(double gainMin MEMBER gainMin CONSTANT)
    Q_PROPERTY(double gainMax MEMBER gainMax CONSTANT)
    Q_PROPERTY(double gainStep MEMBER gainStep CONSTANT)
    Q_PROPERTY(bool ioAvailable MEMBER ioAvailable CONSTANT)
    Q_PROPERTY(bool loudnessAvailable MEMBER loudnessAvailable CONSTANT)
    Q_PROPERTY(bool xoAvailable MEMBER xoAvailable CONSTANT)
    Q_PROPERTY(bool swAvailable MEMBER swAvailable CONSTANT)

public:
    enum class Type : uint8_t {
        Low,
        Mid,
        High
    };

    static Config* init(Type type);
    static Config* instance();

    uint8_t peqFilterCount = 1;

    std::vector<double> freqTable;
    uint8_t freqDefault;
    uint8_t freqMin;
    uint8_t freqMax;
    uint8_t freqStep;

    uint8_t qDefault;
    uint8_t qMin;
    uint8_t qMax;
    uint8_t qStep = 1;

    double gainMin = -24.0;
    double gainMax = 6.0;
    double gainStep = 1.0;

    bool ioAvailable = false;
    bool loudnessAvailable = false;
    bool xoAvailable = false;
    bool swAvailable = false;
    bool scAvailable = false; // SC = soft clip

private:
    explicit Config(Type type, QObject *parent = nullptr);
    static Config* s_instance;
};
// QVector<qreal> is Qt 5.9 compatible. Qt 5.11 also accepts std::vector<double>.
//Q_DECLARE_METATYPE(std::vector<double>)
