// *******************************************************************************
//	OpenCVKinect: Provides method to access Kinect Color and Depth Stream        *
//				  in OpenCV Mat format.                                           *
//                                                                                *
//				  Pre-requisites: OpenCV_2.x, OpenNI_2.x, KinectSDK_1.8           *
//                                                                                *
//   Copyright (C) 2013  Muhammad Asad                                            *
//                       Webpage: http://seevisionc.blogspot.co.uk/p/about.html   *
//						 Contact: masad.801@gmail.com                             *
//                                                                                *
//   This program is free software: you can redistribute it and/or modify         *
//   it under the terms of the GNU General Public License as published by         *
//   the Free Software Foundation, either version 3 of the License, or            *
//   (at your option) any later version.                                          *
//                                                                                *
//   This program is distributed in the hope that it will be useful,              *
//   but WITHOUT ANY WARRANTY; without even the implied warranty of               *
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
//   GNU General Public License for more details.                                 *
//                                                                                *
//   You should have received a copy of the GNU General Public License            *
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.        *
//                                                                                *
// *******************************************************************************


#include "OpenCVKinect.h"
#include "segm\ms.h"
#include <vector>
#include <iostream>
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <objbase.h>
#include <Windows.h>
#include <stdio.h>
#include <omp.h>

using namespace cv;
using namespace std;

void detectAndDisplay(Mat frame);
//String face_cascade_name = "haarcascade_frontalface_alt.xml";
//String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
//string window_name = "Capture - Face detection";
RNG rng(12345);


void displayIntroAndUsage()
{
	std::cout << "***********************************************************" << std::endl;
	std::cout << "OpenCVKinect Sample Implementation" << std::endl;
	std::cout << "***********************************************************" << std::endl;
	std::cout << "This example demonstrates the use of OpenCVKinect" << std::endl;
	std::cout << std::endl;

	std::cout << "Flags used: C_MODE_DEPTH -->> Acquire Depth Stream Data" << std::endl;
	std::cout << "            C_MODE_COLOR -->> Acquire Color Stream Data" << std::endl;
	std::cout << "            C_MODE_ALIGNED -->> (Optional) Register Color and Depth Streams for Aligned Frames" << std::endl;
	std::cout << "***********************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

void displayMenu()
{
	std::cout << "Please select the mode for acquiring data streams from Kinect" << std::endl;
	std::cout << "1. C_MODE_COLOR - Acquire Color Stream only" << std::endl;
	std::cout << "2. C_MODE_DEPTH - Acquire Depth Stream only" << std::endl;
	std::cout << "3. C_MODE_COLOR | C_MODE_DEPTH - Acquire both Color and Depth Streams" << std::endl;
	std::cout << "4. C_MODE_COLOR | C_MODE_DEPTH | C_ALIGNED- Acquire both Color and Depth Streams with registration" << std::endl;
	std::cout << std::endl;
	std::cout << "Please select from option 1-4" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

// colorize depth map based on solution at: http://stackoverflow.com/a/13843342
// possible colorizing options: http://docs.opencv.org/2.4/modules/contrib/doc/facerec/colormaps.html#applycolormap

//    COLORMAP_AUTUMN = 0,
//    COLORMAP_BONE = 1,
//    COLORMAP_JET = 2,
//    COLORMAP_WINTER = 3,
//    COLORMAP_RAINBOW = 4,
//    COLORMAP_OCEAN = 5,
//    COLORMAP_SUMMER = 6,
//    COLORMAP_SPRING = 7,
//    COLORMAP_COOL = 8,
//    COLORMAP_HSV = 9,
//    COLORMAP_PINK = 10,
//    COLORMAP_HOT = 11

cv::Mat colorizeDepth(const cv::Mat &inDepth, int colorOpts)
{
	// define min and max for kinec : http://stackoverflow.com/a/7698010
	// for Kinect 360 this is
	// min --> 600  - 0.4m
	// max --> 4000 - 4m  

	double minDistance, maxDistance;
	//minDistance = 600;
	//maxDistance = 4000;

	// or alternatively we can look at min max of the current depth frame
	// this is not good for dynamic scene as color representation changes at each frame
	// depending on the farthest object
	cv::minMaxIdx(inDepth, &minDistance, &maxDistance);
	cv::Mat adjustedMap;
	// expand your range to 0..255. Similar to histEq();
	inDepth.convertTo(adjustedMap, CV_8UC1, 255 / (maxDistance - minDistance), -minDistance);

	// colorize the depth map
	cv::Mat colorizedDepth;
	cv::applyColorMap(adjustedMap, colorizedDepth, colorOpts);

	//cv::imshow("Out", colorizedDepth);

	return colorizedDepth;
}

inline bool compareModes(vector<int> mode1, double* mode2, int dim, double tolerance)
{
	for (int i = 0; i < dim; i++)
		if (abs(mode1[i] - mode2[i]) > tolerance) return false;
	return true;
}

std::vector<vector<int>> modeSeeking(const std::vector<vector<int>>& distribution)
{
	
	int n = (int)distribution.size();
	int dim = 3;
	double tolerance = 5.0; //TODO: check impact and sense behind tolerance

	if (n == 0) return std::vector<vector<int>>();

	MeanShift ms;

	// kernel definition
	kernelType kT[] = { Gaussian }; // kernel type
	float h[] = { 100 }; // bandwidth in millimeters
	int P[] = { dim }; // subspace definition, we have only 1 space of dimension 3
	int kp = 1; // subspace definition, we have only 1 space of dimension 3
	ms.DefineKernel(kT, h, P, kp);

	// set input data
	float* data = new float[n*dim];
#pragma omp parallel for
	for (int i = 0; i < n; i++)
#pragma omp parallel for
		for (int j = 0; j < dim; j++)
		{
			data[i * dim + j] = distribution[i][j];
		}

	// data[0] = data[0 * dim + 0] = pt_0.x
	// data[1] = data[0 * dim + 1] = pt_0.y
	// data[2] = data[0 * dim + 2] = pt_0.z
	// data[3] = data[1 * dim + 0] = pt_1.x
	// data[4] = data[1 * dim + 1] = pt_1.y

	ms.DefineInput(data, n, dim);

	// find modes
	vector<vector<int>> modes;
	vector<vector<vector<int>>> support;

	double* mode = new double[dim];
	double* point = new double[dim];
	vector<int> newSuppPoint;
	
	cout << n << endl;
	#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			mode[j] = 0.f;
			point[j] = distribution[i][j];
			newSuppPoint.push_back(distribution[i][j]);
		}
		//cout << "newsupppoint" << endl;
		ms.FindMode(mode, point);
		bool modeFound = false;
		for (int j = 0; j < modes.size(); j++)
		{
			if (compareModes(modes[j], mode, dim, tolerance))
			{
				support[j].push_back(newSuppPoint);
				modeFound = true;
				break;
			}
		}
		if (!modeFound)
		{
			vector<int> newMode;
			for (int j = 0; j < dim; j++) newMode.push_back(mode[j]);
			modes.push_back(newMode);
			std::vector<vector<int>> newSupport;
			newSupport.push_back(newSuppPoint);
			support.push_back(newSupport);
		}
	}
	
	// clean up
	delete[] mode;
	delete[] point;
	delete[] data;
	
	// you may want to do some filtering, e.g. remove very small modes (that have just a few points)

	return modes;
}

