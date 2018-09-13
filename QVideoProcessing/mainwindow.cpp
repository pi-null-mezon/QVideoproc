#include "mainwindow.h"

//------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    QWidget *_cW = new QWidget(NULL);
    this->setCentralWidget(_cW);
    //--------------------------------------------------------------
    pt_mainLayout = new QVBoxLayout();
    _cW->setLayout(pt_mainLayout);
    //--------------------------------------------------------------
    pt_display = new QImageWidget();
    pt_mainLayout->addWidget(pt_display);
    //--------------------------------------------------------------
    createThreads();
    //--------------------------------------------------------------
    createActions();
    createMenus();
    //--------------------------------------------------------------
    resize(640,480);
    statusBar()->showMessage(tr("Context menu is available by right click"));
}

//------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{

}

//------------------------------------------------------------------------------------

void MainWindow::createActions()
{
    pt_openDeviceAct = new QAction(tr("Device"),this);
    pt_openDeviceAct->setStatusTip(tr("Try to open available video capture device"));
    connect(pt_openDeviceAct, SIGNAL(triggered()), this, SLOT(opendevice()));

    pt_openVideoAct = new QAction(tr("File"),this);
    pt_openVideoAct->setStatusTip(tr("Try to open available video file"));
    connect(pt_openVideoAct, SIGNAL(triggered()), this, SLOT(openvideofile()));

    pt_pauseAct = new QAction(tr("&Pause"), this);
    pt_pauseAct->setStatusTip(tr("Stop a processing session"));
    connect(pt_pauseAct, SIGNAL(triggered()), pt_videoCapture, SLOT(pause()));

    pt_resumeAct = new QAction(tr("&Resume"), this);
    pt_resumeAct->setStatusTip(tr("Resume a processing session"));
    connect(pt_resumeAct, SIGNAL(triggered()), pt_videoCapture, SLOT(resume()));

    pt_exitAct = new QAction(tr("E&xit"), this);
    pt_exitAct->setStatusTip(tr("Application exit"));
    connect(pt_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    pt_aboutAct = new QAction(tr("&About"), this);
    pt_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(pt_aboutAct, SIGNAL(triggered()), this, SLOT(show_about()));

    pt_helpAct = new QAction(tr("&Help"), this);
    pt_helpAct->setStatusTip(tr("Show the application's Help"));
    connect(pt_helpAct, SIGNAL(triggered()), this, SLOT(show_help()));

    pt_deviceResAct = new QAction(tr("&CamResolution"), this);
    pt_deviceResAct->setStatusTip(tr("Open a video device resolution dialog"));
    connect(pt_deviceResAct, SIGNAL(triggered()), this, SLOT(opendeviceresolutiondialog()));

    pt_deviceSetAct = new QAction(tr("&CamSettings"), this);
    pt_deviceSetAct->setStatusTip(tr("Open a video device settings dialog"));
    connect(pt_deviceSetAct, SIGNAL(triggered()), this, SLOT(opendevicesettingsdialog()));

    pt_DirectShowAct = new QAction(tr("&DS_dialog"), this);
    pt_DirectShowAct->setStatusTip(tr("Try to open a device-driver embedded settings dialog"));
    connect(pt_DirectShowAct, SIGNAL(triggered()), this, SLOT(callDirectShowSdialog()));
}

//------------------------------------------------------------------------------------

void MainWindow::createMenus()
{
    pt_fileMenu = this->menuBar()->addMenu(tr("&Source"));
    pt_fileMenu->addAction(pt_openVideoAct);
    pt_fileMenu->addAction(pt_openDeviceAct);
    pt_fileMenu->addSeparator();
    pt_fileMenu->addAction(pt_exitAct);   
    //--------------------------------------------------
    pt_settingsMenu = this->menuBar()->addMenu(tr("&Settings"));
    pt_settingsMenu->addAction(pt_deviceResAct);
    pt_settingsMenu->addAction(pt_deviceSetAct);
    pt_settingsMenu->addSeparator();
    pt_settingsMenu->addAction(pt_DirectShowAct);
    //--------------------------------------------------
    pt_helpMenu = menuBar()->addMenu(tr("&Help"));
    pt_helpMenu->addAction(pt_helpAct);
    pt_helpMenu->addAction(pt_aboutAct);
}

//------------------------------------------------------------------------------------

void MainWindow::createThreads()
{
    //-------------------Pointers for objects------------------------
    pt_improcThread = new QThread(this); // Make an own QThread for opencv interface
    pt_opencvProcessor = new QOpencvProcessor();
    std::string cascadeFilename = std::string("haarcascades/haarcascade_frontalface_alt.xml");
    #ifdef HAARCASCADES_PATH
        //cascadeFilename = std::string(HAARCASCADES_PATH) + std::string("haarcascade_frontalface_alt.xml");
        cascadeFilename = std::string(LBPCASCADES_PATH) + std::string("lbpcascade_frontalface.xml");
    #endif
    bool loadResult = pt_opencvProcessor->loadClassifier( cascadeFilename );
    qWarning("Haarcascade %s was %s", cascadeFilename.c_str(), loadResult ? "loaded" : "not loadeded");
    pt_opencvProcessor->moveToThread( pt_improcThread );
    connect(pt_improcThread, SIGNAL(finished()), pt_opencvProcessor, SLOT(deleteLater()));

    //--------------------QVideoCapture------------------------------
    pt_videoThread = new QThread(this);
    pt_videoCapture = new QVideoCapture();
    pt_videoCapture->moveToThread(pt_videoThread);
    connect(pt_videoThread, SIGNAL(started()), pt_videoCapture, SLOT(initializeTimer()));
    connect(pt_videoThread, SIGNAL(finished()), pt_videoCapture, SLOT(close()));
    connect(pt_videoThread, SIGNAL(finished()), pt_videoCapture, SLOT(deleteLater()));

    //----------Register openCV types in Qt meta-type system---------
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<cv::Rect>("cv::Rect");

    //----------------------Connections------------------------------
    //connect(pt_videoCapture, SIGNAL(frame_was_captured(cv::Mat)), pt_opencvProcessor, SLOT(custom_algorithm(cv::Mat)),Qt::BlockingQueuedConnection);
    connect(pt_videoCapture, SIGNAL(frame_was_captured(cv::Mat)), pt_opencvProcessor, SLOT(searchFace(cv::Mat)),Qt::BlockingQueuedConnection);
    connect(pt_opencvProcessor, SIGNAL(frame_was_processed(cv::Mat,double)), pt_display, SLOT(updateImage(cv::Mat,double)), Qt::BlockingQueuedConnection);
    connect(pt_display, SIGNAL(selectionUpdated(cv::Rect)), pt_opencvProcessor, SLOT(setRect(cv::Rect)));

    //----------------------Thread start-----------------------------
    pt_improcThread->start();
    pt_videoThread->start();
}

//------------------------------------------------------------------------------------

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(pt_pauseAct);
    menu.addAction(pt_resumeAct);
    menu.exec(event->globalPos());
}

