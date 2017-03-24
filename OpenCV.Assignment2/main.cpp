#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <memory>
#include <iostream>

using namespace cv;
using namespace std;

class MeanShiftSegmentation {
public:
	MeanShiftSegmentation()
	{
	}
	~MeanShiftSegmentation() = default;

	void DisplayMeanShift(string wName, Mat& img)
	{
		originalImage = img;
		windowName = wName;
		namedWindow(windowName, CV_WINDOW_NORMAL);


		createTrackbar("proximity", windowName, &proximtiyRadius, 80, trackbarCallback);
		createTrackbar("color radius", windowName, &colorRadius, 60, trackbarCallback);
		createTrackbar("level", windowName, &maxPyramidLevel, 5, trackbarCallback);

		trackbarCallback(0, 0);
	}

private:
	static void trackbarCallback(int, void*)
	{
		Mat result;
		pyrMeanShiftFiltering(originalImage, result, proximtiyRadius, colorRadius, maxPyramidLevel);
		fillSegments(result, Scalar::all(2));
		imshow(windowName, result);
	}

	//FillSegments
	static void fillSegments(Mat& img, const Scalar& colorDiff = Scalar::all(1))
	{
		CV_Assert(!img.empty());
		RNG random = theRNG();
		Mat mask(img.rows + 2, img.cols + 2, CV_8UC1, Scalar::all(0));
		for (int y = 0; y < img.rows; y++)
		{
			for (int x = 0; x < img.cols; x++)
			{
				if (mask.at<uchar>(y + 1, x + 1) == 0)
				{
					Scalar newColor(random(256), random(256), random(256));
					floodFill(img, mask, Point(x, y), newColor, 0, colorDiff, colorDiff);
				}
			}
		}
	}

	static int proximtiyRadius;
	static int colorRadius;
	static int maxPyramidLevel;
	static Mat originalImage;
	static string windowName;
};

 int MeanShiftSegmentation::proximtiyRadius = 30;
 int MeanShiftSegmentation::colorRadius = 30;
 int MeanShiftSegmentation::maxPyramidLevel = 1;
 Mat MeanShiftSegmentation::originalImage = Mat();
 string MeanShiftSegmentation::windowName = "Default";

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Require image as input";
		return -1;
	}

	Mat img = imread(argv[1]);
	if (img.empty())
		return -1;
	else {
		namedWindow("Original Picture", CV_WINDOW_NORMAL);
		imshow("Original Picture", img);
	}
	
	//preprocessing

	//blur
	Mat blur;
	GaussianBlur(img, blur,Size(5,5),0,0);
	namedWindow("Blurred Picture", CV_WINDOW_NORMAL);
	imshow("Blurred Picture", blur);

	////sharpen
	//Mat sharpen;
	//Laplacian(img, sharpen, CV_8UC1);
	//namedWindow("Sharpen Picture", CV_WINDOW_NORMAL);
	//imshow("Sharpen Picture", sharpen);

	////Laplacian of Gausian 
	//Mat LoG;
	//Laplacian(blur, LoG, CV_8UC1);
	//namedWindow("Sharpen Picture", CV_WINDOW_NORMAL);
	//imshow("Lap of Gauss", LoG);

	string windowName = "Mean Shift";
	shared_ptr<MeanShiftSegmentation> mss = make_shared<MeanShiftSegmentation>();

	mss->DisplayMeanShift(windowName, /*LoG*/ blur /*sharpen*/ /*img*/);

	waitKey();
	return 0;
}

