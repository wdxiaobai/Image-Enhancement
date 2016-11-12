#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
template <typename T>
cv::Mat plotGraph(std::vector<T>& vals, int YRange[2])
{

    auto it = minmax_element(vals.begin(), vals.end());
    float scale = 1./ceil(*it.second - *it.first); 
    float bias = *it.first;
    int rows = YRange[1] - YRange[0] + 1;
    cv::Mat image = Mat::zeros( rows, vals.size(), CV_8UC3 );
    image.setTo(0);
    for (int i = 0; i < (int)vals.size()-1; i++)
    {
        cv::line(image, cv::Point(i, rows - 1 - (vals[i] - bias)*scale*YRange[1]), cv::Point(i+1, rows - 1 - (vals[i+1] - bias)*scale*YRange[1]), Scalar(255, 0, 0), 1);
    }

    return image;
}

void myAdd_weight(Mat& input1, Mat& input2, Mat& output, double weight){
	/*int max[3] = {0,0,0};
	int min[3] = {255,255,255};

	for(int row = 0; row < output.rows; row++)
		for(int col = 0; col < output.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++)
				if(input1.at<Vec3b>(row, col).val[pixel]+input2.at<Vec3b>(row, col).val[pixel]*weight > max[pixel])
					max[pixel] = input1.at<Vec3b>(row, col).val[pixel] 
								+ input2.at<Vec3b>(row, col).val[pixel]*weight;

	for(int row = 0; row < output.rows; row++)
		for(int col = 0; col < output.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++){
				output.at<Vec3b>(row, col).val[pixel] = (input1.at<Vec3b>(row, col).val[pixel] 
												+ input2.at<Vec3b>(row, col).val[pixel]*weight)*255.0/max[pixel];
				// output.at<Vec3b>(row, col).val[pixel] = (input1.at<Vec3b>(row, col).val[pixel] 
								// + input2.at<Vec3b>(row, col).val[pixel]*0.1);
			}*/
	for(int row = 0; row < output.rows; row++)
		for(int col = 0; col < output.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++){
				int temp = input1.at<Vec3b>(row, col).val[pixel] 
								+ input2.at<Vec3b>(row, col).val[pixel]*weight;
				if(temp > 255)
					output.at<Vec3b>(row, col).val[pixel] = 255;
				else
					output.at<Vec3b>(row, col).val[pixel] = temp;
				// output.at<Vec3b>(row, col).val[pixel] = (input1.at<Vec3b>(row, col).val[pixel] 
								// + input2.at<Vec3b>(row, col).val[pixel]*0.1);
			}

}

void powerTransform(Mat& img, double gamma){
	double histogram[256];
	for(int i = 0; i < 256; i++)
		histogram[i] = pow(i,gamma);
	double max = histogram[255];
	double min = histogram[0];
	double delta = max - min;
	for(int i = 0; i < 256; i++)
		histogram[i] = histogram[i]*255.0/delta;


	for(int row = 0; row < img.rows; row++)
		for(int col = 0; col < img.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++)
				img.at<Vec3b>(row, col).val[pixel] = histogram[img.at<Vec3b>(row, col).val[pixel]];

	vector<int> numbers(256);	
	for(int i = 0; i < 256; i++)
		numbers[i] = histogram[i];
	int range[2] = {0, 256};

    Mat lineGraph = plotGraph(numbers, range);
	imshow("plot", lineGraph);
}

