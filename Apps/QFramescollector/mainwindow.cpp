#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>
#include <QUuid>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1 v%2").arg(APP_NAME,APP_VERSION));
    loadSessionSettings();
    commutate();
    ui->captureTB->setDefaultAction(ui->actionCapture);
    getDevicesList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateImage(const QImage &_qimage)
{
    qimage = _qimage;
}

void MainWindow::saveImage()
{
    if(!qimage.isNull() && !ui->targetlocationLE->text().isEmpty()) {
        QString _filename = ui->targetlocationLE->text().append("/%1.jpg").arg(QUuid::createUuid().toString());
        qimage.save(_filename,"jpg",ui->qualityD->value());
        ui->lastframeW->updateImage(QImage(_filename));
        QDir _dir(ui->targetlocationLE->text());
        auto _files = _dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        ui->targetlocationfilesLE->setText(QString::number(_files.size()));
    }
}

void MainWindow::getDevicesList()
{
    QList<QCameraInfo> _devlist = QCameraInfo::availableCameras();
    for(int i = 0; i < _devlist.size(); ++i) {
        ui->deviceCB->addItem(_devlist.at(i).description());
    }
}

void MainWindow::getViewfinderSettings()
{
    ui->viewfindersettingsCB->clear();
    QList<QCameraViewfinderSettings> _settingslist = qvideosource.supportedViewfinderSettings();
    for(int i = 0; i < _settingslist.length(); i++) {
        QString description = QString("%1x%2 - %3 fps").arg(QString::number(_settingslist[i].resolution().width()),
                                                            QString::number(_settingslist[i].resolution().height()),
                                                            QString::number(_settingslist[i].minimumFrameRate(),'f',1));
        ui->viewfindersettingsCB->addItem(description);
    }
}

void MainWindow::on_deviceCB_currentIndexChanged(int index)
{
    QList<QCameraInfo> _devlist = QCameraInfo::availableCameras();
    qvideosource.close();
    qvideosource.open(_devlist.at(index));
    qvideosource.resume();
    getViewfinderSettings();
}

void MainWindow::on_viewfindersettingsCB_currentIndexChanged(int index)
{
    if(index > 0) {
        QList<QCameraViewfinderSettings> _settingslist = qvideosource.supportedViewfinderSettings();
        qvideosource.setViewfinderSettings(_settingslist.at(index));
    }
}

void MainWindow::commutate()
{
    connect(&qvideosource,SIGNAL(frameReady(QImage)),ui->videodisplayW,SLOT(updateImage(QImage)));
    connect(&qvideosource,SIGNAL(frameReady(QImage)),this,SLOT(updateImage(QImage)));
    connect(&capturetimer,SIGNAL(timeout()),this,SLOT(saveImage()));
}

void MainWindow::on_targetlocationB_clicked()
{
    QString _dirname = QFileDialog::getExistingDirectory(this,tr("Select target directory"),ui->targetlocationLE->text());
    if(!_dirname.isEmpty())
        ui->targetlocationLE->setText(_dirname);
}

void MainWindow::on_targetlocationLE_textChanged(const QString &arg1)
{
    QDir _dir(arg1);
    auto _files = _dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    ui->targetlocationfilesLE->setText(QString::number(_files.size()));
}

void MainWindow::on_captureintervalD_valueChanged(int value)
{
    ui->captureintervalLED->display(value);
    capturetimer.setInterval(value*1000);
}

void MainWindow::closeEvent(QCloseEvent *_event)
{
    saveSessionSettings();
    QMainWindow::closeEvent(_event);
}

void MainWindow::saveSessionSettings()
{
    QString _settingsdir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir _dir;
    _dir.mkpath(_settingsdir);

    QSettings _settings(_settingsdir.append("/%1.ini").arg(APP_NAME),QSettings::IniFormat);
    _settings.setValue("Targetlocation",ui->targetlocationLE->text());
    _settings.setValue("Captureinterval_s",ui->captureintervalD->value());
    _settings.setValue("Photoquality",ui->qualityD->value());
}

void MainWindow::loadSessionSettings()
{
    QString _settingsdir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir _dir;
    _dir.mkpath(_settingsdir);

    QSettings _settings(_settingsdir.append("/%1.ini").arg(APP_NAME),QSettings::IniFormat);
    ui->targetlocationLE->setText(_settings.value("Targetlocation","C:/Testdata").toString());

    ui->captureintervalD->setValue(_settings.value("Captureinterval_s",1).toInt());
    ui->captureintervalLED->display(ui->captureintervalD->value());
    capturetimer.setInterval(1000*ui->captureintervalD->value());

    ui->qualityD->setValue(_settings.value("Photoquality",95).toInt());
    ui->qualityLCD->display(ui->qualityD->value());
}

void MainWindow::on_actionCapture_triggered(bool checked)
{
    if(checked == true) {
        QPalette _palette = palette();
        _palette.setBrush(QPalette::Button,Qt::red);
         ui->captureTB->setPalette(_palette);
        capturetimer.start();
    } else {
        ui->captureTB->setPalette(palette());
        capturetimer.stop();
    }
}

void MainWindow::on_qualityD_valueChanged(int value)
{
    ui->qualityLCD->display(value);
}
