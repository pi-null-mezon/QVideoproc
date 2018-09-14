/*------------------------------------------------------------------------------------------------------
Taranov Alex, 2014									     SOURCE FILE
Class that wraps opencv functions into Qt SIGNAL/SLOT interface
The simplest way to use it - rewrite appropriate section in QOpencvProcessor::custom_algorithm(...) slot
------------------------------------------------------------------------------------------------------*/

#include "qopencvprocessor.h"

//------------------------------------------------------------------------------------------------------

QOpencvProcessor::QOpencvProcessor(QObject *parent):
    QObject(parent)
{
    //Initialization
    m_cvRect.width = 0;
    m_cvRect.height = 0;
    m_timeCounter = cv::getTickCount();
}

//-----------------------------------------------------------------------------------------------------

void QOpencvProcessor::update_timeCounter()
{
    m_timeCounter = cv::getTickCount();
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::custom_algorithm(const cv::Mat &input)
{
    cv::Mat output(input); // Copy the header and pointer to data of input object
    cv::Mat temp; // temporary object

    //-------------CUSTOM ALGORITHM--------------

    //You can do here almost whatever you wnat...
    cv::cvtColor(input,temp,CV_BGR2GRAY);
    cv::Canny(temp,output, 15, 300);

    //----------END OF CUSTOM ALGORITHM----------

    //---------Drawing of rectangle--------------
    if( (m_cvRect.area() > 0) )
        cv::rectangle( output , m_cvRect, cv::Scalar(255,255,255), 1, CV_AA); // white color

    //-------------Time measurement--------------
    m_framePeriod = (cv::getTickCount() -  m_timeCounter) * 1000.0 / cv::getTickFrequency(); // result is calculated in milliseconds
    m_timeCounter = cv::getTickCount();

    emit frame_was_processed(output, m_framePeriod);
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::setRect(const cv::Rect &input_rect)
{
     m_cvRect = input_rect;
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::searchFace(const cv::Mat &img)
{
    cv::Mat temp = img;
    if(!m_classifier.empty()) {
        std::vector<cv::Rect> v_objects;
        m_classifier.detectMultiScale(temp, v_objects, 1.1, 7, CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(100,100));
        for(uint i = 0; i < v_objects.size(); i++ ) {
            cv::rectangle(temp, v_objects[i], cv::Scalar(0,255,255));
        }
    }    
    m_framePeriod = (cv::getTickCount() -  m_timeCounter) * 1000.0 / cv::getTickFrequency(); // result is calculated in milliseconds
    m_timeCounter = cv::getTickCount();
    emit frame_was_processed(temp, m_framePeriod);
}

//------------------------------------------------------------------------------------------------------

bool QOpencvProcessor::loadClassifier(const std::string &fileName)
{
    return m_classifier.load(fileName);
}
