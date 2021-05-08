// Include files for required libraries
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv_aee.hpp"
#include "pi2c.h"
#include <string.h>


#include "main.hpp"     // You can use this file for declaring defined values and functions
using namespace std;

//#include "pi2c.h"

//Pi2c car(0x22); // Configure the I2C interface to the Car as a global variable

void setup(void)
{
    setupCamera(320, 240);  // Enable the camera for OpenCV
}

int main( int argc, char** argv )
{
    Pi2c arduino(7);
    setup();    // Call a setup function to prepare IO and devices

    //Variables used for 90 degree right
    Mat symbol_Left;
    Mat symbol_LeftHSV;
    Mat symbol_Left_grey_pink;

    //variables used for 90 right
    Mat symbol_Right;
    Mat symbol_RightHSV;
    Mat symbol_Right_grey_pink;

    //variables used for 180
    Mat symbol_180;
    Mat symbol_180HSV;
    Mat symbol_180_grey_pink;

    //variables used for 360
    Mat symbol_360;
    Mat symbol_360HSV;
    Mat symbol_360_grey_pink;

    //symbol used for reverse
    Mat symbol_Reverse;
    Mat symbol_ReverseHSV;
    Mat symbol_Reverse_grey_pink;

    //symbol used for reverse
    Mat symbol_Forward;
    Mat symbol_ForwardHSV;
    Mat symbol_Forward_grey_pink;

    //reading in reference image for left
    symbol_Left = readImage("Football.PNG");
    cvtColor(symbol_Left,symbol_LeftHSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_LeftHSV,Scalar(100,40,70),Scalar(179,255,255),symbol_Left_grey_pink);

    //reading in reference image for right
    symbol_Right = readImage("GreenShortCut.PNG");
    cvtColor(symbol_Right,symbol_RightHSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_RightHSV,Scalar(100,40,70),Scalar(179,255,255),symbol_Right_grey_pink);

    //reading in reference image for 180
    symbol_180 = readImage("RedShortCut.PNG");
    cvtColor(symbol_180,symbol_180HSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_180HSV,Scalar(100,40,70),Scalar(179,255,255),symbol_180_grey_pink);

    //reading in reference image for 360
    symbol_360  = readImage("StopLight.PNG");
    cvtColor(symbol_360,symbol_360HSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_360HSV,Scalar(100,40,70),Scalar(179,255,255),symbol_360_grey_pink);

    //reading in reference image for reverse for n seconds where n is number of shapes in this image
    symbol_Reverse= readImage("DistanceMeasurement.PNG");
    cvtColor(symbol_Reverse,symbol_ReverseHSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_ReverseHSV,Scalar(130,130,0),Scalar(179,255,255),symbol_Reverse_grey_pink);

    //reading in reference image for forward for n seconds where n is the number of shapes in this image
    symbol_Forward= readImage("ShapeCounter.PNG");
    cvtColor(symbol_Forward,symbol_ForwardHSV,COLOR_BGR2HSV); //convert image from BGR to HSV
    inRange(symbol_ForwardHSV,Scalar(130,130,0),Scalar(179,255,255),symbol_Forward_grey_pink);


    while(1)    // Main loop to perform image processing
    {
        //variables for frame input
        Mat frame_preflip;
        Mat frame;
        Mat frameHSV;
        Mat frame_grey_pink;
        Mat transformed;

        while(frame.empty())
        {
            /// Can't capture frames without a camera attached. Use static images instead
            //frame = readImage("DistanceMeasurement.PNG");
            frame_preflip = captureFrame();
            flip(frame_preflip,frame, -1); //used to orientate the image in the right way
            //frame = captureFrame();
        }
        cvtColor(frame,frameHSV,COLOR_BGR2HSV); //convert image taken from BGR to HSV
        inRange(frameHSV,Scalar(100,40,70),Scalar(179,255,255),frame_grey_pink); //makes a new image where only the HSV values in the range are presented as white on black

        int maxarea; //variables for calculating max contour area
        int maxareaarray = 0;

        std::vector< std::vector<cv::Point> > contours;// Variable for list of contours
        std::vector<Vec4i> hierarchy;// Variable for image topology data

        cv::findContours(frame_grey_pink,contours,hierarchy,RETR_TREE, CHAIN_APPROX_SIMPLE,Point(0,0));// Calculate the contours and store them
        int num_cont = contours.size(); //variable for number of contours
        //cout<<num_cont<<endl;

        if (num_cont >= 1) //only calculates the max area of contours if there are any in the image
        {
            for(int i = 0; i < contours.size(); i++)//calculated the area for each of the contours
            {
                //drawContours( frame, contours, i, Scalar(0,0,255), 2, LINE_8, noArray(), 0, Point() );
                int area=cv::contourArea(contours[i]); //stores the area of the contours for the position in the loop
                //cout<< i << " " << area <<endl;

                if (i == 0 ) //if its the first time in the loop max area is the the first area
                {
                    maxarea = area;
                }
                if (maxarea < area) //checks to see if the current max area is actually the largest area available.
                {
                    maxarea = area;
                    maxareaarray = i;
                }
            }

            std::vector<cv::Point>approxedcontours; //variable for approximated contour
            cv::approxPolyDP(contours[maxareaarray],approxedcontours,10,true); //approximated contour for the largest area only
            transformed = transformPerspective(approxedcontours, frame, 320,240); //perspective transformed only using the largest area (the outline)
        }

        Mat transformed_hsv; //variables for converting the transformed image
        Mat transformed_grey_pink;

        if (transformed.empty() == 0) //only shows the transformed image if it has been created.
        {
            cvtColor(transformed,transformed_hsv,COLOR_BGR2HSV); //convert image from BGR to HSV
            inRange(transformed_hsv,Scalar(100,40,70),Scalar(179,255,255),transformed_grey_pink);

            cv::imshow("Transformed_input_grey_pink", transformed_grey_pink); //

            int match_left = compareImages(transformed_grey_pink,symbol_Left_grey_pink);
            //std::cout << match_left << std::endl;

            int match_right = compareImages(transformed_grey_pink,symbol_Right_grey_pink);
            //std::cout << match_right << std::endl;

            int match_180 = compareImages(transformed_grey_pink,symbol_180_grey_pink);
            //std::cout << match_180 << std::endl;

            int match_360 = compareImages(transformed_grey_pink,symbol_360_grey_pink);
            //std::cout << match_360 << std::endl;

            int match_Reverse = compareImages(transformed_grey_pink,symbol_Reverse_grey_pink);
            //std::cout << match_Reverse << std::endl;

            int match_Forward = compareImages(transformed_grey_pink, symbol_Forward_grey_pink);

            if (match_Reverse >60 || match_left > 60 || match_right > 60 || match_180 > 60 || match_360 >60 || match_Forward > 60)
            {
                if (match_Reverse > match_left && match_Reverse > match_right && match_Reverse > match_180 && match_Reverse > match_360 && match_Reverse > match_Forward)
                {
                    //do function when symbol for reverse seen
                    std::cout << "Reverse" << std::endl;
                    //std::cout << num_cont << std::endl;

                    std::vector< std::vector<cv::Point> > contours_reverse;// Variable for list of contours
                    std::vector<Vec4i> hierarchy_reverse;// Variable for image topology dat

                    cv::findContours(transformed_grey_pink,contours_reverse,hierarchy_reverse,RETR_TREE, CHAIN_APPROX_SIMPLE,Point(0,0));// Calculate the contours and store them
                    int num_shape_reverse = contours_reverse.size();
                    std::cout << num_shape_reverse << std::endl;
                    arduino.i2cWriteArduinoInt((100+num_shape_reverse));
                }

                else if (match_left > match_Reverse && match_left > match_right && match_left > match_180 && match_left > match_360 && match_left > match_Forward)
                {
                    //do function when symbol for left seen
                    std::cout << "left" << std::endl;
                    arduino.i2cWriteArduinoInt(2);
                }
                else if (match_right > match_Reverse && match_right > match_left && match_right > match_180 && match_right > match_360 && match_right > match_Forward)
                {
                    //do function when symbol for right seen
                    std::cout << "Right" << std::endl;
                    arduino.i2cWriteArduinoInt(3);
                }
                else if (match_180 > match_Reverse && match_180 > match_left && match_180 > match_right && match_180 > match_360 && match_180 > match_Forward)
                {
                    //do function when symbol for 180 turn seen
                    std::cout << "180" << std::endl;
                    arduino.i2cWriteArduinoInt(4);
                }
                else if (match_360 > match_Reverse && match_360 > match_left && match_360 > match_right && match_360 > match_180 && match_360 > match_Forward)
                {
                    //do function when symbol for 360 turn seen
                    std::cout << "360" << std::endl;
                    arduino.i2cWriteArduinoInt(5);
                }
                else if (match_Forward > match_Reverse && match_Forward > match_left && match_Forward > match_right && match_Forward > match_180 && match_Forward > match_360)
                {
                    //do function when symbol for forward seen
                    std::cout << "Forward" << std::endl;
                    //std::cout << num_cont << std::endl;

                    std::vector< std::vector<cv::Point> > contours_forward;// Variable for list of contours
                    std::vector<Vec4i> hierarchy_forward;// Variable for image topology dat

                    cv::findContours(transformed_grey_pink,contours_forward,hierarchy_forward,RETR_TREE, CHAIN_APPROX_SIMPLE,Point(0,0));// Calculate the contours and store them
                    int num_shape_forward = contours_forward.size();
                    std::cout << num_shape_forward << std::endl;
                    arduino.i2cWriteArduinoInt(num_shape_forward + 50);
                }
                /*
                char receive[16]; //Create a buffer of char (single bytes) for the data
                char confirmdone[16] = "done";
                arduino.i2cRead(receive,16);
                std::cout << receive << std::endl;

                while (strcmp(receive,confirmdone) != 0)
                {
                    arduino.i2cRead(receive,16);
                    std::cout << receive << std::endl;
                }
                std::cout << "out of the loop" << std::endl;
                //while (1);
                */
            }
        }
        //cv::imshow("Input", frame); //Display the image in the window;
        cv::imshow("Input_Pink_filter", frame_grey_pink); //Display the image in the window

        int key = cv::waitKey(1);   // Wait 1ms for a keypress (required to update windows)

        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
            break;
    }

    closeCV();  // Disable the camera and close any windows

    return 0;
}




