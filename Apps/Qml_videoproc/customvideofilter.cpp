#include "customvideofilter.h"

#include <QDebug>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


QVideoFilterRunnable *CustomFilter::createFilterRunnable()
{
    return new CustomVideoFilterRunnable(this);
}

CustomVideoFilterRunnable::CustomVideoFilterRunnable(QObject *_parent)
{

}

CustomVideoFilterRunnable::~CustomVideoFilterRunnable()
{

}

QVideoFrame CustomVideoFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, QVideoFilterRunnable::RunFlags flags)
{
    if(input->isValid()) {
        qDebug() << input->pixelFormat();
        input->map(QAbstractVideoBuffer::ReadOnly);
        qDebug("input width x height: %d x %d x %d",input->width(),input->height(), input->bytesPerLine()/input->width());
        switch(input->pixelFormat()) {
            case QVideoFrame::Format_YUYV: {
                cv::Mat frame = cv::Mat(input->height(),input->width(),CV_8UC2,input->bits()).clone();
                cv::cvtColor(frame,frame,cv::COLOR_YUV2GRAY_YUYV);
                cv::Canny(frame,frame,70,110);
                /*if(!frame.empty())
                    cv::imshow("Video probe",frame);*/
                QImage _qimg((const uchar *)frame.data,frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Grayscale8);
                return QVideoFrame(_qimg.convertToFormat(QImage::Format_RGB32));
            } break;

            case QVideoFrame::Format_ABGR32: {
                cv::Mat frame = cv::Mat(input->height(),input->width(),CV_8UC4,input->bits()).clone();
                cv::cvtColor(frame,frame,cv::COLOR_BGRA2GRAY);
                cv::Canny(frame,frame,70,110);
                /*if(!frame.empty())
                    cv::imshow("Video probe",frame);*/
                QImage _qimg((const uchar *)frame.data,frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Grayscale8);
                return QVideoFrame(_qimg.convertToFormat(QImage::Format_RGB32));
            } break;

            default:

            break;
        }
        input->unmap();

    } else {
        qDebug("CustomVideoFilterRunnable::run - invalid input");
    }

    return *input;
}
