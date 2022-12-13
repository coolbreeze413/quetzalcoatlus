// SPDX-License-Identifier: BSD-3-Clause

#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include <QColor>
#include "window.h"

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


    QPixmap pixmap(":splash/splash.jpg");
    QSplashScreen splash(pixmap, Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    splash.show();
    splash.showMessage(QObject::tr("Thinking..."), Qt::AlignBottom | Qt::AlignRight, Qt::black);
    QTimer::singleShot(3000, &splash, &QWidget::close); // keep displayed for 5 seconds

    Window window;
    window.show();
    QTimer::singleShot(3000, &window, &QWidget::raise); // raise it to front after splash screen is done.
    return app.exec();
}
