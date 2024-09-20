#ifndef SSG_H
#define SSG_H
#include <opencv2/opencv.hpp>
#define DRAW_H 1
#define DRAW_V 0




void drawLine(cv::Mat target,unsigned int rootCord,int begin, int end,unsigned int color,char HOrV);
void printDigit(cv::Mat target,int x, int y,char number,int size = 1,int color = 0xFF00FF);
void printNum(cv::Mat target,int x, int y,int number,int size = 1,int color = 0xFF00FF);


#endif /* SSG_H */
