/*-----------------------------------------------------------------------------------------------------
Taranov Alex, 2014 									    SOURCE FILE
QVideoCapture class was created as Qt SIGNAL/SLOT wrapper for OpenCV VideoCapture class
Work with: call openfile(...) or opendevice(...) and on success, object will start to emit
frame_was_captured(const cv::Mat& value) signal with determined period of time.
To stop frame capturing use pause() or close(). Also
class provides some GUI interface to cv::VideoCapture::set(...) function.
------------------------------------------------------------------------------------------------------*/

#include "qvideocapture.h"

QVideoCapture::QVideoCapture(QObject *parent) :
    QObject(parent)
{
}

bool QVideoCapture::openfile(const QString &filename)
{
    pt_timer->stop();
    if( m_cvCapture.open( filename.toLocal8Bit().constData() ) )
    {
        deviceFlag = false;
        pt_timer->setInterval( 1000.0 / m_cvCapture.get(CV_CAP_PROP_FPS) ); // CV_CAP_PROP_FPS - m_frame rate

        return true;
    }
    return false;
}

bool QVideoCapture::opendevice(int period) // period should be entered in ms
{
    pt_timer->stop();
    if( m_cvCapture.open( device_id ) )
    {
        deviceFlag = true;
        pt_timer->setInterval( period );
        emit deviceOpened();
        return true;
    }
    return false;
}

bool QVideoCapture::set(int propertyID, double value)
{
    return m_cvCapture.set( propertyID, value);
}

bool QVideoCapture::resume()
{
    if( m_cvCapture.isOpened() )
    {
        pt_timer->start();
        return true;
    }
    return false;
}

bool QVideoCapture::start()
{
    if( m_cvCapture.isOpened() )
    {
        if(deviceFlag)
        {
            pt_timer->start();
        }
        else
        {
            //TO DO
        }
        return true;
    }
    return false;
}

bool QVideoCapture::close()
{
    if( m_cvCapture.isOpened() )
    {
        pt_timer->stop();
        m_cvCapture.release();
        return true;
    }
    return false;
}

bool QVideoCapture::pause()
{
    if( m_cvCapture.isOpened() )
    {
        pt_timer->stop();
        return true;
    }
    return false;
}

bool QVideoCapture::read_frame()
{
    if( ( m_cvCapture.read(m_frame) ) && ( !m_frame.empty() ) )
    {
        emit frame_was_captured(m_frame);
        return true;
    }
    else
    {
        this->pause();
        return false;
    }
}

