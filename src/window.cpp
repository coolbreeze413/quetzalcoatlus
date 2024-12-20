// SPDX-License-Identifier: BSD-3-Clause

#include "window.h"

#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
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
#include <QMovie>
#include <QProgressDialog>
#include <QDebug>
#include <QSpacerItem>
#include <QStatusBar>
#include <QMenuBar>
#include <QDialogButtonBox>

#include <iostream>
#include <regex>
#include <fstream>


// https://stackoverflow.com/questions/240353/convert-a-preprocessor-token-to-a-string
// https://gcc.gnu.org/onlinedocs/gcc-13.2.0/cpp/Stringizing.html
// if we have '#define foo abcd', then:
// step 1: TOSTRING(foo)  fully macro-expanded -> TOSTRING(abcd) -> STRINGIFY(abcd)
#define TOSTRING(x) STRINGIFY(x)
// step 2: STRINGIFY(abcd) -> replaced by "abcd" and not macro expanded because it is stringized with '#'
#define STRINGIFY(x) #x


Window::Window()
{
    createSimpleGroupBox();
    createActions();
    createMenus();

    QIcon icon(":/images/logo_256x256.png");

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

    // finally adjust ourself to position centered and an appropriate size
    // this involves calling 'show()' and should be controlled from outside.
    // setPositionAndSize();

    statusBar()->showMessage(tr("I am hungry"));

}

void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}


void Window::setPositionAndSize()
{
    // aesthetic: setup the MainWindow to be two-third the area of the screen where it is displayed, centered.

    // support multiple screens correctly.
    // NOTE: show() required before windowHandle() is valid!
    show();

    // set the screen to be displayed according to the mouse cursor position right now
    windowHandle()->setScreen(QGuiApplication::screenAt(QCursor::pos()));

    // set the geometry so that we are approximately 2/3 of the screen size
    QRect desk_rect = QGuiApplication::screenAt(QCursor::pos())->geometry();
    setGeometry(desk_rect.width() / 6, desk_rect.height() / 6, desk_rect.width() * 2 / 3,
                desk_rect.height() * 2 / 3);
    
    // move ourself so that we are centered
    move(desk_rect.width() / 2 - width() / 2 + desk_rect.left(), desk_rect.height() / 2 - height() / 2 + desk_rect.top());
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
        // qDebug() << "closeEvent() -> spontaneous()";
    }
    else
    {
        // qDebug() << "closeEvent() -> *NOT* spontaneous()";
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
    QIcon icon(":/images/logo_256x256.png");
    trayIcon->showMessage("This is TITLE",
                          "This is BODY",
                          icon,
                          1 * 1000);
}