Mat whiteBalance(Mat img){
/*
    vector<vector<int> > numbers;
    numbers.resize(3, vector<int>(256,0));*/
	// iota (numbers.begin(), numbers.end(), 0);


	Mat WB_img = img.clone();
	double* histogram;
	double* equalHistorgram;
	double normalizeFactor = img.rows * img.cols;
	for(int i = 0; i < 3; i++){
		histogram = new double[256];
		// equalHistorgram = new double[256];

		for(int index = 0; index < 256; index++)
			histogram[index] = 0;

		for(int row = 0; row < img.rows; row++)
			for(int col = 0; col < img.cols; col++)
				histogram[img.at<Vec3b>(row, col).val[i]]++;

		for(int index = 1; index < 256; index++){
			histogram[index] = histogram[index] + histogram[index-1];
		}

		/* Obtain the max and min of streching boundary based on the histogram*/
		double discard_ratio = 0.05;
		int min = 0;
		int max = 255;
		while(histogram[min] < discard_ratio*normalizeFactor)
			min += 1;
		while(histogram[max] > (1.0-discard_ratio)*normalizeFactor)
			max -= 1;

		if(max < 254)
			max += 1;


		for(int row = 0; row < img.rows; row++)
			for(int col = 0; col < img.cols; col++){
				uchar val = img.at<Vec3b>(row, col).val[i];
				if(val < min)
					val = min;
				if(val > max)
					val = max;
				/* 1 to 1 Mapping of histogram */
				WB_img.at<Vec3b>(row, col).val[i] = histogram[img.at<Vec3b>(row, col).val[i]]/normalizeFactor*255;
				
				/* Streching based on the Histogram */
				// WB_img.at<Vec3b>(row, col).val[i] = (uchar)(val - min)*255 / (max - min);

			}
		}
/*
	double* histogram_2 = new double[256];
	// equalHistorgram = new double[256];

	for(int index = 0; index < 256; index++)
		histogram_2[index] = 0;

	for(int row = 0; row < img.rows; row++)
		for(int col = 0; col < img.cols; col++)
			histogram_2[WB_img.at<Vec3b>(row, col).val[i]]++;

	for(int index = 1; index < 256; index++){
		histogram_2[index] = histogram_2[index] + histogram_2[index-1];
	}
	for(int z = 0; z < numbers[i].size(); z++)
		numbers[i][z] = histogram_2[z];
	}

	int range[2] = {0, 256};
	cv::Mat lineGraph1 = plotGraph(numbers[0], range);
	cv::Mat lineGraph2 = plotGraph(numbers[1], range);
	cv::Mat lineGraph3 = plotGraph(numbers[2], range);
	imshow("plot1", lineGraph1);
	imshow("plot2", lineGraph2);
	imshow("plot3", lineGraph3);
	waitKey(0);*/
	return WB_img;
}

void myHighBoost(Mat& input, Mat& output, double boost){

    vector<vector< double> > boost_kernel;
    boost_kernel.resize(3, vector<double>(3,0));
    boost_kernel[0][0] = -1;
    boost_kernel[0][1] = -1;
    boost_kernel[0][2] = -1;
    boost_kernel[1][0] = -1;
    boost_kernel[1][1] = 8 + boost;
    boost_kernel[1][2] = -1;
    boost_kernel[2][0] = -1;
    boost_kernel[2][1] = -1;
    boost_kernel[2][2] = -1;
    int min[3] = {255,255,255};
    int max[3] = {0,0,0};

    for(int row = 0; row < output.rows; row++)
        for(int col = 0; col < output.cols; col++){
            for(int pixel = 0; pixel < 3; pixel++){
                int sum = 0;
                for(int kernel_x = -1; kernel_x <= 1; kernel_x++){
                    for(int kernel_y = -1; kernel_y <= 1; kernel_y++){
                        int currentCol = col + kernel_x;
                        int currentRow = row + kernel_y;
                        if(currentCol<0 || currentRow<0 || currentCol==output.cols || currentRow==output.rows)
                            sum += input.at<Vec3b>(row, col).val[pixel] * boost_kernel[kernel_y+1][kernel_x+1];
                        else
                            sum += input.at<Vec3b>(currentRow, currentCol).val[pixel] * boost_kernel[kernel_y+1][kernel_x+1];
                    }

                }
                if(sum < min[pixel])
                	min[pixel] = sum;
                if(sum > max[pixel])
                	max[pixel] = sum;
            }
        }
    for(int row = 0; row < output.rows; row++)
        for(int col = 0; col < output.cols; col++){
            for(int pixel = 0; pixel < 3; pixel++){
                int sum = 0;
                for(int kernel_x = -1; kernel_x <= 1; kernel_x++){
                    for(int kernel_y = -1; kernel_y <= 1; kernel_y++){
                        int currentCol = col + kernel_x;
                        int currentRow = row + kernel_y;
                        if(currentCol<0 || currentRow<0 || currentCol==output.cols || currentRow==output.rows)
                            sum += input.at<Vec3b>(row, col).val[pixel] * boost_kernel[kernel_y+1][kernel_x+1];
                        else
                            sum += input.at<Vec3b>(currentRow, currentCol).val[pixel] * boost_kernel[kernel_y+1][kernel_x+1];
                    }

                }
                output.at<Vec3b>(row, col).val[pixel] = (sum - min[pixel])*255.0/(max[pixel] - min[pixel]);
            }
        } 
}