bool QVideoCapture::open_resolutionDialog()
{
    if (m_cvCapture.isOpened() && deviceFlag)
    {
        //GUI CONSTRUCTION//
        QDialog dialog;
        int _psize = dialog.font().pointSize();
        dialog.setFixedSize(30*_psize, 15*_psize);
        dialog.setWindowTitle(tr("Resolution dialog"));

        QVBoxLayout toplevellayout;
        toplevellayout.setMargin(5);

        QHBoxLayout resLayout;
        resLayout.setMargin(5);
        QComboBox CBresolution;
        CBresolution.addItem("640 x 480"); // 0
        CBresolution.addItem("800 x 600"); // 1
        CBresolution.addItem("1024 x 768"); // 2
        CBresolution.addItem("1280 x 960"); // 3
        CBresolution.addItem("1600 x 1200"); // 4
        CBresolution.addItem("1980 x 1280"); // 5
        CBresolution.setSizeAdjustPolicy(QComboBox::AdjustToContents);
        CBresolution.setCurrentIndex(0);
        QLabel Lresolution("Resolution:");
        resLayout.addWidget(&Lresolution);
        resLayout.addWidget(&CBresolution);

        QHBoxLayout framerateLayout;
        framerateLayout.setMargin(5);
        QComboBox CBframerate;
        CBframerate.addItem("30 fps"); // 0
        CBframerate.addItem("25 fps"); // 1
        CBframerate.addItem("20 fps"); // 2
        CBframerate.addItem("15 fps"); // 3
        CBframerate.addItem("10 fps"); // 4
        CBframerate.addItem("5 fps"); // 5
        CBframerate.setSizeAdjustPolicy(QComboBox::AdjustToContents);
        CBframerate.setCurrentIndex(0);
        QLabel Lframerate("Framerate:");
        framerateLayout.addWidget(&Lframerate);
        framerateLayout.addWidget(&CBframerate);

        QHBoxLayout buttons;
        buttons.setMargin(5);
            QPushButton Baccept;
            Baccept.setText("Accept");
            connect(&Baccept, SIGNAL(clicked()), &dialog, SLOT(accept()));
            QPushButton Bcancel;
            Bcancel.setText("Cancel");
            connect(&Bcancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        buttons.addWidget(&Baccept);
        buttons.addWidget(&Bcancel);

        toplevellayout.addLayout(&resLayout);
        toplevellayout.addLayout(&framerateLayout);
        toplevellayout.addLayout(&buttons);
        dialog.setLayout(&toplevellayout);
        //GUI CONSTRUCTION END//

            if(dialog.exec() == QDialog::Accepted)
            {
                m_cvCapture.set(CV_CAP_PROP_FRAME_WIDTH, CBresolution.currentText().section(" x ",0,0).toDouble());
                m_cvCapture.set(CV_CAP_PROP_FRAME_HEIGHT, CBresolution.currentText().section(" x ",1,1).toDouble());
                pt_timer->setInterval(1000.0 / CBframerate.currentText().section(" ",0,0).toDouble());
                return true;
            }
    }
    return false; // false if device was not opened or dialog.exec() == QDialog::Rejected
}

bool QVideoCapture::open_settingsDialog()
{
    if (m_cvCapture.isOpened() && deviceFlag)
    {
    //DIALOG CONSTRUCTION//
    QDialog dialog;
    int _psize = dialog.font().pointSize();
    dialog.setFixedSize(30*_psize, 15*_psize);
    dialog.setWindowTitle(tr("Settings dialog"));

    QVBoxLayout toplevellayout;
    toplevellayout.setMargin(5);
    QTabWidget tabwidget;

    QWidget page1; // a widget for a cam general settings selection
        QHBoxLayout centralbox;
        centralbox.setMargin(5);
            QVBoxLayout sliders;
            sliders.setMargin(5);
            QVBoxLayout lineedits;
            lineedits.setMargin(5);
            QVBoxLayout names;
            names.setMargin(5);

                QSlider Sbrightness;
                Sbrightness.setOrientation(Qt::Horizontal);
                Sbrightness.setMinimum(MIN_BRIGHTNESS);
                Sbrightness.setMaximum(MAX_BRIGHTNESS);
                Sbrightness.setValue( (int)m_cvCapture.get(CV_CAP_PROP_BRIGHTNESS) );
                QLabel Lbrightness;
                Lbrightness.setNum( (int)m_cvCapture.get(CV_CAP_PROP_BRIGHTNESS) );
                connect(&Sbrightness, SIGNAL(valueChanged(int)), &Lbrightness, SLOT(setNum(int)));
                connect(&Sbrightness, SIGNAL(valueChanged(int)), this, SLOT(set_brightness(int)));
                connect(this, SIGNAL(set_default_brightness(int)), &Sbrightness, SLOT(setValue(int)));
                QLabel Nbrightness("Brightness:");

                QSlider Scontrast;
                Scontrast.setOrientation(Qt::Horizontal);
                Scontrast.setMinimum(MIN_CONTRAST);
                Scontrast.setMaximum(MAX_CONTRAST);
                Scontrast.setValue( (int)m_cvCapture.get(CV_CAP_PROP_CONTRAST) );
                QLabel Lcontrast;
                Lcontrast.setNum( (int)m_cvCapture.get(CV_CAP_PROP_CONTRAST) );
                connect(&Scontrast, SIGNAL(valueChanged(int)), &Lcontrast, SLOT(setNum(int)));
                connect(&Scontrast,SIGNAL(valueChanged(int)), this, SLOT(set_contrast(int)));
                connect(this, SIGNAL(set_default_contrast(int)), &Scontrast, SLOT(setValue(int)));
                QLabel Ncontrast("Contrast:");

                QSlider Ssaturation;
                Ssaturation.setOrientation(Qt::Horizontal);
                Ssaturation.setMinimum(MIN_SATURATION);
                Ssaturation.setMaximum(MAX_SATURATION);
                Ssaturation.setValue( (int)m_cvCapture.get(CV_CAP_PROP_SATURATION) );
                QLabel Lsaturation;
                Lsaturation.setNum( (int)m_cvCapture.get(CV_CAP_PROP_SATURATION) );
                connect(&Ssaturation, SIGNAL(valueChanged(int)), &Lsaturation, SLOT(setNum(int)));
                connect(&Ssaturation,SIGNAL(valueChanged(int)), this, SLOT(set_saturation(int)));
                connect(this, SIGNAL(set_default_saturation(int)), &Ssaturation, SLOT(setValue(int)));
                QLabel Nsaturation("Saturation:");

                QSlider SwhitebalanceU;
                SwhitebalanceU.setOrientation(Qt::Horizontal);
                SwhitebalanceU.setMinimum(MIN_WHITE_BALANCE);
                SwhitebalanceU.setMaximum(MAX_WHITE_BALANCE);
                SwhitebalanceU.setValue( (int)m_cvCapture.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U) );
                QLabel LwhitebalanceU;
                LwhitebalanceU.setNum( (int)m_cvCapture.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U) );
                connect(&SwhitebalanceU, SIGNAL(valueChanged(int)), &LwhitebalanceU, SLOT(setNum(int)));
                connect(&SwhitebalanceU,SIGNAL(valueChanged(int)), this, SLOT(set_white_balanceU(int)));
                connect(this, SIGNAL(set_default_white_balanceU(int)), &SwhitebalanceU, SLOT(setValue(int)));
                QLabel NwhitebalanceU("White_balanceU:");

                QSlider SwhitebalanceV;
                SwhitebalanceV.setOrientation(Qt::Horizontal);
                SwhitebalanceV.setMinimum(MIN_WHITE_BALANCE);
                SwhitebalanceV.setMaximum(MAX_WHITE_BALANCE);
                SwhitebalanceV.setValue( (int)m_cvCapture.get(CV_CAP_PROP_WHITE_BALANCE_RED_V) );
                QLabel LwhitebalanceV;
                LwhitebalanceV.setNum( (int)m_cvCapture.get(CV_CAP_PROP_WHITE_BALANCE_RED_V) );
                connect(&SwhitebalanceV, SIGNAL(valueChanged(int)), &LwhitebalanceV, SLOT(setNum(int)));
                connect(&SwhitebalanceV,SIGNAL(valueChanged(int)), this, SLOT(set_white_balanceV(int)));
                connect(this, SIGNAL(set_default_white_balanceV(int)), &SwhitebalanceV, SLOT(setValue(int)));
                QLabel NwhitebalanceV("White_balanceV:");

            sliders.addWidget(&Sbrightness);
            lineedits.addWidget(&Lbrightness, 2);
            names.addWidget(&Nbrightness, 1);

            sliders.addWidget(&Scontrast);
            lineedits.addWidget(&Lcontrast, 2);
            names.addWidget(&Ncontrast, 1);

            sliders.addWidget(&Ssaturation);
            lineedits.addWidget(&Lsaturation, 2);
            names.addWidget(&Nsaturation, 1);

            sliders.addWidget(&SwhitebalanceU);
            lineedits.addWidget(&LwhitebalanceU, 2);
            names.addWidget(&NwhitebalanceU, 1);

            sliders.addWidget(&SwhitebalanceV);
            lineedits.addWidget(&LwhitebalanceV, 2);
            names.addWidget(&NwhitebalanceV, 1);

        centralbox.addLayout(&names);
        centralbox.addLayout(&sliders);
        centralbox.addLayout(&lineedits);
    page1.setLayout( &centralbox );

    QWidget page2; // a widget for a cam gain/exposure selection
    QHBoxLayout centralbox2;
    centralbox2.setMargin(5);
        QVBoxLayout sliders2;
        sliders2.setMargin(5);
        QVBoxLayout lineedits2;
        lineedits2.setMargin(5);
        QVBoxLayout names2;
        names2.setMargin(5);

            QSlider Sgain;
            Sgain.setOrientation(Qt::Horizontal);
            Sgain.setMinimum(MIN_GAIN);
            Sgain.setMaximum(MAX_GAIN);
            Sgain.setValue( (int)m_cvCapture.get(CV_CAP_PROP_GAIN) );
            QLabel Lgain;
            Lgain.setNum( (int)m_cvCapture.get(CV_CAP_PROP_GAIN) );
            connect(&Sgain, SIGNAL(valueChanged(int)), &Lgain, SLOT(setNum(int)));
            connect(&Sgain,SIGNAL(valueChanged(int)), this, SLOT(set_gain(int)));
            connect(this, SIGNAL(set_default_gain(int)), &Sgain, SLOT(setValue(int)),Qt::DirectConnection);
            QLabel Ngain("Gain:");

            QSlider Sexposure;
            Sexposure.setOrientation(Qt::Horizontal);
            Sexposure.setMinimum(MIN_EXPOSURE);
            Sexposure.setMaximum(MAX_EXPOSURE);
            Sexposure.setValue( (int)m_cvCapture.get(CV_CAP_PROP_EXPOSURE) );
            QLabel Lexposure;
            Lexposure.setNum( (int)m_cvCapture.get(CV_CAP_PROP_EXPOSURE) );
            connect(&Sexposure, SIGNAL(valueChanged(int)), &Lexposure, SLOT(setNum(int)));
            connect(&Sexposure,SIGNAL(valueChanged(int)), this, SLOT(set_exposure(int)));
            connect(this, SIGNAL(set_default_exposure(int)), &Sexposure, SLOT(setValue(int)));
            QLabel Nexposure("Exposure:");

        sliders2.addWidget(&Sgain);
        lineedits2.addWidget(&Lgain, 2);
        names2.addWidget(&Ngain, 1);

        sliders2.addWidget(&Sexposure);
        lineedits2.addWidget(&Lexposure, 2);
        names2.addWidget(&Nexposure, 1);

        centralbox2.addLayout(&names2);
        centralbox2.addLayout(&sliders2);
        centralbox2.addLayout(&lineedits2);
    page2.setLayout( &centralbox2 );


    QHBoxLayout buttons;
    buttons.setMargin(5);
        QPushButton Baccept;
        Baccept.setText(tr("Accept"));
        connect(&Baccept, SIGNAL(clicked()), &dialog, SLOT(accept()));
        QPushButton Bcancel;
        Bcancel.setText(tr("Cancel"));
        connect(&Bcancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        QPushButton Bdefault;
        Bdefault.setText(tr("Default"));
        connect(&Bdefault, SIGNAL(clicked()), this, SLOT(set_default_settings()));
    buttons.addWidget(&Baccept);
    buttons.addWidget(&Bcancel);
    buttons.addWidget(&Bdefault);

    tabwidget.addTab(&page1, "Brightness/Contrast");
    tabwidget.addTab(&page2, "Gain/Exposure");
    toplevellayout.addWidget(&tabwidget);
    toplevellayout.addLayout(&buttons);
    dialog.setLayout(&toplevellayout);
    //DIALOG CONSTRUCTION END//
    dialog.exec();
    return true;
    }
    return false; // false if device was not opened yet
}

