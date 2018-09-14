#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "qvideosource.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateImage(const QImage &_qimage);
    void saveImage();
    void on_deviceCB_currentIndexChanged(int index);
    void on_viewfindersettingsCB_currentIndexChanged(int index);
    void on_targetlocationB_clicked();
    void on_targetlocationLE_textChanged(const QString &arg1);
    void on_captureintervalD_valueChanged(int value);

    void on_captureTB_clicked(bool checked);

protected:
    void closeEvent(QCloseEvent *_event);

private:
    void saveSessionSettings();
    void loadSessionSettings();
    void commutate();
    void getDevicesList();
    void getViewfinderSettings();

    Ui::MainWindow *ui;
    QVideoSource qvideosource;
    QTimer capturetimer;
    QImage qimage;
};

#endif // MAINWINDOW_H
