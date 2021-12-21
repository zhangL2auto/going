/*
 * @Project: port_parking
 * @version: 1.0
 * @Author: lZhang
 * @Date: 2021-12-14 15:12:47
 */
#include"port_parking.h"

float detect_line(cv::Mat frame)
{
    cv::Mat gray, gray_blur, canny, dilate;
    float scale = 0.25;
    cv::resize(frame, frame, cv::Size(0,0), scale, scale);
    cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
    cv::GaussianBlur(gray, gray_blur, cv::Size(9,9), 1);
    cv::Canny(gray_blur, canny,150,200);
    std::vector<cv::Vec2f> plines;
    cv::HoughLines(canny, plines, 0.8, CV_PI/ 180, 80, 0, 0);

    if (plines.size() < 2)
    {
        return 0;
    }
    float r, th;
    std::vector<float> X;
    for (auto line : plines)
    {
        r = line[0];
        X.push_back(r);
    }
    std::vector<int> labels;
    std::vector<float> centers;
    cv::kmeans(X, 2, labels,     
    cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 50, 0.1),
    3, cv::KMEANS_PP_CENTERS, centers);

    int l0_cnt, l1_cnt;
    float r0_sum, r1_sum, th0_sum, th1_sum, r0_avg, r1_avg, th0_avg, th1_avg;
    for(int i = 0; i < labels.size(); i++)
    {
        r = plines[i][0];
        th = plines[i][1];
        if (labels[i] == 0)
        {
            l0_cnt += 1;
            r0_sum += r;
            th0_sum += th;
        }
        if (labels[i] == 1)
        {
            l1_cnt += 1;
            r1_sum += r;
            th1_sum += th;
        }
    }
    r0_avg = r0_sum / l0_cnt;
    th0_avg = th0_sum / l0_cnt;
    r1_avg = r1_sum / l1_cnt;
    th1_avg = th1_sum / l1_cnt;

    float r_avg = (r0_avg + r1_avg) / 2;
    float th_avg = (th0_avg + th1_avg) / 2; 

    return th_avg;
}

bool verify_line(float th, float lower, float upper)
{
    if (th < upper && th > lower)
    {
        return true;
    }
    else
    {
        return false;
    }
}