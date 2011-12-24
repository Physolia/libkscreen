/*************************************************************************************
 *  Copyright (C) 2011 by Alex Fiestas <afiestas@kde.org>                            *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#ifndef QSCREEN_H
#define QSCREEN_H

#include "xlibandxrandr.h"

#include <QtCore/QObject>
#include <QSize>

namespace QRandR {

class Crtc;

class Screen : public QObject
{
    friend class Crtc;

    Q_OBJECT

    public:
        Screen (int screenId, Display* display);
        virtual ~Screen();

        const QSize minSize();
        const QSize maxSize();
        const QSize currentSize();

        QList<Crtc *> crtc();

    private:
        void getMinAndMaxSize();
        Window rootWindow();
        XRRScreenResources* resources();

    private:
        int m_id;
        Display *m_display;
        Window m_rootWindow;
        QSize m_minSize;
        QSize m_maxSize;
        QSize m_currentSize;

        XRRScreenResources *m_resources;
        QList<Crtc *> m_crtc;
};

#endif //QSCREEN_H

}