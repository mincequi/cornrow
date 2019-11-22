/*
 * Copyright (C) 2018 Manuel Weichselbaumer <mincequi@web.de>
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

#include "CoroPipeline.h"

#include <Peq.h>

using namespace coro::core;

CoroPipeline::CoroPipeline()
{
    m_peq = new coro::Peq();

    Node::link(m_appSource, m_sbcDecoder);
    Node::link(m_sbcDecoder, *m_peq);
    Node::link(*m_peq, m_alsaSink);
    /*
    Node::link(m_intToFloat, *m_peq);
    Node::link(*m_peq, m_intToFloat);
    Node::link(m_intToFloat, m_alsaSink);
    */
    m_alsaSink.start();
}

CoroPipeline::~CoroPipeline()
{
    m_alsaSink.stop();
    delete m_peq;
}

void CoroPipeline::pushBuffer(const coro::audio::AudioConf& conf, coro::audio::AudioBuffer& buffer)
{
    m_appSource.process(conf, buffer);
}
