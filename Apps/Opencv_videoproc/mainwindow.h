#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//------------------------------------------------------------------------------------------------------

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QLayout>
#include <QLabel>
#include <QThread>
#include <QStatusBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

#include "qimagewidget.h"
#include "qopencvprocessor.h"
#include "qvideocapture.h"

//------------------------------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void closeEvent(QCloseEvent *event);

public slots:
    bool openvideofile(); // for video file open
    bool opendevice(); // for video device open
    void opendeviceresolutiondialog();
    void opendevicesettingsdialog();
    void callDirectShowSdialog();
    void show_about();
    void show_help();

private:
    void createActions();
    void createMenus();
    void createTimers();
    void createThreads();
    QImageWidget *pt_display;
    QVBoxLayout *pt_mainLayout;
    QAction *pt_openVideoAct;
    QAction *pt_openDeviceAct;
    QAction *pt_exitAct;
    QAction *pt_aboutAct;
    QAction *pt_helpAct;
    QAction *pt_pauseAct;
    QAction *pt_resumeAct;
    QAction *pt_deviceResAct;
    QAction *pt_deviceSetAct;
    QAction *pt_DirectShowAct;
    QMenu *pt_fileMenu;
    QMenu *pt_helpMenu;
    QMenu *pt_settingsMenu;
    QVideoCapture *pt_videoCapture;
    QOpencvProcessor *pt_opencvProcessor;
    QThread *pt_improcThread;
    QThread *pt_videoThread;
};
//------------------------------------------------------------------------------------------------------
#endif // MAINWINDOW_H