void normalize(Mat& add){
	int max[3] = {0,0,0};
	for(int row = 0; row < add.rows; row++)
		for(int col = 0; col < add.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++)
				if(add.at<Vec3b>(row, col).val[pixel] > max[pixel])
					max[pixel] = add.at<Vec3b>(row, col).val[pixel];


	for(int row = 0; row < add.rows; row++)
		for(int col = 0; col < add.cols; col++)
			for(int pixel = 0; pixel < 3; pixel++)
				add.at<Vec3b>(row, col).val[pixel] = add.at<Vec3b>(row, col).val[pixel]*255/max[pixel];
}

void myGaussianBlur(Mat& input, Mat& output){

    vector<vector< double> > g_kernel;
    g_kernel.resize(3, vector<double>(3,0));
    g_kernel[0][0] = 1.0/16;
    g_kernel[0][1] = 1.0/8;
    g_kernel[0][2] = 1.0/16;
    g_kernel[1][0] = 1.0/8;
    g_kernel[1][1] = 1.0/4;
    g_kernel[1][2] = 1.0/8;
    g_kernel[2][0] = 1.0/16;
    g_kernel[2][1] = 1.0/8;
    g_kernel[2][2] = 1.0/16;

    for(int row = 0; row < output.rows; row++)
        for(int col = 0; col < output.cols; col++){
            for(int pixel = 0; pixel < 3; pixel++){
                int sum = 0;
                for(int kernel_x = -1; kernel_x <= 1; kernel_x++){
                    for(int kernel_y = -1; kernel_y <= 1; kernel_y++){
                        int currentCol = col + kernel_x;
                        int currentRow = row + kernel_y;
                        if(currentCol<0 || currentRow<0 || currentCol==output.cols || currentRow==output.rows)
                            sum += input.at<Vec3b>(row, col).val[pixel] * g_kernel[kernel_y+1][kernel_x+1];
                        else
                            sum += input.at<Vec3b>(currentRow, currentCol).val[pixel] * g_kernel[kernel_y+1][kernel_x+1];
                    }

                }
                output.at<Vec3b>(row, col).val[pixel] = sum;
            }
        } 
}

int main(char argc, char ** argv){

	string inputFile = argv[1];
	string fileNum;

	double prePow, laplacianPow, weight, boost;
	if(inputFile[0]=='i' && inputFile[1]=='n' && inputFile[2]=='p' && inputFile[3]=='u' 
		&& inputFile[4]=='t'){
		fileNum = inputFile[5];
		switch(stoi(fileNum)){
			case 1:{
				prePow = 0.8;
				laplacianPow = 5;
				weight = 1;
				boost = 0;
				break;
			}
			case 2:{
				prePow = 0.5;
				laplacianPow = 5;
				weight = 0.8;
				boost = 1;
				break;
			}
			case 3:{
				prePow = 0.5;
				laplacianPow = 3;
				weight = 1;
				boost = 0;
				break;
			}
			case 4:{
				prePow = 0.5;
				laplacianPow = 3;
				weight = 0.8;
				boost = 0;
				break;
			}
		}
	}
	else{
		fileNum = "";
		prePow = 0.5;
		laplacianPow = 3;
		weight = 0.8;
		boost = 0;
	}

    Mat src = imread(inputFile);
    imshow("ori", src);

    Mat add = src.clone();
    Mat gaussian = src.clone();
    Mat laplacian = src.clone();
    Mat gaussian2 = src.clone();

    // Power to enhance darker image
    powerTransform(src, prePow);
    // imwrite("pow.jpg", src);

    // Gaussian to remove noise
    myGaussianBlur(src, gaussian);
    // imwrite("gaussian.jpg", gaussian);

    // imshow("pow + gaussian", gaussian);

    // Gaussian again to further remove noise
    myGaussianBlur(gaussian, gaussian2);

    // Laplacian or High Boost to enhance detail
    myHighBoost(gaussian2, laplacian, boost);
    // imwrite("laplacian.jpg", laplacian);

    // Increase contrast of lapacian
    powerTransform(laplacian, laplacianPow);
    // imwrite("laplacian_pow.jpg", laplacian);

    // imshow("laplacian", laplacian);

    // Add laplacian result to original image
    myAdd_weight(gaussian, laplacian, add, weight);
    imshow("Output", add);

    string outputFile = "output" + fileNum + ".bmp";
    imwrite(outputFile, add);
    waitKey(0);
    return 0;
}
