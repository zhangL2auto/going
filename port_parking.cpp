/*
 * @Project: your project
 * @version: 1.0
 * @Author: lZhang
 * @Date: 2021-12-14 15:12:47
 */
#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>
#include<time.h>

void draw_line(cv::Mat img, float r, float th)
{
    float a = cos(th);
    float b = sin(th);
    float x0 = a * r;
    float y0 = b * r;
    cv::Point pt1, pt2;
    pt1.x = cvRound(x0 + 2000*(-b));
    pt1.y = cvRound(y0 + 2000*a);
    pt2.x = cvRound(x0 - 2000*(-b));
    pt2.y = cvRound(y0 - 2000*a);
    cv::Scalar color = cv::Scalar(53, 57, 229);
    std::string notification = "NO";
    if (th < 1.6 && th > 1.5)
    {
        color = cv::Scalar(71, 160, 67);
        notification = "OK";
    }
    cv::line(img, pt1, pt2, color, 3);
    cv::putText(img, notification, cv::Point(200,250), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2);

}

cv::Mat yellow_enhance(cv::Mat img)
{
    cv::Mat img_hsv, mask, gray, result;
    cv::cvtColor(img, img_hsv, cv::COLOR_RGB2HSV);
    cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
    cv::Scalar lower_color = cv::Scalar(40, 100, 20);
    cv::Scalar upper_color = cv::Scalar(100, 255, 255);
    cv::inRange(img_hsv, lower_color, upper_color, mask);
    cv::addWeighted(mask, 1.0, gray, 1.0, 1.0, result);
}

cv::Mat detect_line(cv::Mat img)
{
    cv::Mat img_copy, gray, gray_blur, canny, dilate;
    img_copy = img;
    gray = img;  
    cv::cvtColor(img_copy, gray, cv::COLOR_RGB2GRAY);
    cv::GaussianBlur(gray, gray_blur, cv::Size(9,9), 1);
    cv::Canny(gray_blur, canny,150,200);
    std::vector<cv::Vec2f> plines;
    cv::HoughLines(canny, plines, 0.8, CV_PI/ 180, 80, 0, 0);

    if (plines.size() < 2)
    {
        return img;
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
    draw_line(img, r_avg, th_avg);
    return img;
}
void parking_assist(std::string video_path, std::string out_path)
{

    cv::VideoCapture cap;
    cv::Mat frame;
    double scale = 0.25;
    frame = cap.open(video_path);
    cv::Size size = cv::Size(cvRound(cap.get(cv::CAP_PROP_FRAME_WIDTH)*scale),cvRound(cap.get(cv::CAP_PROP_FRAME_HEIGHT)*scale));
    int fps = cap.get(cv::CAP_PROP_FPS);
    cv::VideoWriter out;
    out.open(out_path, cv::VideoWriter::fourcc('m','p','4','v'), fps, size, true);

    while (cap.read(frame))
    {
        cv::Mat result = yellow_enhance(frame);
        cv::resize(frame, frame, cv::Size(0,0),scale,scale);
        cv::imshow("img",frame);
        frame = detect_line(frame);
        out.write(frame); 
    }
    cap.release();
    out.release();
    std::cout<<"finished!"<<std::endl;
}
int main()
{
    std::string video_path = "VID_20210918_090448.mp4";
    std::string out_path = "out.mp4";
    parking_assist(video_path,out_path);
    
    return 0;
}