bool QVideoCapture::isOpened()
{
    return m_cvCapture.isOpened();
}

QVideoCapture::~QVideoCapture()
{
    delete pt_timer;
}

bool QVideoCapture::set_brightness(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_BRIGHTNESS, (double)value);
}

bool QVideoCapture::set_contrast(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_CONTRAST, (double)value);
}

bool QVideoCapture::set_saturation(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_SATURATION, (double)value);
}

bool QVideoCapture::set_gain(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_GAIN, (double)value);
}

bool QVideoCapture::set_exposure(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_EXPOSURE, (double)value);
}

bool QVideoCapture::set_white_balanceU(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, (double)value);
}

bool QVideoCapture::set_white_balanceV(int value)
{
    return m_cvCapture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, (double)value);
}

void QVideoCapture::set_default_settings()
{
    emit set_default_brightness(DEFAULT_BRIGHTNESS);
    emit set_default_contrast(DEFAULT_CONTRAST);
    emit set_default_saturation(DEFAULT_SATURATION);
    emit set_default_white_balanceU(DEFAULT_WHITE_BALANCE);
    emit set_default_white_balanceV(DEFAULT_WHITE_BALANCE);
    emit set_default_gain(DEFAULT_GAIN);
    emit set_default_exposure(DEFAULT_EXPOSURE);
}

