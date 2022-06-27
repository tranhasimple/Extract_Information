#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <string>
#include <tesseract/baseapi.h>
#include <math.h>
using namespace cv;
using namespace std;

RNG rng(12345);

int getMaxAreaContourId(vector <vector<cv::Point>> contours) {
	double maxArea = 0;
	int maxAreaContourId = -1;
	for (int j = 0; j < contours.size(); j++) {
		double newArea = cv::contourArea(contours.at(j));
		if (newArea > maxArea) {
			maxArea = newArea;
			maxAreaContourId = j;
		}
	}
	return maxAreaContourId;
}

std::vector<cv::Rect> detectLetters(cv::Mat img)
{
	// Find contours
	std::vector<cv::Rect> boundRect;
	cv::Mat img_gray, img_sobel, img_threshold, element;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);
	cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::threshold(img_sobel, img_threshold, 0, 255, THRESH_OTSU + THRESH_BINARY);
	element = getStructuringElement(cv::MORPH_RECT, cv::Size(20, 7));
	cv::morphologyEx(img_threshold, img_threshold, MORPH_GRADIENT, element);
	std::vector< std::vector< cv::Point> > contours;
	cv::findContours(img_threshold, contours, 0, 1);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
		if (contours[i].size() > 100)
		{
			cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
			cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
			if (appRect.width > appRect.height)
				boundRect.push_back(appRect);
		}
	return boundRect;
}

string trim(const string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

int main() {
	Mat img, img_resized, img_original, img_gray, img_canny, edged, hstack;
	img = imread("D:/AI_PHOTOS/data/student3.jpg");
	//img_original = img.copyTo();

	resize(img, img_resized, Size(), 0.5, 0.5);


	cvtColor(img_resized, img_original, COLOR_BGR2GRAY);
	bilateralFilter(img_original, img_gray, 20, 30, 30);
	Canny(img_gray, edged, 10, 20);

	hconcat(edged, img_gray, hstack);

	// Find contour for image
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(edged, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	int index = getMaxAreaContourId(contours);

	Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
	drawContours(img_resized, contours, (int)index, color, 2, LINE_8, hierarchy, 0);

	// Find four corner points
	vector<cv::Point2i> approx;
	approxPolyDP(contours[index], approx, 30, true);
	auto mean = cv::mean(approx);
	vector<cv::Point2i> bottomCorners;
	vector<cv::Point2i> topCorners;

	for (auto p : approx)
	{
		if (p.y > mean[1]) bottomCorners.push_back(p);
		if (p.y < mean[1]) topCorners.push_back(p);
	}

	circle(img_resized, topCorners[1], 5, color, 10);

	// Use perspective transformation to rotate cropped image
	Mat output;
	Point2f Poly2[4] = {
				  topCorners[0],
				   topCorners[1],
				  bottomCorners[0],
				  bottomCorners[1],
		};

	Point2f Points[4] = {
				  cv::Point2f(0,0),
				  cv::Point2f(432,0),
				  cv::Point2f(0,576),
				  cv::Point2f(432,576),
	};

	cv::Mat Matrix = cv::getPerspectiveTransform(Poly2, Points);

	cv::warpPerspective(img_resized, output, Matrix, cv::Size(432, 576));
	
	//OCR time
	vector<Rect> letterBBoxes1 = detectLetters(output);

	tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
	ocr->Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng", tesseract::OEM_LSTM_ONLY);
	ocr->SetPageSegMode(tesseract::PSM_AUTO);

	for (int i = 0; i < letterBBoxes1.size(); i++) {

		rectangle(output, letterBBoxes1[i], cv::Scalar(0, 255, 0), 1, 1, 0);
		Mat cropped_image = output(letterBBoxes1[i]);
		ocr->SetImage(cropped_image.data, cropped_image.cols, cropped_image.rows, 3, cropped_image.step);
		string outText = string(ocr->GetUTF8Text());
		outText.erase(std::remove(outText.begin(), outText.end(), '\n'), outText.end());
		putText(output, trim(outText), cv::Point(letterBBoxes1[i].x, letterBBoxes1[i].y - 10), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(255, 255, 255), 1, false);
	}


	ocr->End();

	imshow("origin", img_resized);

	imshow("Output", output);
	imwrite("output.jpg", output);
	//imshow("gray and edged", hstack);
	waitKey(0);
	destroyAllWindows();
}
