// SPDX-License-Identifier: BSD-3-Clause

#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include <QColor>
#include "quetzalcoatlus_config.h"
#include "window.h"

#include "logfileerrorsextractor.h"

int main(int argc, char *argv[])
{
    // initialize the Qt resource system ('quetzalcoatlus.qrc')
    Q_INIT_RESOURCE(quetzalcoatlus);

    // https://doc.qt.io/qt-5/highdpi.html
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Qt5 only: Make QIcon::pixmap() generate high-dpi pixmaps that can be larger than the requested size.
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

#ifndef QT_NO_SYSTEMTRAYICON
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(nullptr, 
                             QObject::tr("quetzalcoatlus"),
                             QObject::tr("system tray not available on this system!"));
        return 1;
    }
#endif // #ifndef QT_NO_SYSTEMTRAYICON


#if QUETZALCOATLUS_USE_SPLASH_SCREEN
    QPixmap pixmap(":splash/splash.jpg");
    QSplashScreen splash(pixmap, Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    splash.show();
    splash.showMessage(QObject::tr("Thinking..."), Qt::AlignBottom | Qt::AlignRight, Qt::black);
    QTimer::singleShot(3000, &splash, &QWidget::close); // keep displayed for 5 seconds
#endif // #if QUETZALCOATLUS_USE_SPLASH_SCREEN

    Window window;
    window.show();
#if QUETZALCOATLUS_USE_SPLASH_SCREEN
    QTimer::singleShot(3000, &window, &QWidget::raise); // raise it to front after splash screen is done.
#endif // #if QUETZALCOATLUS_USE_SPLASH_SCREEN

    LogFileErrorsExtractor worker("/workspace/repos/test_work/quetzalcoatlus/testfiles/test1.log");
    worker.doWork();
    exit(1);

    return app.exec();
}
