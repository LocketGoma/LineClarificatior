#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>


using namespace std;

//뭔짓을 할거냐면
//1.원본 이미지, 그레이스케일, 반전 3가지를 이용
//2.해당 이미지들에 샤프닝 적용
//3.샤프닝값을 합산하여 이미지 테두리 떼기.

void onMouse(int event, int x, int y, int flags, void* param);
cv::Mat ReverseColor(cv::Mat image);
void Lightning(cv::Mat image);
cv::Mat lineing(cv::Mat image, int start, int end, int max);
cv::Mat Sharpning(cv::Mat image, float, float, float, float, float);

int main()
{

	std::cout << "OpenCV Version : " << CV_VERSION << endl;
	cv::Mat img;
	cv::Mat img_gray;
	string filename = "sample.jpg";

	cout << "작업할 이미지의 경로를 입력해주세요." << endl;
	cout << "잘못된 경로/없는 파일일 경우 샘플 파일이 실행됩니다." << endl;
	cin >> filename;


	img = cv::imread(filename, cv::IMREAD_COLOR);
	img_gray = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	if (img.empty())
	{
		std::cout << "[!] 잘못된 경로입니다. 샘플 파일이 실행됩니다." << std::endl;
		img = cv::imread("sample.jpg", cv::IMREAD_COLOR);
		img_gray = cv::imread("sample.jpg", cv::IMREAD_GRAYSCALE);

	}

	//수치 입력 부분
	int start;
	int end;
	int max;

	cout << "샘플링에서 제거할 밝기 영역을 입력해주세요, start < x < end" << endl;
	cout << "default : 145~165" << endl;
	cin >> start >> end;

	cout << "샘플링에서 제거할 잔 선 (밝기 영역)을 입력해주세요. max > x" << endl;
	cout << "default : 235" << endl;

	cout << "(디폴트값은 -1입니다)" << endl;

	cin >> max;
	cout << endl;


	float sigX;
	float sigY;
	float alpha;
	float beta;
	float gamma;

	cout << "가우시안 표준편차를 입력해주세요. SigmaX, SigmaY 순" << endl;
	cout << "default : 1.5, 1.5" << endl;
	cin >> sigX >> sigY;
	cout << endl;

	cout << "weight 상수를 입력해주세요" << endl;
	cout << "AnsImg = saturate(Base*alpha + Gaussian*beta + gamma" << endl;
	cout << "default = saturate(Base*1.5 + Gaussian*(-0.3) + 0" << endl;


	cin >> alpha >> beta >> gamma;
	cout << endl;

	//수치 입력부분 끝

	cv::Mat answer = Sharpning(lineing(img_gray, start, end, max), sigX, sigY, alpha, beta, gamma);

	//--original


	cv::namedWindow("Original");
	cv::imshow("Original", img);
	cv::setMouseCallback("Original", onMouse, reinterpret_cast<void*>(&img));



	cv::namedWindow("Changed");
	//cv::imshow("Changed", lineing(img_gray));
	cv::imshow("Changed", answer);
	//lineing은 gray로 넣어줘야 됨. RGB로 밀리면 이미지 밀림.




	//Lightning(img_gray);
	cv::waitKey(33);
	//cv::destroyWindow("EXAM");
	char select;
	cout << "이미지를 저장하시겠습니까? Y/N" << endl;
	cin >> select;
	if (select == 'Y' || select == 'y')
		cv::imwrite("answer.png", answer);

	system("pause");

	return 0;


}
void onMouse(int event, int x, int y, int flags, void* param) {
	cv::Mat* im = reinterpret_cast<cv::Mat*>(param);

	uchar r;
	uchar g;
	uchar b;


	switch (event) {
	case cv::EVENT_FLAG_LBUTTON: {
		r = im->at<cv::Vec3b>(y, x)[2];
		g = im->at<cv::Vec3b>(y, x)[1];
		b = im->at<cv::Vec3b>(y, x)[0];

		cout << "(" << x << "," << y << ")";
		//cout << static_cast<int>(im->at<uchar>(cv::Point(x, y)));
		cout << " // (" << (int)r << "," << (int)g << "," << (int)b << ")" << endl;
	}
	}
}
cv::Mat ReverseColor(cv::Mat image) {
	cv::Mat reversed;
	if (image.empty())
		return reversed;

	reversed = image;


	for (int j = 0; j < image.size().height; j++) {
		uchar* value = image.ptr<uchar>(j);
		uchar* result = reversed.ptr<uchar>(j);
		for (int i = 0; i < image.size().width; i++) {
			*result++ = *value++ ^ 0xff;
			*result++ = *value++ ^ 0xff;
			*result++ = *value++ ^ 0xff;
			//B,G,R  순
		}

	}

	return reversed;
}
void Lightning(cv::Mat image) {
	//밝기값 출력.
	//주변 샘플링
	int sampling = 7;	//n*n;
	int temp_phase = 0;
	int x_remain = image.size().width % sampling;
	int y_remain = image.size().height % sampling;
	int add = 0;


	for (int i = 0; i < image.size().height - y_remain; i = i + sampling) {
		for (int j = 0; j < image.size().width - x_remain; j = j + sampling) {
			//cout << (int)image.at<uchar>(i, j)<<" ";
			for (int k = 0; k < sampling; k++) {
				for (int y = 0; y < sampling; y++) {
					add = (int)image.at<uchar>(i + y, j + k);
					if (add != 255)
						temp_phase += add;
				}
			}

			if (temp_phase / (sampling * sampling) > 145 && temp_phase / (sampling * sampling) < 165)
				printf("%3d", 0);
			else
				printf("%3d", temp_phase / (sampling * sampling));


			temp_phase = 0;

		}
		cout << endl;
	}
}

