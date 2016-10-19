/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
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

#include <QtTest/QtTest>
#include <QtCore/QObject>

#include "../src/screen.h"
#include "../src/config.h"
#include "../src/configmonitor.h"
#include "../src/output.h"
#include "../src/mode.h"
#include "../src/getconfigoperation.h"
#include "../src/setconfigoperation.h"
#include "../src/backendmanager_p.h"

using namespace KScreen;


class TestModeListChange : public QObject
{
    Q_OBJECT

private:
    KScreen::ConfigPtr getConfig();
    KScreen::ModeList createModeList();
    bool compareModeList(KScreen::ModeList before, KScreen::ModeList &after);

    QSize s0 = QSize(1920, 1080);
    QSize s1 = QSize(1600, 1200);
    QSize s2 = QSize(1280, 1024);
    QSize s3 = QSize(800, 600);
    QSize snew = QSize(777, 888);
    QString idnew = QStringLiteral("666");

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void modeListChange();
    void configChange();
};

ConfigPtr TestModeListChange::getConfig()
{
    qputenv("KSCREEN_BACKEND_INPROCESS", "1");
    auto *op = new GetConfigOperation();
    if (!op->exec()) {
        qWarning("ConfigOperation error: %s", qPrintable(op->errorString()));
        BackendManager::instance()->shutdownBackend();
        return ConfigPtr();
    }

    BackendManager::instance()->shutdownBackend();

    return op->config();
}

KScreen::ModeList TestModeListChange::createModeList()
{

    KScreen::ModeList newmodes;
    {
        QString _id = QString::number(11);
        KScreen::ModePtr kscreenMode(new KScreen::Mode);
        kscreenMode->setId(_id);
        kscreenMode->setName(_id);
        kscreenMode->setSize(s0);
        kscreenMode->setRefreshRate(60);
        newmodes.insert(_id, kscreenMode);
    }
    {
        QString _id = QString::number(22);
        KScreen::ModePtr kscreenMode(new KScreen::Mode);
        kscreenMode->setId(_id);
        kscreenMode->setName(_id);
        kscreenMode->setSize(s1);
        kscreenMode->setRefreshRate(60);
        newmodes.insert(_id, kscreenMode);
    }
    {
        QString _id = QString::number(33);
        KScreen::ModePtr kscreenMode(new KScreen::Mode);
        kscreenMode->setId(_id);
        kscreenMode->setName(_id);
        kscreenMode->setSize(s2);
        kscreenMode->setRefreshRate(60);
        newmodes.insert(_id, kscreenMode);
    }
    return newmodes;
}


void TestModeListChange::initTestCase()
{
    qputenv("KSCREEN_LOGGING", "false");
    qputenv("KSCREEN_BACKEND", "Fake");
}

void TestModeListChange::cleanupTestCase()
{
    BackendManager::instance()->shutdownBackend();
}

void TestModeListChange::modeListChange()
{
    return;
    //json file for the fake backend
    qputenv("KSCREEN_BACKEND_ARGS", "TEST_DATA=" TEST_DATA "singleoutput.json");

    const ConfigPtr config = getConfig();
    QVERIFY(!config.isNull());

    auto output = config->outputs().first();
    QVERIFY(!output.isNull());
    auto modelist = output->modes();

    for (auto &mode : modelist) {
        //qDebug() << " Mode: " << mode->id() << mode->size();
    }
    auto mode = modelist.first();
    mode->setId(QStringLiteral("44"));
    mode->setSize(QSize(880, 440));
    for (auto &mode : output->modes()) {
        //qDebug() << "-Mode: " << mode->id() << mode->size();
    }
    output->setModes(modelist);

    QCOMPARE(output->modes().first()->id(), QStringLiteral("44"));
    QCOMPARE(output->modes().first()->size(), QSize(880, 440));
    QVERIFY(!modelist.isEmpty());

    QSignalSpy configChangedSpy(ConfigMonitor::instance(), &ConfigMonitor::configurationChanged);
    ConfigMonitor::instance()->addConfig(config);
    QSignalSpy outputChangedSpy(output.data(), &Output::outputChanged);
    QVERIFY(outputChangedSpy.isValid());
    QSignalSpy modesChangedSpy(output.data(), &Output::modesChanged);
    QVERIFY(modesChangedSpy.isValid());

    auto before = createModeList();
    qDebug() << "--------";
    qDebug() << output->modes();
    qDebug() << before;
    output->setModes(before);
    QCOMPARE(modesChangedSpy.count(), 1);
    output->setModes(before);
    QCOMPARE(modesChangedSpy.count(), 1);
    output->setModes(before);
    QCOMPARE(modesChangedSpy.count(), 1);
    QCOMPARE(output->modes().first()->size(), s0);
    QCOMPARE(output->modes().first()->id(), QStringLiteral("11"));

    auto after = createModeList();

    //QVERIFY(compareModeList(before, after));
    auto firstmode = after.first();
    QVERIFY(!firstmode.isNull());

    QCOMPARE(firstmode->size(), s0);
    QCOMPARE(firstmode->id(), QStringLiteral("11"));
    firstmode->setSize(snew);
    firstmode->setId(idnew);
    output->setModes(after);
    QCOMPARE(modesChangedSpy.count(), 2);

    QString _id = QString::number(11);
    KScreen::ModePtr kscreenMode(new KScreen::Mode);
    kscreenMode->setId(_id);
    kscreenMode->setName(_id);
    kscreenMode->setSize(s0);
    kscreenMode->setRefreshRate(60);
    before.insert(_id, kscreenMode);
    output->setModes(before);
    QCOMPARE(modesChangedSpy.count(), 3);

    //QVERIFY(configChangedSpy.count() > 0);
}


void TestModeListChange::configChange()
{
    qputenv("KSCREEN_BACKEND_ARGS", "TEST_DATA=" TEST_DATA "singleoutput.json");
    auto activeConfig = getConfig();

    QSignalSpy configChangedSpy(ConfigMonitor::instance(), &ConfigMonitor::configurationChanged);
    ConfigMonitor::instance()->addConfig(activeConfig);

    auto changedConfig = getConfig();

    auto output = changedConfig->outputs().first();
    auto newmodes = createModeList();
    output->setModes(newmodes);

    // fixme: current mode id == -1?
    // ...

    output->setCurrentModeId(QStringLiteral("11"));

    auto setop = new KScreen::SetConfigOperation(changedConfig);
    QVERIFY(!setop->hasError());
    setop->exec();
    //activeConfig->apply(changedConfig);
    QVERIFY(!setop->hasError());
    qDebug() << "AC:" << activeConfig->outputs().first()->currentMode();

    // we're not really using a backend, so does this make sense at all?
//     QVERIFY(configChangedSpy.wait(200));
//     QVERIFY(configChangedSpy.count() > 0);

}

QTEST_MAIN(TestModeListChange)

#include "testmodelistchange.moc"
