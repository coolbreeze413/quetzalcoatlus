// SPDX-License-Identifier: BSD-3-Clause

#include "window.h"
#include "logfileerrorsextractor.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QWindow>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QThread>

#include <iostream>
#include <regex>
#include <fstream>

Window::Window()
{
    createSimpleGroupBox();
    createActions();

    QIcon icon(":icons/logo.png");

#ifndef QT_NO_SYSTEMTRAYICON
    createTrayIcon();
    // QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Tray Icon Tooltip");
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
    trayIcon->show();
#endif // #ifndef QT_NO_SYSTEMTRAYICON

    QVBoxLayout *centraWidgetLayout = new QVBoxLayout;
    centraWidgetLayout->addWidget(simpleGroupBox);
    QWidget *widget = new QWidget();
    widget->setLayout(centraWidgetLayout);

    setCentralWidget(widget);

    setWindowTitle(tr("quetzalcoatlus"));
    setWindowIcon(icon);

    // aesthetic: setup the MainWindow to be two-third the area of the screen where it is displayed, centered.
    QDesktopWidget dw;
    // support multiple screens correctly.
    // show() required before windowHandle() is valid!
    show();
    // set the screen to be displayed according to the mouse cursor position right now
    windowHandle()->setScreen(QGuiApplication::screenAt(QCursor::pos()));
    // set the geometry so that we are approximately 2/3 of the screen size
    QRect desk_rect = dw.screenGeometry(dw.screenNumber(QCursor::pos()));
    setGeometry(desk_rect.width() / 6, desk_rect.height() / 6, desk_rect.width() * 2 / 3,
                desk_rect.height() * 2 / 3);
    // move ourself so that we are centered
    move(desk_rect.width() / 2 - width() / 2 + desk_rect.left(), desk_rect.height() / 2 - height() / 2 + desk_rect.top());

    runLogFileErrorsExtractorWork("/workspace/repos/test_work/quetzalcoatlus/testfiles/test1.log");
}

void Window::runLogFileErrorsExtractorWork(const QString& filePath)
{
    qDebug() << "ui thread" << QThread::currentThreadId();

    LogFileErrorsExtractor* worker = new LogFileErrorsExtractor(filePath);
    QThread* thread = new QThread;

    // move the worker object to the thread
    worker->moveToThread(thread);

    // start worker on thread start
    Q_ASSERT(connect(thread, &QThread::started, worker, &LogFileErrorsExtractor::doWork));

    // finish thread on worker finish
    Q_ASSERT(connect(worker, &LogFileErrorsExtractor::finished, thread, &QThread::quit));

    // clean dynamically allocated memory
    Q_ASSERT(connect(worker, &LogFileErrorsExtractor::finished, worker, &QObject::deleteLater));
    Q_ASSERT(connect(worker, &LogFileErrorsExtractor::finished, thread, &QThread::deleteLater));

    // start the thread
    thread->start();
}

void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void Window::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        if (windowState() == Qt::WindowMinimized)
        {
#ifndef QT_NO_SYSTEMTRAYICON
            if (simpleCheckBox->isChecked())
            {
                // make ourselves invisible (tray icon only.)
                hide();
            }
#endif // #ifndef QT_NO_SYSTEMTRAYICON
        }
        else if (windowState() == Qt::WindowNoState)
        {
        }
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    // close event originated outside application (system event?)
    // basically, did the user click the close button?
    if (event->spontaneous())
    {
        qDebug() << "closeEvent() -> spontaneous()";
    }
    else
    {
        qDebug() << "closeEvent() -> *NOT* spontaneous()";
    }
    QMainWindow::closeEvent(event);
}

#ifndef QT_NO_SYSTEMTRAYICON
void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        break;
    }
}

void Window::showMessage()
{
    QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    QIcon icon(":/images/quetzalcoatlus.png");
    trayIcon->showMessage("This is TITLE",
                          "This is BODY",
                          icon,
                          1 * 1000);
}

