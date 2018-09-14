/*------------------------------------------------------------------------------------------------------
Taranov Alex, 2014									     HEADER FILE
Class that wraps opencv functions into Qt SIGNAL/SLOT interface
The simplest way to use it - rewrite appropriate section in QOpencvProcessor::custom_algorithm(...) slot
------------------------------------------------------------------------------------------------------*/

#ifndef QOPENCVPROCESSOR_H
#define QOPENCVPROCESSOR_H
//------------------------------------------------------------------------------------------------------

#include <QObject>
#include <opencv2/opencv.hpp>

//------------------------------------------------------------------------------------------------------

class QOpencvProcessor : public QObject
{
    Q_OBJECT
public:
    explicit QOpencvProcessor(QObject *parent = 0);

signals:
    void frame_was_processed(const cv::Mat& value, double frame_period); //should be emited in the end of each frame processing
    void objectFound(const cv::Mat &objImg, double frame_period);

public slots:
    void custom_algorithm(const cv::Mat& input);    // just a template of how a program logic should work
    void update_timeCounter();                       // use it in the beginning of any time-measurement operations
    void setRect(const cv::Rect &input_rect);       // set m_cvrect
    void searchFace(const cv::Mat &img);
    bool loadClassifier(const std::string &fileName);

private:
    int64 m_timeCounter;                        // stores time of application/computer start
    double m_framePeriod;                             // stores time of frame processing
    cv::Rect m_cvRect;                                // this rect is used by process_rectregion_pulse slot
    cv::CascadeClassifier m_classifier;
};

//------------------------------------------------------------------------------------------------------
#endif // QOPENCVPROCESSOR_H
