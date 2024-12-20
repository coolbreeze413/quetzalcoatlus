// SPDX-License-Identifier: BSD-3-Clause

#include <QtGlobal>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTimer>
#include <QColor>
#include "quetzalcoatlus_config.h"
#include "window.h"


// https://stackoverflow.com/questions/240353/convert-a-preprocessor-token-to-a-string
// https://gcc.gnu.org/onlinedocs/gcc-13.2.0/cpp/Stringizing.html
// if we have '#define foo abcd', then:
// step 1: TOSTRING(foo)  fully macro-expanded -> TOSTRING(abcd) -> STRINGIFY(abcd)
#define TOSTRING(x) STRINGIFY(x)
// step 2: STRINGIFY(abcd) -> replaced by "abcd" and not macro expanded because it is stringized with '#'
#define STRINGIFY(x) #x


// example usage for checking version for includes
// remember to #include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


int main(int argc, char *argv[])
{
    // initialize the Qt resource system ('quetzalcoatlus.qrc')
    Q_INIT_RESOURCE(quetzalcoatlus);


    // https://stackoverflow.com/questions/52256264/qt-version-incorrect
    qDebug() << "";
    qDebug() << "Application Version           :" << "quetzalcoatlus" << TOSTRING(BUILD_VERSION);
    qDebug() << "Built                         :" << TOSTRING(BUILD_DATE);
    qDebug() << "Git Repo URL                  :" << TOSTRING(BUILD_GIT_REPO_URL);
    qDebug() << "Git SHA1                      :" << TOSTRING(BUILD_GIT_HASH);
    qDebug() << "built with Qt Version (string):" << QT_VERSION_STR;
    qDebug() << "built with Qt Version (hex)   :"
             << (((QT_VERSION) >> 16) & 0xff)
             << (((QT_VERSION) >> 8) & 0xff)
             << ((QT_VERSION) & 0xff);
    qDebug() << "runtime Qt Version (string)   :" << qVersion();
    qDebug() << "";


#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3))
    // https://doc.qt.io/qt-5/highdpi.html
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // Qt5 only: Make QIcon::pixmap() generate high-dpi pixmaps that can be larger than the requested size.
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#else
    // the Qt::AA_EnableHighDpiScaling and Qt::AA_UseHighDpiPixmaps are always enabled in Qt6 and deprecated.
#endif

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
    QPixmap pixmap = QIcon(":/images/logo.svg").pixmap(QSize(800,800));
    QSplashScreen splash(pixmap, Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    splash.show();
    splash.windowHandle()->setScreen(QGuiApplication::screenAt(QCursor::pos()));
    splash.showMessage(QObject::tr("Thinking..."), Qt::AlignBottom | Qt::AlignRight, Qt::gray);
    QTimer::singleShot(2600, &splash, &QWidget::close); // keep displayed for ~3 seconds
#endif // #if QUETZALCOATLUS_USE_SPLASH_SCREEN

    Window window;

#if QUETZALCOATLUS_USE_SPLASH_SCREEN
    QTimer::singleShot(2650, &window, &QWidget::show); // show the main window after splash
    QTimer::singleShot(2650, &window, &Window::setPositionAndSize); // adjust size and position
#else
    window.show();
    window.setPositionAndSize();
#endif // #if QUETZALCOATLUS_USE_SPLASH_SCREEN
    return app.exec();
}
