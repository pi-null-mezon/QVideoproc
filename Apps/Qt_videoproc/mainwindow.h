#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qvideosource.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void setupActions();
    void createThreads();

private slots:
    void __selectVideoSource();

private:
    Ui::MainWindow *ui;

    QVideoSource *pt_qvideosource;
};

#endif // MAINWINDOW_H