//------------------------------------------------------------------------------------

bool MainWindow::openvideofile()
{
    pt_pauseAct->trigger();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open video file"));
    while( !pt_videoCapture->openfile(fileName) )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open file!"), QMessageBox::Open | QMessageBox::Cancel, this, Qt::Dialog);
        if(msgBox.exec() == QMessageBox::Open)
        {
            fileName = QFileDialog::getOpenFileName(this, tr("Open video file"), "/video.avi", tr("Video (*.avi *.mp4, *.wmv)"));
        }
        else
        {
            return false;
        }
    }
    pt_resumeAct->trigger();
    return true;
}

//------------------------------------------------------------------------------------

bool MainWindow::opendevice()
{
    pt_pauseAct->trigger();
    pt_videoCapture->open_deviceSelectDialog();
    while( !pt_videoCapture->opendevice() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device!"), QMessageBox::Open | QMessageBox::Cancel, this, Qt::Dialog);
        if(msgBox.exec() == QMessageBox::Open)
        {
            pt_videoCapture->open_deviceSelectDialog();
        }
        else
        {
            return false;
        }
    }
    pt_resumeAct->trigger();
    return true;
}

//------------------------------------------------------------------------------------

void MainWindow::show_about()
{
   QDialog *aboutdialog = new QDialog(); 
   int pSize = aboutdialog->font().pointSize();
   aboutdialog->setWindowTitle("About");
   aboutdialog->setFixedSize(pSize*30,pSize*17);

   QVBoxLayout *templayout = new QVBoxLayout();

   QLabel *projectname = new QLabel(QString(APP_NAME) +"\t"+ QString(APP_VERS));
   projectname->setFrameStyle(QFrame::Box | QFrame::Raised);
   projectname->setAlignment(Qt::AlignCenter);
   QLabel *projectauthors = new QLabel("Designed by: Taranov Alex\n\nFirst release was in 2014");
   projectauthors->setWordWrap(true);
   projectauthors->setAlignment(Qt::AlignCenter);
   QLabel *hyperlink = new QLabel("<a href='mailto:pi-null-mezon@yandex.ru?subject=QVideoProcessing'>Contact us at pi-null-mezon@yandex.ru");
   hyperlink->setToolTip("Will try to open your default mail client");
   hyperlink->setOpenExternalLinks(true);
   hyperlink->setAlignment(Qt::AlignCenter);

   templayout->addWidget(projectname);
   templayout->addWidget(projectauthors);
   templayout->addWidget(hyperlink);

   aboutdialog->setLayout(templayout);
   aboutdialog->exec();

   delete hyperlink;
   delete projectauthors;
   delete projectname;
   delete templayout;
   delete aboutdialog;
}

//------------------------------------------------------------------------------------

void MainWindow::show_help()
{
    if (!QDesktopServices::openUrl(QUrl(QString("https://github.com/pi-null-mezon/QVideoProcessing.git"), QUrl::TolerantMode))) // runs the ShellExecute function on Windows
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not find help"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
}

//------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *)
{
    pt_improcThread->exit();
    pt_improcThread->wait();
    pt_videoThread->exit();
    pt_videoThread->wait();
}

//-----------------------------------------------------------------------------------

void MainWindow::opendeviceresolutiondialog()
{
    pt_pauseAct->trigger();
    if( !pt_videoCapture->open_resolutionDialog() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device resolution dialog!"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
    pt_resumeAct->trigger();
}

//-----------------------------------------------------------------------------------

void MainWindow::opendevicesettingsdialog()
{
    if( !pt_videoCapture->open_settingsDialog() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device settings dialog!"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
}

//-----------------------------------------------------------------------------------

void MainWindow::callDirectShowSdialog()
{   
    if (!QProcess::startDetached(QString("WVCF_utility.exe"),QStringList("-c")))// runs the ShellExecute function on Windows
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open utility, maybe you do not have it"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    } 
}

//----------------------------------------------------------------------------------------