void Window::messageClicked()
{
    qDebug() << "Window::messageClicked()";
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
    simpleComboBox->setIconSize(QSize(120,100));
    simpleComboBox->setStyleSheet("QComboBox { background-color: gray; color: black; }");
    simpleComboBox->addItem(QIcon(":/images/sun.svg"), "Sun");
    simpleComboBox->addItem(QIcon(":/images/mercury.svg"), "Mercury");
    simpleComboBox->addItem(QIcon(":/images/venus.svg"), "Venus");
    simpleComboBox->addItem(QIcon(":/images/earth.svg"), "Earth");
    simpleComboBox->addItem(QIcon(":/images/mars.svg"), "Mars");
    simpleComboBox->addItem(QIcon(":/images/jupiter.svg"), "Jupiter");
    simpleComboBox->addItem(QIcon(":/images/saturn.svg"), "Saturn");
    simpleComboBox->addItem(QIcon(":/images/uranus.svg"), "Uranus");
    simpleComboBox->addItem(QIcon(":/images/neptune.svg"), "Neptune");
    simpleComboBox->setCurrentIndex(simpleComboBox->findText("Jupiter"));

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

    simplePixmapLabelLabel = new QLabel(tr("PixmapLabel"));
    simplePixmapLabel = new QLabel();
    // pixmap from a svg, via QIcon (or use QIcon directly):
    QPixmap p = QIcon(":/images/accretion_disk.svg").pixmap(QSize(93,80)); // use original image w,h or multiple.
    if(!p.isNull()) {
        simplePixmapLabel->setPixmap(p);
    }
    // QMovie* selectedGif = new QMovie(":/gif/accretion_disk.gif");
    // simplePixmapLabel->setMovie(selectedGif);
    // selectedGif->start();


    // https://stackoverflow.com/questions/31580362/qt-creating-icon-button
    simplePixmapPushButtonLabel = new QLabel(tr("PixmapPushButton"));
    simplePixmapPushButton = new QPushButton();
    simplePixmapPushButton->setToolTip("Click to view an accretion disk!");
    simplePixmapPushButton->setIcon(QIcon(":/images/accretion_disk.svg"));
    simplePixmapPushButton->setIconSize(QSize(93,80)); // use original image w,h or multiple.
    simplePixmapPushButton->setFixedSize(QSize(93+10,80+10));
    QObject::connect(simplePixmapPushButton, &QPushButton::released,
                     this,
                     [this]() {

                        // https://stackoverflow.com/questions/41079412/qt-show-gif-on-qdialog
                        QDialog* gifContainerDialog = new QDialog(this);
                        gifContainerDialog->setWindowModality(Qt::WindowModal);
                        gifContainerDialog->setWindowTitle("Accretion Disk");

                        QVBoxLayout* gifContainerDialogLayout = new QVBoxLayout();
                        gifContainerDialog->setLayout(gifContainerDialogLayout);

                        QLabel* gifContainerLabel = new QLabel();
                        QMovie* selectedGif = new QMovie(":/gif/accretion_disk.gif");
                        gifContainerLabel->setMovie(selectedGif);
                        gifContainerDialogLayout->addWidget(gifContainerLabel);
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3))
                        gifContainerDialogLayout->setMargin(0);
#else
                        gifContainerDialogLayout->setContentsMargins(0, 0, 0, 0);
#endif

                        // disallow resizing, but still adjust to content size.
                        // https://stackoverflow.com/questions/696209/non-resizeable-qdialog-with-fixed-size-in-qt
                        gifContainerDialog->layout()->setSizeConstraint( QLayout::SetFixedSize );
                        
                        selectedGif->start();
                        gifContainerDialog->show();
                     }
                    );

    fileSelectLabel = new QLabel(tr("Select Log File"));
    fileSelectTextEdit = new QLineEdit();
    fileSelectTextEdit->setPlaceholderText("Enter Path or Browse to Log File");
    fileSelectButton = new QPushButton("Browse");
    QObject::connect(fileSelectButton, &QPushButton::clicked, this, &Window::selectFile);

    simplePushButton = new QPushButton(tr("RightPushButton"));
    QObject::connect(simplePushButton, &QPushButton::released,
                     this,
                     [this]() {
                        if(this->simplePushButton->text() == "RightPushButton") {
                            this->simplePushButton->setText("0");
                        }
                        else {
                            QString numberstr = this->simplePushButton->text();
                            int number = numberstr.toInt();
                            number++;
                            numberstr = QString::number(number);
                            this->simplePushButton->setText(numberstr);
                        }
                     }
                    );

    regexPushButton = new QPushButton(tr("regex"));
    QObject::connect(regexPushButton, &QPushButton::released,
                     this,
                     [this]() {
                        ////// std:: implementation //////
                        ///// can also do Qt implementation 
                        std::regex regex;
                        std::smatch smatches;
                        bool found;
                        
                        if(!logfilepath.isEmpty()) {

                            std::string filecontent;

                            // get it into a ifstream and parse into std::string
                            std::ifstream stream(logfilepath.toStdString());
                            if (stream.good()) {

                                filecontent = 
                                std::string((std::istreambuf_iterator<char>(stream)),
                                            std::istreambuf_iterator<char>());
                            }
                            stream.close();


                            regex = std::regex("errors\\s*:\\s*(\\d+)", std::regex::ECMAScript);
                            found = std::regex_search ( filecontent, smatches, regex );
                            if(found) {
                                std::string string_value = smatches.str(1);
                                std::cout << "found: " << string_value << std::endl;
                            }

                        }
                     }
                    );

#ifndef QT_NO_SYSTEMTRAYICON
    simpleCheckBox = new QCheckBox(tr("Minimize To Tray?"));
    simpleCheckBox->setChecked(false);
#else  // #ifndef QT_NO_SYSTEMTRAYICON
    simpleCheckBox = new QCheckBox(tr("CheckBox"));
    simpleCheckBox->setChecked(true);
#endif // #ifndef QT_NO_SYSTEMTRAYICON