int main()
{

	OpenCVKinect myKin;
	/*
	VideoCapture capture(C_DEPTH_STREAM);
	//cout << cv::getBuildInformation();
	Mat depthMap;
	capture >> depthMap;

	capture.grab();

	capture.retrieve(depthMap, CV_CAP_OPENNI_POINT_CLOUD_MAP);

	imshow("RGB", depthMap); // Show our image inside it.*/

	cv::vector<cv::Mat> dataStream;
	myKin.setMode(C_MODE_DEPTH | C_MODE_COLOR);

	if (!myKin.init())
	{
		std::cout << "Error initializing" << std::endl;
		return 1;
	}

	char ch = ' ';

	while (ch != 27) {
		dataStream = myKin.getData();
		std::cout << dataStream[0].at<ushort>(50, 10) << std::endl;

		cv::Rect myROI(100, 100, 300, 300);
		cv::vector<cv::Mat> cuttedStream;
		cv::Mat cuttedMat = dataStream[0](myROI);
		cuttedStream.push_back(cuttedMat);


/*
		//------------------------------face tracking--------------------------------------

		if (!face_cascade.load("C:/Users/Yang/Downloads/OpenCVKinect-master test/OpenCVKinect-master/OpenCVKinect/haarcascade_frontalface_alt.xml")) { printf("--(!)Error loading face\n"); return -1; };
		if (!eyes_cascade.load("C:/Users/Yang/Downloads/OpenCVKinect-master test/OpenCVKinect-master/OpenCVKinect/haarcascade_eye_tree_eyeglasses.xml")) { printf("--(!)Error loading\n"); return -1; };

		
		std::vector<Rect> faces;
		Mat frame_gray;

		cvtColor(frame, frame_gray, CV_BGR2GRAY);

		int faceX=0, faceY=0;

		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		
		for (size_t i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

			//faceX = faces[i].x;
			//faceY = faces[i].y;
			//ellipse(depthframe, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

			//cout << "face_position" + faceX << endl;
			/*Mat faceROI = frame_gray(faces[i]);
			std::vector<Rect> eyes;

			//-- In each face, detect eyes
			eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

			for (size_t j = 0; j < eyes.size(); j++)
			{
			Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			}
			
		}
		*/
	
		//-- Show what you got
		//imshow(window_name, frame);


		//cv::Mat madataStream.data();
		//int i = mat.type();

		//std::cout << i << std::endl;

		

		float imageWidth = 640;
		float imageHeight = 480;
		float focalLength = 585;

		float dim = 3;
		float size = imageHeight*imageWidth;
		vector< vector<int> > pointCloudStream(size, vector<int>(dim));
		vector<vector<int>> modes;
		//long pointCloudStream[640*480][3];
		int current = 0;
		//#pragma omp parallel for
		for (int y = 0; y < imageHeight - 1; y++) {
			for (int x = 0; x < imageWidth - 1; x++) {
				ushort depth = dataStream[0].at<ushort>(y, x);
				//std::cout <<  << std::endl;
				if (depth != NULL) {
					if (depth == 0) {
						pointCloudStream[current][0] = 0;
						pointCloudStream[current][1] = 0;
						pointCloudStream[current][2] = 0;
					}
					else {
						/*std::cout << "d: " << depth << std::endl;
						std::cout << "f: " << focalLength << std::endl;
						std::cout << "fd: " << (focalLength / depth) << std::endl;
						std::cout << "nenner: " << ((x - (imageWidth / 2))) << std::endl;
						std::cout << "r: " << ((x - (imageWidth / 2)) / (focalLength / depth)) << std::endl;*/

						float px = ((x - (imageWidth / 2)) / (focalLength / depth));
						float py = -((y - (imageWidth / 2)) / (focalLength / depth));
						float pz = -depth;
						pointCloudStream[current][0] = px;
						pointCloudStream[current][1] = py;
						pointCloudStream[current][2] = pz;
						current += 1;
					}
				}
			}
		}

 
		//cv::InputArray cuttedArray = new InputArray(&pointCloudStream);


		

		//modes = modeSeeking(pointCloudStream);
		

		cv::imshow("Colorized Depth", colorizeDepth(cuttedStream[C_DEPTH_STREAM], 0));
		
		
		cv::Mat frame = dataStream[C_COLOR_STREAM];
		cv::Mat depthframe = dataStream[C_DEPTH_STREAM];
		

		
		//cv::imshow("Color", frame);
		//cv::imshow("Depth_Color", depthframe);





		ch = cv::waitKey(10);
		if (waitKey(30) >= 0)
			break;

		
	}

	
	/*for (;;)
	{




	//std::cout << depthMap.cols << " " << depthMap.rows << std::endl;

	//imshow("RGB", depthMap); // Show our image inside it.
	//if (waitKey(30) >= 0)
	//break;
	}*/
	/*
	displayIntroAndUsage();
	cv::vector<cv::Mat> dataStream;
	OpenCVKinect myDataCap;

	int selectedOption = 0;
	DISPLAY_OPTIONS:
	displayMenu();

	std::cin >> selectedOption;
	switch(selectedOption)
	{
	case 1:
	myDataCap.setMode(C_MODE_COLOR);
	break;
	case 2:
	myDataCap.setMode(C_MODE_DEPTH);
	break;
	case 3:
	myDataCap.setMode(C_MODE_DEPTH | C_MODE_COLOR);
	break;
	case 4:
	myDataCap.setMode(C_MODE_DEPTH | C_MODE_COLOR | C_MODE_ALIGNED);
	break;
	default:
	std::cout << "Error: Invalid option selected." << std::endl;
	goto DISPLAY_OPTIONS;
	break;
	}

	if(!myDataCap.init())
	{
	std::cout << "Error initializing" << std::endl;
	return 1;
	}

	char ch = ' ';
	int colorOpts = 0;

	while(ch != 27)
	{
	dataStream = myDataCap.getData();
	std::cout << dataStream.size() <<  std::endl;

	//cv::imshow("Depth", dataStream[C_DEPTH_STREAM]);

	// if depth map is present then colorize it and display
	if(selectedOption > 1)
	cv::imshow("Colorized Depth", colorizeDepth(dataStream[C_DEPTH_STREAM], colorOpts));

	//cv::imshow("Color", dataStream[C_COLOR_STREAM]);
	ch = cv::waitKey(10);

	if(ch == 't')
	{
	// toggle through different color maps
	colorOpts = (colorOpts + 1)%11;
	}
	}
	return 1;
	*/
}