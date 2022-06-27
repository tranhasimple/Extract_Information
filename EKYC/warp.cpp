#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include<string>
using namespace cv;
using namespace std;

RNG rng(12345);

bool Contour_Area(vector<Point>, vector<Point>);
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
int main() {
	Mat img, img_resized, img_original, img_gray, img_canny, edged, hstack;
	img = imread("D:/AI_PHOTO/data/student3.jpg");
	//img_original = img.copyTo();

	resize(img, img_resized, Size(), 0.5, 0.5);

	cvtColor(img_resized, img_original, COLOR_BGR2GRAY);
	bilateralFilter(img_original, img_gray, 20, 30, 30);
	Canny(img_gray, edged, 10, 20);

	hconcat(edged, img_gray, hstack);

	// Find contour for image
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(edged, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	sort(contours.begin(), contours.end(), Contour_Area);
	int index = getMaxAreaContourId(contours);

	// scale color to 
	Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

	// draw Contour on image
	drawContours(img_resized, contours, (int)index, color, 2, LINE_8, hierarchy, 0);

	/*cout << img_resized.size() << endl;
	cout << img_gray.size() << endl;
	cout << edged.size() << endl;*/

	cout << rng.uniform(0, 256) << endl;

	vector <Point2i> approx;
	approxPolyDP(contours[index], approx, 30, true);
	auto mean = cv::mean(approx);
	vector<Point2i>


	imshow("origin", img_resized);
	imshow("gray and edged", hstack);
	waitKey(0);
	destroyAllWindows();
}

bool Contour_Area(vector<Point> contour1, vector<Point> contour2)
{
	return contourArea(contour1) > contourArea(contour2);
}