void Window::messageClicked()
{
    QMessageBox::information(nullptr,
                             tr("TITLE"),
                             tr("TEXT"));
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
#endif // #ifndef QT_NO_SYSTEMTRAYICON

void Window::createSimpleGroupBox()
{
    simpleGroupBox = new QGroupBox(tr("GroupBox"));

    simpleComboBoxLabel = new QLabel(tr("ComboBox"));
    simpleComboBox = new QComboBox;
#ifndef QT_NO_SYSTEMTRAYICON
    simpleComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    simpleComboBox->addItem(style()->standardIcon(
                                QStyle::SP_MessageBoxInformation),
                            tr("Information"),
                            QSystemTrayIcon::Information);
    simpleComboBox->addItem(style()->standardIcon(
                                QStyle::SP_MessageBoxWarning),
                            tr("Warning"),
                            QSystemTrayIcon::Warning);
    simpleComboBox->addItem(style()->standardIcon(
                                QStyle::SP_MessageBoxCritical),
                            tr("Critical"),
                            QSystemTrayIcon::Critical);
#else // #ifndef QT_NO_SYSTEMTRAYICON
    simpleComboBox->addItem(tr("One"), QString("One"));
    simpleComboBox->addItem(tr("Two"), QString("Two"));
    simpleComboBox->addItem(tr("Three"), QString("Three"));
    simpleComboBox->addItem(tr("Four"), QString("Four"));
#endif // #ifndef QT_NO_SYSTEMTRAYICON
    simpleComboBox->setCurrentIndex(1);

    simpleSpinBoxLabel = new QLabel(tr("SpinBox"));
    simpleSpinBox = new QSpinBox;
    simpleSpinBox->setRange(1, 120);
    simpleSpinBox->setSuffix(" s");
    simpleSpinBox->setValue(15);

    simpleLineEditLabel = new QLabel(tr("LineEdit:"));
    simpleLineEdit = new QLineEdit(tr("This is a Line Edit"));

    simpleTextEditLabel = new QLabel(tr("TextEdit"));
    simpleTextEdit = new QTextEdit();
    simpleTextEdit->setPlainText(tr("This is a TextEdit and can have\n"
                                    "two lines or\n"
                                    "three"));

    fileSelectLabel = new QLabel(tr("Select Log File"));
    fileSelectTextEdit = new QLineEdit();
    fileSelectTextEdit->setPlaceholderText("Enter Path or Browse to Log File");
    fileSelectButton = new QPushButton("Browse");    
    QObject::connect(fileSelectButton, &QPushButton::clicked, this, &Window::selectFile);

    simplePushButton = new QPushButton(tr("PushButton"));
    // [1] on button press -> change to "something"
    // fix the lambda invocation and go further.
    // [2] on button press -> change button text to next integer starting from 0
    // use signal-slot, lambda
    
    QObject::connect(simplePushButton,
                     &QPushButton::released,
                     this,
                     [this]() {
                         bool ok;
                         int val = simplePushButton->text().toInt(&ok);
                         if (ok) {
                             simplePushButton->setText(QString::number(++val));
                         } else {
                             // if previous simplePushButton text was not integer, we start with 0 then
                             simplePushButton->setText(QString::number(0));
                         }
                     }
                    );

    regexPushButton = new QPushButton(tr("regex"));
    // [3] on button press -> do a regex search for number of errors (integer) in
    // selected logfilepath content (if logfilepath has been populated)
    // use the first match only, should be able to match any of the 3 patterns in log file.
    // use signal-slot, lambda
    // use c++ std::regex
    // use c++ std:: file/stream operations to read in the file and parse it.

#ifndef QT_NO_SYSTEMTRAYICON
    simpleCheckBox = new QCheckBox(tr("Minimize To Tray?"));
    simpleCheckBox->setChecked(false);
#else  // #ifndef QT_NO_SYSTEMTRAYICON
    simpleCheckBox = new QCheckBox(tr("CheckBox"));
    simpleCheckBox->setChecked(true);
#endif // #ifndef QT_NO_SYSTEMTRAYICON

    QGridLayout *simpleGroupBoxLayout = new QGridLayout;
    simpleGroupBoxLayout->addWidget(simpleComboBoxLabel, 0, 0);
    simpleGroupBoxLayout->addWidget(simpleComboBox, 0, 1, 1, 2);
    simpleGroupBoxLayout->addWidget(simpleSpinBoxLabel, 1, 0);
    simpleGroupBoxLayout->addWidget(simpleSpinBox, 1, 1);
    simpleGroupBoxLayout->addWidget(simpleLineEditLabel, 2, 0);
    simpleGroupBoxLayout->addWidget(simpleLineEdit, 2, 1, 1, 4);
    simpleGroupBoxLayout->addWidget(simpleTextEditLabel, 3, 0);
    simpleGroupBoxLayout->addWidget(simpleTextEdit, 3, 1, 2, 4);
    simpleGroupBoxLayout->addWidget(fileSelectLabel, 7, 0, 1, 1);
    simpleGroupBoxLayout->addWidget(fileSelectTextEdit, 7, 1, 1, 3);
    simpleGroupBoxLayout->addWidget(fileSelectButton, 7, 4, 1, 1);
    simpleGroupBoxLayout->addWidget(simplePushButton, 8, 4, 1, 1);
    simpleGroupBoxLayout->addWidget(regexPushButton, 9, 4, 1, 1);
    simpleGroupBoxLayout->addWidget(simpleCheckBox, 10, 0);
    simpleGroupBoxLayout->setColumnStretch(3, 1);
    simpleGroupBoxLayout->setRowStretch(4, 1);
    simpleGroupBox->setLayout(simpleGroupBoxLayout);
}

void Window::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}


void Window::selectFile() {
    logfilepath = QFileDialog::getOpenFileName(this,
                                               "Select Log File",
                                               QDir::currentPath(),
                                               "Log Files (*.log);;Text Files (*.txt);;Other Files (*.*)");
    fileSelectTextEdit->setText(QDir(QDir::currentPath()).filePath(logfilepath));
}
