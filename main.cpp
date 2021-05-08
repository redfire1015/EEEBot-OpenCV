// Include files for required libraries
#include <stdio.h>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "opencv_aee.hpp"
#include "main.hpp"     // You can use this file for declaring defined values and functions


void setup(void)
{
    /// Setup camera won't work if you don't have a compatible webcam
    //setupCamera(320, 240);  // Enable the camera for OpenCV
}

int main( int argc, char** argv )
{
    setup();    // Call a setup function to prepare IO and devices

    while(1)    // Main loop to perform image processing
    {
        Mat frame;
        Mat frameHSV;
        Mat frame_grey_red;
        Mat frame_grey_green;
        Mat frame_grey_blue;

        // Can't capture frames without a camera attached. Use static images instead
        while(frame.empty())
        {
            /// Can't capture frames without a camera attached. Use static images instead
            //frame = captureFrame(); // Capture a frame from the camera and store in a new matrix variable
            frame = readImage("GreenApple.bmp");

            cvtColor(frame,frameHSV,COLOR_BGR2HSV); //convert image from BGR to HSV

            inRange(frameHSV,Scalar(80,100,50),Scalar(130,255,255),frame_grey_blue);
            inRange(frameHSV,Scalar(0,0,0),Scalar(10,255,240),frame_grey_red);
            inRange(frameHSV,Scalar(40,100,40),Scalar(72,255,255),frame_grey_green);

            int numNonZeroBlue = 0;// Create an integer to store the result
            int numNonZeroRed = 0;// Create an integer to store the result
            int numNonZeroGreen = 0;// Create an integer to store the result

            numNonZeroBlue=countNonZero(frame_grey_blue);// Count the number of non-zero pixels
            numNonZeroRed=countNonZero(frame_grey_red);// Count the number of non-zero pixels
            numNonZeroGreen=countNonZero(frame_grey_green);// Count the number of non-zero pixels

            if (numNonZeroBlue>numNonZeroRed && numNonZeroBlue > numNonZeroGreen)
                std::cout << "Blue detected" << std::endl;
            else if (numNonZeroRed>numNonZeroBlue && numNonZeroRed > numNonZeroGreen)
                std::cout << "Red detected" << std::endl;
            else if (numNonZeroGreen>numNonZeroRed && numNonZeroGreen > numNonZeroBlue)
                std::cout << "Green detected" << std::endl;

            //imshow("PhotoHSV",frameHSV);
            imshow("Photo",frame);
            imshow("Photo_grey_blue",frame_grey_blue);
            imshow("Photo_grey_red",frame_grey_red);
            imshow("photo_grey_green",frame_grey_green);
        }

        int key = cv::waitKey(1);   // Wait 1ms for a keypress (required to update windows)
        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
            break;
    }

    closeCV();  // Disable the camera and close any windows
    return 0;
}




