#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>


using namespace std;

//������ �Ұųĸ�
//1.���� �̹���, �׷��̽�����, ���� 3������ �̿�
//2.�ش� �̹����鿡 ������ ����
//3.�����װ��� �ջ��Ͽ� �̹��� �׵θ� ����.

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

	cout << "�۾��� �̹����� ��θ� �Է����ּ���." << endl;
	cout << "�߸��� ���/���� ������ ��� ���� ������ ����˴ϴ�." << endl;
	cin >> filename;


	img = cv::imread(filename, cv::IMREAD_COLOR);
	img_gray = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	if (img.empty())
	{
		std::cout << "[!] �߸��� ����Դϴ�. ���� ������ ����˴ϴ�." << std::endl;
		img = cv::imread("sample.jpg", cv::IMREAD_COLOR);
		img_gray = cv::imread("sample.jpg", cv::IMREAD_GRAYSCALE);

	}

	//��ġ �Է� �κ�
	int start;
	int end;
	int max;

	cout << "���ø����� ������ ��� ������ �Է����ּ���, start < x < end" << endl;
	cout << "default : 145~165" << endl;
	cin >> start >> end;

	cout << "���ø����� ������ �� �� (��� ����)�� �Է����ּ���. max > x" << endl;
	cout << "default : 235" << endl;

	cout << "(����Ʈ���� -1�Դϴ�)" << endl;

	cin >> max;
	cout << endl;


	float sigX;
	float sigY;
	float alpha;
	float beta;
	float gamma;

	cout << "����þ� ǥ�������� �Է����ּ���. SigmaX, SigmaY ��" << endl;
	cout << "default : 1.5, 1.5" << endl;
	cin >> sigX >> sigY;
	cout << endl;

	cout << "weight ����� �Է����ּ���" << endl;
	cout << "AnsImg = saturate(Base*alpha + Gaussian*beta + gamma" << endl;
	cout << "default = saturate(Base*1.5 + Gaussian*(-0.3) + 0" << endl;


	cin >> alpha >> beta >> gamma;
	cout << endl;

	//��ġ �Էºκ� ��

	cv::Mat answer = Sharpning(lineing(img_gray, start, end, max), sigX, sigY, alpha, beta, gamma);

	//--original


	cv::namedWindow("Original");
	cv::imshow("Original", img);
	cv::setMouseCallback("Original", onMouse, reinterpret_cast<void*>(&img));



	cv::namedWindow("Changed");
	//cv::imshow("Changed", lineing(img_gray));
	cv::imshow("Changed", answer);
	//lineing�� gray�� �־���� ��. RGB�� �и��� �̹��� �и�.




	//Lightning(img_gray);
	cv::waitKey(33);
	//cv::destroyWindow("EXAM");
	char select;
	cout << "�̹����� �����Ͻðڽ��ϱ�? Y/N" << endl;
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
			//B,G,R  ��
		}

	}

	return reversed;
}
void Lightning(cv::Mat image) {
	//��Ⱚ ���.
	//�ֺ� ���ø�
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
	//���̴�
	//�ֺ� ���� ä��
	int sampling = 1;	//n*n;
	int temp_phase = 1;
	int x_remain = image.size().width % sampling;
	int y_remain = image.size().height % sampling;
	int add = 0;

	if (start > end) {
		start = -1;
		end = -1;
	}
	if (start == -1) start = 145;		//��� ���� ���� ��������
	if (end == -1) end = 165;			//��� ���� ���� ���� ����
	if (max == -1) max = 235;			//��� ���� ���� '�� ��' ���� ����




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
