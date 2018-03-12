#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QThread>
#include <QContextMenuEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createThreads();
    setupActions();
}

void MainWindow::createThreads()
{
    qRegisterMetaType<cv::Mat>("cv::Mat");

    pt_qvideosource = new QVideoSource;
    connect(pt_qvideosource, SIGNAL(frameUpdated(cv::Mat,QImage::Format)), ui->displayWidget, SLOT(updateImage(cv::Mat,QImage::Format)));
    connect(ui->actionRotate, SIGNAL(triggered()), pt_qvideosource, SLOT(nextTransform()));
    connect(ui->actionResolution, SIGNAL(triggered()), pt_qvideosource, SLOT(setViewfinderSettings()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupActions()
{
    ui->actionSelect->setStatusTip(tr("Select video source"));
    connect(ui->actionSelect, SIGNAL(triggered()), this, SLOT(__selectVideoSource()));

    ui->pausevideoAct->setStatusTip(tr("Pause video"));
    connect(ui->pausevideoAct, SIGNAL(triggered(bool)), pt_qvideosource, SLOT(pause()));

    ui->resumevideoAct->setStatusTip(tr("Resume video"));
    connect(ui->resumevideoAct, SIGNAL(triggered(bool)), pt_qvideosource, SLOT(resume()));
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(ui->resumevideoAct);
    menu.addAction(ui->pausevideoAct);
    menu.exec(event->globalPos());
}

void MainWindow::__selectVideoSource()
{
    pt_qvideosource->selectDevice();
    pt_qvideosource->open();
}
