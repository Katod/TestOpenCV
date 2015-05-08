#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>

using namespace cv;
using std::cout;
using std::endl;

int main(int argc, char **argv){
    char mainWindow[] = "Main";
	char trackbarWindow[] = "Trackbar";
	char thresholdWindow[] = "Threshold";
	int min = 0, max = 1000;
	int hmin = 0, smin = 0, vmin = 0,
		hmax = 255, smax = 255, vmax = 255;
    Mat frame, HSV, threshold, blurred;
    VideoCapture capture;
	const int FRAME_BUFF_SIZE = 5;
	const int MOVE =80;
    //Создаем окна
    namedWindow(mainWindow, 0);
	namedWindow(trackbarWindow, 0);
	namedWindow(thresholdWindow, 0);

	createTrackbar("H min:", trackbarWindow, &hmin, hmax);
	createTrackbar("H max:", trackbarWindow, &hmax, hmax);
	createTrackbar("S min:", trackbarWindow, &smin, smax);
	createTrackbar("S max:", trackbarWindow, &smax, smax);
	createTrackbar("V min:", trackbarWindow, &vmin, vmax);
	createTrackbar("V max:", trackbarWindow, &vmax, vmax);
	createTrackbar("Size min:", trackbarWindow, &min, max);
	createTrackbar("Size max:", trackbarWindow, &max, max);

    //Открываем камеру
    capture.open(0);
    if(!capture.isOpened()){
        cout << "Камера не может быть открыта." << endl;
        exit(1);
    }
    
	capture >> frame;
	Rect buff_rect[FRAME_BUFF_SIZE];
	Rect rect;
	int current_rect=0;
	bool start_calc = false;
    //Запускаем цикл чтения с камеры
	while(true){
		capture >> frame;
		cvtColor(frame, HSV, COLOR_BGR2HSV);
		medianBlur(HSV, blurred, 21);
		inRange(blurred, Scalar(hmin, smin, vmin), Scalar(hmax, smax, vmax), threshold);
		for (int y = 0; y < threshold.rows; y++){
			for (int x = 0; x < threshold.cols; x++){
				int value = threshold.at<uchar>(y, x);
				if (value == 255){
					int count = floodFill(threshold, Point(x, y), Scalar(200), &rect);
					//std::cout << count;
					if (rect.width >= min && rect.width <= max
						&& rect.height >= min && rect.height <= max){
						rectangle(frame, rect, Scalar(255, 0, 255, 4));
						if (rect.x != 0 || rect.y != 0)
						{
							buff_rect[current_rect++] = rect;
							int delta_y = 0;
							int delta_x = 0;
							if (start_calc)
							{
								for (int i = 0; i < FRAME_BUFF_SIZE - 1; i++)
								{
									delta_x += buff_rect[i].x - buff_rect[i + 1].x;
									delta_y += buff_rect[i].y - buff_rect[i + 1].y;
								}
									
								if (abs(delta_x) > abs(delta_y))
								{
									if (delta_x >= MOVE)
									{
										cout << "<<UP>>\n";
									}
									if (delta_x <= -MOVE)
									{
										cout << "<<DOWN>>\n";
									}
								}
								else
								{
									if (delta_y >= MOVE)
									{
										cout << "<<LEFT>>\n";
									}
									if (delta_y <= -MOVE)
									{
										cout << "<<RIGHT>>\n";
										
									}
								}
								delta_x = 0;
								delta_y = 0;
								std::memset(buff_rect, 0, sizeof(Rect)*FRAME_BUFF_SIZE);
							}
							if (current_rect >= FRAME_BUFF_SIZE)
							{
								start_calc = true;
								current_rect = 0;
							}
						}
						//cout << rect.x <<"	"<< rect.y<<std::endl;
						}
				}
			}
		}
		imshow(mainWindow, frame);
		imshow(thresholdWindow, threshold);
		if (waitKey(33) == 27) break;
    }
	
    return 0;
}