cv::Mat lineing(cv::Mat image, int start, int end, int max) {
	//라이닝
	//주변 샘플 채취
	int sampling = 1;	//n*n;
	int temp_phase = 1;
	int x_remain = image.size().width % sampling;
	int y_remain = image.size().height % sampling;
	int add = 0;

	if (start > end) {
		start = -1;
		end = -1;
	}
	if (start == -1) start = 145;		//흑백 영역 제거 시작지점
	if (end == -1) end = 165;			//흑백 영역 제거 종료 지점
	if (max == -1) max = 235;			//흑백 영역 제거 '잔 선' 제거 범위




	//cv::Mat lined = cvCreateImage(cvSize(image.size().width, image.size().height), IPL_DEPTH_8U, 1);
	cv::Mat lined = image;

	//cout << "phase 2: " << image.size().width - x_remain << endl;
	for (int i = 0; i < image.size().height - y_remain; i = i + sampling) {
		for (int j = 0; j < image.size().width - x_remain; j = j + sampling) {
			//cout << (int)image.at<uchar>(i, j)<<" ";
			for (int k = 0; k < sampling; k++) {
				for (int y = 0; y < sampling; y++) {
					add = (int)image.at<uchar>(i + y, j + k);
					if (add != 255)
						temp_phase += add;
				}
			}

			if (temp_phase / (sampling * sampling) > start&& temp_phase / (sampling * sampling) < end || temp_phase / (sampling * sampling) > max)
				temp_phase = 0;
			else
				temp_phase = temp_phase / (sampling * sampling);

			//printf("%3d", temp_phase);


			//cout << temp_phase << ":" << (uchar)temp_phase;
			for (int k = 0; k < sampling; k++) {
				for (int y = 0; y < sampling; y++) {
					uchar* pt = lined.ptr<uchar>(i + y);

					pt[j + k] = temp_phase ^ 0xff;
					//lined.at<uchar>(i + y, j + k) = temp_phase^0xff;
				}
			}
			//cout << temp_phase << ":" << (uchar)temp_phase;

			temp_phase = 0;

		}
		//cout << endl;
	}
	return lined;
}
cv::Mat Sharpning(cv::Mat image, float sigX, float sigY, float alpha, float beta, float gamma) {



	cv::Mat sharped;
	sharped = image;

	cv::GaussianBlur(image, sharped, cv::Size(0, 0), sigX, sigY);
	cv::addWeighted(image, alpha, sharped, beta, gamma, image);


	cv::Mat sharpen_mask = (cv::Mat_<char>(3, 3) << -1, -2, -1,
		-2, 13, -2,
		-1, -2, -1);
	cv::filter2D(image, sharped, image.depth(), sharpen_mask);








	return sharped;
}