#if (QUETZALCOATLUS_USE_QGRIDLAYOUT == 1)

    QGridLayout *simpleGroupBoxLayout = new QGridLayout;
    // for simpler manipulation of Grid Layouts, use a bunch of variables
    // which makes it more human friendly to understand and modify:
    unsigned int row = 0;
    unsigned int column = 0;
    unsigned int rowspan = 0;
    unsigned int columnspan = 0;
    unsigned int total_columns = 0;
    unsigned int total_columns_max = 0;
    // unsigned int entity_max_rowspan = 0; // for more advanced usage, later.
    
    // simpleComboBoxLabel and simpleComboBox at same row
    // simpleComboBoxLabel = 1x1 and simpleComboBox = 1x2
    row = 0; 
    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simpleComboBoxLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 2;
    simpleGroupBoxLayout->addWidget(simpleComboBox, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simpleSpinBoxLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simpleSpinBox, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simpleLineEditLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 4;
    simpleGroupBoxLayout->addWidget(simpleLineEdit, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1; // set rowspan = 2 if the label should be centered.
    simpleGroupBoxLayout->addWidget(simpleTextEditLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 2; columnspan = 4;
    simpleGroupBoxLayout->addWidget(simpleTextEdit, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simplePixmapLabelLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simplePixmapLabel, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simplePixmapPushButtonLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simplePixmapPushButton, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(fileSelectLabel, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 3;
    simpleGroupBoxLayout->addWidget(fileSelectTextEdit, row, column, rowspan, columnspan);
    column = column + columnspan; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(fileSelectButton, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = total_columns_max - 1; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simplePushButton, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    column = total_columns_max - 1; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(regexPushButton, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;


    // create a gap between other content and last row:
    // so we add 'blank' labels in there
    column = 0; rowspan = 2; columnspan = 1;
    simpleGroupBoxLayout->addWidget(new QLabel(), row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;


    column = 0; rowspan = 1; columnspan = 1;
    simpleGroupBoxLayout->addWidget(simpleCheckBox, row, column, rowspan, columnspan);
    row += rowspan;
    total_columns = column + columnspan;
    if(total_columns > total_columns_max) total_columns_max = total_columns;

    // simpleGroupBoxLayout->setColumnStretch(3, 1);
    simpleGroupBox->setLayout(simpleGroupBoxLayout);

#else // #if (QUETZALCOATLUS_USE_QGRIDLAYOUT == 1) --> use hierarchical Box Layouts.

    QVBoxLayout *simpleGroupBoxLayout = new QVBoxLayout();

    QHBoxLayout* simpleComboBoxLayout = new QHBoxLayout();
    simpleComboBoxLabel->setMinimumWidth(200);
    simpleComboBoxLayout->addWidget(simpleComboBoxLabel, 1);
    simpleComboBoxLayout->addWidget(simpleComboBox, 2);
    //simpleComboBoxLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum));
    simpleComboBoxLayout->addStretch(17);
    simpleGroupBoxLayout->addLayout(simpleComboBoxLayout);

    QHBoxLayout* simpleSpinBoxLayout = new QHBoxLayout();
    simpleSpinBoxLabel->setMinimumWidth(200);
    simpleSpinBoxLayout->addWidget(simpleSpinBoxLabel, 1);
    simpleSpinBoxLayout->addWidget(simpleSpinBox, 2);
    simpleSpinBoxLayout->addStretch(17);
    simpleGroupBoxLayout->addLayout(simpleSpinBoxLayout);

    QHBoxLayout* simpleLineEditLayout = new QHBoxLayout();
    simpleLineEditLabel->setMinimumWidth(200);
    simpleLineEdit->setMinimumWidth(400);
    simpleLineEditLayout->addWidget(simpleLineEditLabel, 1);
    simpleLineEditLayout->addWidget(simpleLineEdit, 19);
    simpleGroupBoxLayout->addLayout(simpleLineEditLayout);

    QHBoxLayout* simpleTextEditLayout = new QHBoxLayout();
    simpleTextEditLabel->setMinimumWidth(200);
    simpleTextEdit->setMinimumWidth(400);
    simpleTextEditLayout->addWidget(simpleTextEditLabel, 1);
    simpleTextEditLayout->addWidget(simpleTextEdit, 19);
    simpleGroupBoxLayout->addLayout(simpleTextEditLayout);

    QHBoxLayout* simplePixmapLabelLayout = new QHBoxLayout();
    simplePixmapLabelLabel->setMinimumWidth(200);
    simplePixmapLabelLayout->addWidget(simplePixmapLabelLabel, 1);
    simplePixmapLabelLayout->addWidget(simplePixmapLabel, 2);
    simplePixmapLabelLayout->addStretch(17);
    simpleGroupBoxLayout->addLayout(simplePixmapLabelLayout);

    QHBoxLayout* simplePixmapPushButtonLayout = new QHBoxLayout();
    simplePixmapPushButtonLabel->setMinimumWidth(200);
    simplePixmapPushButtonLayout->addWidget(simplePixmapPushButtonLabel, 1);
    simplePixmapPushButtonLayout->addWidget(simplePixmapPushButton, 2);
    simplePixmapPushButtonLayout->addStretch(17);
    simpleGroupBoxLayout->addLayout(simplePixmapPushButtonLayout);

    QHBoxLayout* fileSelectLayout = new QHBoxLayout();
    fileSelectLabel->setMinimumWidth(200);
    fileSelectTextEdit->setMinimumWidth(400);
    fileSelectLayout->addWidget(fileSelectLabel, 1);
    fileSelectLayout->addWidget(fileSelectTextEdit, 18);
    fileSelectLayout->addWidget(fileSelectButton, 1);
    simpleGroupBoxLayout->addLayout(fileSelectLayout);

    QHBoxLayout* simplePushButtonLayout = new QHBoxLayout();
    simplePushButtonLayout->addStretch(19);
    simplePushButtonLayout->addWidget(simplePushButton, 1);
    simpleGroupBoxLayout->addLayout(simplePushButtonLayout);

    QHBoxLayout* regexPushButtonLayout = new QHBoxLayout();
    regexPushButtonLayout->addStretch(19);
    regexPushButtonLayout->addWidget(regexPushButton, 1);
    simpleGroupBoxLayout->addLayout(regexPushButtonLayout);


    // create a gap between other content and last row:
    //simpleGroupBoxLayout->addStretch(2);
    simpleGroupBoxLayout->addSpacerItem(new QSpacerItem(0,100,QSizePolicy::Minimum,QSizePolicy::MinimumExpanding));


    QHBoxLayout* simpleCheckBoxLayout = new QHBoxLayout();
    simpleCheckBoxLayout->addWidget(simpleCheckBox);
    simpleCheckBoxLayout->addStretch(19);
    simpleGroupBoxLayout->addLayout(simpleCheckBoxLayout);

    simpleGroupBox->setLayout(simpleGroupBoxLayout);

#endif // #if (QUETZALCOATLUS_USE_QGRIDLAYOUT == 1)

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

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show information about quetzalcoatlus"));
    connect(aboutAct, &QAction::triggered, this, &Window::about);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
}


void Window::createMenus() {
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}


void Window::selectFile() {
    logfilepath = QFileDialog::getOpenFileName(this,
                                               "Select Log File",
                                               QDir::currentPath(),
                                               "Log Files (*.log);;Text Files (*.txt);;All Files (*.*)",
                                               nullptr,
                                               QFileDialog::DontUseNativeDialog);
    fileSelectTextEdit->setText(QDir(QDir::currentPath()).filePath(logfilepath));
}


void Window::about() {

    QDialog* dialog = new QDialog(this);

    dialog->setWindowTitle("About quetzalcoatlus");

    QVBoxLayout* dialogLayout = new QVBoxLayout();
    dialog->setLayout(dialogLayout);

    QHBoxLayout* logoAndAppInfoHBoxLayout = new QHBoxLayout();
    QLabel* logoLabel = new QLabel();
    // pixmap from a svg, via QIcon (or use QIcon directly):
    QPixmap p = QIcon(":/images/logo.svg").pixmap(QSize(100,100)); // use original image w,h or multiple.
    if(!p.isNull()) {
        logoLabel->setPixmap(p);
    }
    logoAndAppInfoHBoxLayout->addWidget(logoLabel, 0, Qt::AlignCenter);

    QVBoxLayout* appInfoVBoxLayout = new QVBoxLayout();
    QLabel* versionLabel = new QLabel(QString(TOSTRING(BUILD_VERSION)));
    appInfoVBoxLayout->addWidget(versionLabel, 0, Qt::AlignLeft);

    QLabel* datetimeLabel = new QLabel(QString(TOSTRING(BUILD_DATE)).replace("_", " "));
    appInfoVBoxLayout->addWidget(datetimeLabel, 0, Qt::AlignLeft);

    QLabel* repoURLLabel = new QLabel();
    repoURLLabel->setText(QString("<a href=\"https://") + QString(TOSTRING(BUILD_GIT_REPO_URL)) + QString("\">coolbreeze413/quetzalcoatlus</a>"));
    repoURLLabel->setTextFormat(Qt::RichText);
    repoURLLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    repoURLLabel->setOpenExternalLinks(true);
    appInfoVBoxLayout->addWidget(repoURLLabel, 0, Qt::AlignLeft);

    QString gitHashString = QString(TOSTRING(BUILD_GIT_HASH));
    gitHashString.truncate(9);
    QLabel* gitHashLabel = new QLabel(gitHashString);
    appInfoVBoxLayout->addWidget(gitHashLabel, 0, Qt::AlignLeft);

    appInfoVBoxLayout->addStretch();
    logoAndAppInfoHBoxLayout->addLayout(appInfoVBoxLayout);

    dialogLayout->addLayout(logoAndAppInfoHBoxLayout);


    

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    dialogLayout->addWidget(buttonBox, 0, Qt::AlignRight);

    // disallow resizing, but still adjust to content size.
    // https://stackoverflow.com/questions/696209/non-resizeable-qdialog-with-fixed-size-in-qt
    dialogLayout->setSizeConstraint( QLayout::SetFixedSize );

    dialog->setModal(true);
    dialog->exec();

    dialog->deleteLater();
}
