/*************************************************************************************
 *  Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                             *
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

#include "waylandconfigreader.h"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "edid.h"

using namespace KScreen;

void WaylandConfigReader::outputsFromConfig(const QString& configfile, KWayland::Server::Display* display,
                                            QList< KWayland::Server::OutputDeviceInterface* >& outputs)
{
    QFile file(configfile);
    file.open(QIODevice::ReadOnly);

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = jsonDoc.object();

    QJsonArray omap = json["outputs"].toArray();
    Q_FOREACH(const QJsonValue &value, omap) {
        const QVariantMap &output = value.toObject().toVariantMap();
        if (output["connected"].toBool()) {
            outputs << createOutputDevice(output, display);
            //qDebug() << "new Output created: " << output["name"].toString();
        }
    }

}

OutputDeviceInterface* WaylandConfigReader::createOutputDevice(const QVariantMap& outputConfig, KWayland::Server::Display *display)
{
    KWayland::Server::OutputDeviceInterface *output = display->createOutputDevice(display);

    QByteArray data = QByteArray::fromBase64(outputConfig["edid"].toByteArray());
    Edid edid(data, display);

//     qDebug() << "EDID Info: ";
    if (edid.isValid()) {
//         qDebug() << "\tDevice ID: " << edid.deviceId();
//         qDebug() << "\tName: " << edid.name();
//         qDebug() << "\tVendor: " << edid.vendor();
//         qDebug() << "\tSerial: " << edid.serial();
//         qDebug() << "\tEISA ID: " << edid.eisaId();
//         qDebug() << "\tHash: " << edid.hash();
//         qDebug() << "\tWidth (mm): " << edid.width();
//         qDebug() << "\tHeight (mm): " << edid.height();
//         qDebug() << "\tGamma: " << edid.gamma();
//         qDebug() << "\tRed: " << edid.red();
//         qDebug() << "\tGreen: " << edid.green();
//         qDebug() << "\tBlue: " << edid.blue();
//         qDebug() << "\tWhite: " << edid.white();
        output->setPhysicalSize(QSize(edid.width() * 10, edid.height() * 10));
        output->setManufacturer(edid.vendor());
        output->setModel(edid.name());
    } else {
        output->setPhysicalSize(sizeFromJson(outputConfig["sizeMM"]));
        output->setManufacturer(outputConfig["manufacturer"].toString());
        output->setModel(outputConfig["model"].toString());
    }
    int currentModeId = outputConfig["currentModeId"].toInt();
    QVariantList preferredModes = outputConfig["preferredModes"].toList();

    Q_FOREACH(const QVariant &_mode, outputConfig["modes"].toList()) {
        const QVariantMap &mode = _mode.toMap();
        const QSize _size = sizeFromJson(mode["size"]);
        int refresh = 60000;

        if (mode.keys().contains("refreshRate")) {
            refresh = qRound(mode["refreshRate"].toReal());
        }
        bool isCurrent = currentModeId == mode["id"].toInt();
        bool isPreferred = preferredModes.contains(mode["id"]);

        //qDebug() << "Mode: " << _size << isCurrent << isPreferred;
        OutputDeviceInterface::ModeFlags flags;
        if (isPreferred) {
            flags &= OutputDeviceInterface::ModeFlags(OutputDeviceInterface::ModeFlag::Preferred);
        }
        if (isCurrent) {
            flags &= OutputDeviceInterface::ModeFlags(OutputDeviceInterface::ModeFlag::Preferred);
        }

        output->addMode(_size, flags, refresh);

        if (isCurrent) {
            output->setCurrentMode(_size, refresh);
        }
    }

    output->setGlobalPosition(pointFromJson(outputConfig["pos"]));
    output->setEnabled(outputConfig["enabled"].toBool());
    output->create();

    return output;
}

QSize WaylandConfigReader::sizeFromJson(const QVariant& data)
{
    QVariantMap map = data.toMap();

    QSize size;
    size.setWidth(map["width"].toInt());
    size.setHeight(map["height"].toInt());

    return size;
}

QPoint WaylandConfigReader::pointFromJson(const QVariant& data)
{
    QVariantMap map = data.toMap();

    QPoint point;
    point.setX(map["x"].toInt());
    point.setY(map["y"].toInt());

    return point;
}

QRect WaylandConfigReader::rectFromJson(const QVariant& data)
{
    QRect rect;
    rect.setSize(WaylandConfigReader::sizeFromJson(data));
    rect.setBottomLeft(WaylandConfigReader::pointFromJson(data));

    return rect;
}

