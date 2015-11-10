/*************************************************************************************
 *  Copyright 2014 Sebastian Kügler <sebas@kde.org>                                  *
 *                                                                                   *
 *  This library is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU Lesser General Public                       *
 *  License as published by the Free Software Foundation; either                     *
 *  version 2.1 of the License, or (at your option) any later version.               *
 *                                                                                   *
 *  This library is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 *  Lesser General Public License for more details.                                  *
 *                                                                                   *
 *  You should have received a copy of the GNU Lesser General Public                 *
 *  License along with this library; if not, write to the Free Software              *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA       *
 *************************************************************************************/

#include "waylandconfig.h"
#include "waylandscreen.h"
#include "waylandoutput.h"

#include <mode.h>


using namespace KScreen;

WaylandScreen::WaylandScreen(WaylandConfig *config)
    : QObject(config)
    , m_size(QSize())
    , m_outputCount(0)
{
}

WaylandScreen::~WaylandScreen()
{
}

ScreenPtr WaylandScreen::toKScreenScreen(KScreen::ConfigPtr &parent) const
{
    KScreen::ScreenPtr kscreenScreen(new KScreen::Screen);
    updateKScreenScreen(kscreenScreen);
    return kscreenScreen;
}

void WaylandScreen::setOutputs(const QList<WaylandOutput*> outputs)
{
    m_outputCount = outputs.count();
    QRect r;
    Q_FOREACH (auto o, outputs) {
        if (o->enabled()) {
            r |= QRect(o->output()->globalPosition(), o->output()->pixelSize());
        }
    }
    m_size = r.size();
}

void WaylandScreen::updateKScreenScreen(KScreen::ScreenPtr &screen) const
{
//     screen->setCurrentSize(_s);
//     screen->setId(1);
//     screen->setMaxSize(_s);
//     screen->setMinSize(_s);
//     screen->setCurrentSize(_s);
//     screen->setMaxActiveOutputsCount(QGuiApplication::screens().count());
    screen->setMinSize(QSize(0, 0));
    screen->setMaxSize(QSize(64000, 64000));
    screen->setCurrentSize(m_size);
    screen->setMaxActiveOutputsCount(m_outputCount); // FIXME
}
