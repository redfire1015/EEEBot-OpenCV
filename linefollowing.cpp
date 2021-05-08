// Include files for required libraries
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv_aee.hpp"
#include <string.h>
#include "main.hpp"     // You can use this file for declaring defined values and functions
using namespace std;

void setup(void)
{
    setupCamera(320, 240);  // Enable the camera for OpenCV
}

int main( int argc, char** argv )
{
    setup();    // Call a setup function to prepare IO and devices

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
            //frame_preflip = captureFrame();
            //flip(frame_preflip,frame, -1); //used to orientate the image in the right way
            frame = captureFrame();
        }
        cvtColor(frame,frameHSV,COLOR_BGR2HSV); //convert image taken from BGR to HSV
        inRange(frameHSV,Scalar(0,0,0),Scalar(179,179,60),frame_grey_pink); //makes a new image where only the HSV values in the range are presented as white on black

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
            //cv::approxPolyDP(contours[maxareaarray],approxedcontours,10,true); //approximated contour for the largest area only
            Point regionCentre=findContourCentre(contours[maxareaarray]); //calculate the centrepoint for the largest contour only
            if (regionCentre.x != -2147483648|| regionCentre.y != -2147483648 )
            {
                printf("Contour centre: x = %dpx, y = %dpx\n",regionCentre.x,regionCentre.y);

                if (regionCentre.x > 1.2*320 )
                    printf("Turn Left \n");
                else if (regionCentre.x < 0.8*320)
                    printf("Turn Right \n");
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