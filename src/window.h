// SPDX-License-Identifier: BSD-3-Clause

#ifndef WINDOW_H
#define WINDOW_H

#include <QSystemTrayIcon>


#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
#ifndef QT_NO_SYSTEMTRAYICON
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
#endif
    void selectFile();

private:
    void createSimpleGroupBox();
    void createActions();
#ifndef QT_NO_SYSTEMTRAYICON
    void createTrayIcon();
#endif

    QGroupBox *simpleGroupBox;
    
    QLabel *simpleComboBoxLabel;
    QComboBox *simpleComboBox;

    QLabel *simpleSpinBoxLabel;
    QSpinBox *simpleSpinBox;


    QLabel *simpleLineEditLabel;
    QLineEdit *simpleLineEdit;


    QLabel *simpleTextEditLabel;
    QTextEdit *simpleTextEdit;


    QPushButton *simplePushButton;
    QPushButton *regexPushButton;

    QCheckBox *simpleCheckBox;

    QLabel* fileSelectLabel;
    QLineEdit* fileSelectTextEdit;
    QPushButton* fileSelectButton;


    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

#ifndef QT_NO_SYSTEMTRAYICON
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
#endif // #ifndef QT_NO_SYSTEMTRAYICON

    QString logfilepath;
};

#endif // #ifndef WINDOW_H
