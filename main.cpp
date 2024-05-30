#include "common.hpp"
#include "transform.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

int S = 3; // 注意，S现在是一个int，因为createTrackbar需要一个int参数
cv::Mat img;
std::vector<ColorMatrix> series;
double filter(double x, double s)
{
    return abs(x) > s * sqrt(S * 0.1) ? x : 0;
}
void on_trackbar(int, void *)
{
    // 这个函数将在滑块的值改变时被调用
    // 你可以在这里添加你的代码
    std::vector<ColorMatrix> tmp = series;
    size_t i = 0;
    for (auto &x : tmp)
    {
        i++;
        x = Transformer::Filter(filter, x, i);
    }
    ColorMatrix re = Transformer::Reconstruct(tmp);
    // 在同一个窗口显示series中的所有图像
    cv::Mat gather;
    int imgPerRow = 5; // 每行显示的图像数量
    std::vector<cv::Mat> row;
    const int size = 256;
    cv::Mat blank = cv::Mat::zeros(cv::Size(size, size), CV_8UC3); // 创建一个空白图像

    tmp.push_back(from_cvMat_to_ColorMatrix(img));
    tmp.push_back(re);

    for (int i = 0; i < tmp.size(); ++i)
    {
        cv::Mat tmp_ = from_ColorMatrix_to_cvMat(tmp[i]);
        if (!tmp_.empty())
        {
            cv::resize(tmp_, tmp_, cv::Size(size, size));
            row.push_back(tmp_);
            if ((i + 1) % imgPerRow == 0 || i == tmp.size() - 1) // 每行达到指定数量的图像或者已经是最后一个图像，就将这一行添加到gather中
            {
                while (row.size() < imgPerRow) // 如果这一行的图像数量小于指定的数量，就添加空白图像
                    row.push_back(blank);
                cv::Mat row_;
                cv::hconcat(row, row_); // 使用cv::hconcat函数来在矩阵的右边添加新的列
                if (gather.empty())
                    gather = row_;
                else
                    cv::vconcat(gather, row_, gather); // 使用cv::vconcat函数来在矩阵的底部添加新的行
                row.clear();
            }
        }
    }
    if (!gather.empty())
        cv::imshow("Series", gather);

    //cv::imshow("Reconstructed", from_ColorMatrix_to_cvMat(re));
}

size_t count_level_of_image(size_t size)
{
    size_t i = 1;
    size_t j = 0;
    while (i < size)
    {
        i <<= 1;
        j++;
    }
    return j;
}

int main()
{
    try
    {
        img = cv::imread("test.png");
        cv::imshow("Series", img);
        cv::createTrackbar("Parameter", "Series", &S, 100, on_trackbar); // 创建滑块

        ColorMatrix m = from_cvMat_to_ColorMatrix(img);
        series = Transformer::Series(Blur::SqureBlur, m, count_level_of_image(m.red.c()));
        on_trackbar(S, 0); // 初始化显示

        while (cv::waitKey(0)!=27);
    }
    catch (cv::Exception &e)
    {
        std::cerr << e.msg << std::endl;
    }
    return 0;
}