int QVideoCapture::open_deviceSelectDialog()
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)) // if true then QCamera class is available

    QDialog dialog;
    int pointSize = dialog.font().pointSize();
    dialog.setFixedSize(30*pointSize,17*pointSize);
    dialog.setWindowTitle(tr("Device select dialog"));
    QVBoxLayout layout;
        QGroupBox GBid(tr("Select device from list"));
            QVBoxLayout Lid;
            QComboBox CBid;           
            QList<QByteArray> devices = QCamera::availableDevices();
            for(int i = 0; i < devices.size(); i++)
                CBid.addItem(QCamera::deviceDescription(devices.at(i)), qVariantFromValue(devices.at(i)));
            Lid.addWidget( &CBid );
       GBid.setLayout(&Lid);

       QHBoxLayout Lbuttons;
            QPushButton Baccept(tr("Accept"));
            connect(&Baccept, &QPushButton::clicked, &dialog, &QDialog::accept);
            QPushButton Breject(tr("Reject"));
            connect(&Breject, &QPushButton::clicked, &dialog, &QDialog::reject);
            Breject.setToolTip("Default device will be used if available");
       Lbuttons.addWidget(&Baccept);
       Lbuttons.addWidget(&Breject);
   layout.addWidget(&GBid);
   layout.addLayout(&Lbuttons);
   dialog.setLayout(&layout);

    if( dialog.exec() == QDialog::Accepted )
    {
        #ifdef Q_OS_ANDROID
        switch(CBid.currentIndex()) {
            case 0:
                device_id = CV_CAP_ANDROID_BACK;
                break;
            case 1:
                device_id = CV_CAP_ANDROID_FRONT;
                break;
        }
        #else
        device_id = CBid.currentIndex();
        #endif
    }

    #else // QCamera class is not available
    device_id = 0;
    #endif

    return device_id;
}

void QVideoCapture::initializeTimer()
{
    pt_timer = new QTimer();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    pt_timer->setTimerType(Qt::PreciseTimer);
#endif
    connect(pt_timer, SIGNAL( timeout() ), this, SLOT( read_frame() )); // makes a connection between timer signal and class slot
}
