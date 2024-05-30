#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

class Matrix{
    double* data{};
    int rows{}, cols{};
public:
    ~Matrix()
    {
        if (data != nullptr)
        {
            delete[] data;
            data = nullptr;
        }
    }
    Matrix(int rows, int cols): rows(rows), cols(cols){
        this->~Matrix();
        data = new double[rows*cols];
    }
    Matrix(const Matrix& other): rows(other.rows), cols(other.cols){
        this->~Matrix();
        data = new double[rows*cols];
        for(int i = 0; i < rows*cols; i++){
            data[i] = other.data[i];
        }
    }
    Matrix& operator=(const Matrix& other){
        if(this == &other){
            return *this;
        }
        this->~Matrix();
        rows = other.rows;
        cols = other.cols;
        data = new double[rows*cols];
        for(int i = 0; i < rows*cols; i++){
            data[i] = other.data[i];
        }
        return *this;
    }
    Matrix(Matrix&& other) noexcept: rows(other.rows), cols(other.cols), data(other.data){
        other.rows = 0;
        other.cols = 0;
        other.data = nullptr;
    }
    Matrix& operator=(Matrix&& other) noexcept{
        if(this == &other){
            return *this;
        }
        this->~Matrix();
        rows = other.rows;
        cols = other.cols;
        data = other.data;
        other.rows = 0;
        other.cols = 0;
        other.data = nullptr;
        return *this;
    }
    double& operator()(int i, int j){
        return data[i*cols + j];
    }
    double& operator()(int i, int j) const{
        return data[i*cols + j];
    }
    double& operator[](int i){
        return data[i];
    }
    double& operator[](int i) const{
        return data[i];
    }
    inline int r(){
        return rows;
    }
    inline int c()
    {
        return cols;
    }
    inline int r() const
    {
        return rows;
    }
    inline int c() const
    {
        return cols;
    }
    Matrix operator*(const Matrix& other){
        if(cols != other.rows){
            throw std::runtime_error("Matrix multiplication: invalid dimensions");
        }
        Matrix result(rows, other.cols);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < other.cols; j++){
                result(i, j) = 0;
                for(int k = 0; k < cols; k++){
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }
    Matrix operator+(const Matrix& other){
        if(rows != other.rows || cols != other.cols){
            throw std::runtime_error("Matrix addition: invalid dimensions");
        }
        Matrix result(rows, cols);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                result(i, j) = (*this)(i, j) + other(i, j);
            }
        }
        return result;
    }
    Matrix operator-(const Matrix& other){
        if(rows != other.rows || cols != other.cols){
            throw std::runtime_error("Matrix subtraction: invalid dimensions");
        }
        Matrix result(rows, cols);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                result(i, j) = (*this)(i, j) - other(i, j);
            }
        }
        return result;
    }
    Matrix operator*(double scalar){
        Matrix result(rows, cols);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                result(i, j) = (*this)(i, j) * scalar;
            }
        }
        return result;
    }
    Matrix operator/(double scalar){
        Matrix result(rows, cols);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                result(i, j) = (*this)(i, j) / scalar;
            }
        }
        return result;
    }
    Matrix determinant(){
        if(rows != cols){
            throw std::runtime_error("Matrix determinant: invalid dimensions");
        }
        if(rows == 1){
            return *this;
        }
        if(rows == 2){
            Matrix result(1, 1);
            result(0, 0) = (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
            return result;
        }
        Matrix result(1, 1);
        for(int i = 0; i < rows; i++){
            Matrix submatrix(rows - 1, cols - 1);
            for(int j = 1; j < rows; j++){
                for(int k = 0; k < cols; k++){
                    if(k < i){
                        submatrix(j - 1, k) = (*this)(j, k);
                    }else if(k > i){
                        submatrix(j - 1, k - 1) = (*this)(j, k);
                    }
                }
            }
            Matrix subdeterminant = submatrix.determinant();
            result(0, 0) += (*this)(0, i) * (i % 2 == 0 ? 1 : -1) * subdeterminant(0, 0);
        }
        return result;
    }
    Matrix transpose(){
        Matrix result(cols, rows);
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

};

class ColorMatrix{
public:
    Matrix red, green, blue;
    ColorMatrix(int rows, int cols): red(rows, cols), green(rows, cols), blue(rows, cols){}
    Matrix& operator()(int channel){
        switch(channel){
            case 0: return red;
            case 1: return green;
            case 2: return blue;
            default: throw std::runtime_error("ColorMatrix: invalid channel");
        }
    }
    ColorMatrix(const ColorMatrix &other) : red(other.red), green(other.green), blue(other.blue){}

    ColorMatrix& operator=(const ColorMatrix& other){
        if(this == &other){
            return *this;
        }
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    ColorMatrix(ColorMatrix&& other) noexcept: red(std::move(other.red)), green(std::move(other.green)), blue(std::move(other.blue)){}
    ColorMatrix& operator=(ColorMatrix&& other) noexcept{
        if(this == &other){
            return *this;
        }
        red = std::move(other.red);
        green = std::move(other.green);
        blue = std::move(other.blue);
        return *this;
    }

    ColorMatrix operator*(double scalar){
        ColorMatrix result(red.r(), red.c());
        result.red = red * scalar;
        result.green = green * scalar;
        result.blue = blue * scalar;
        return result;
    }
    ColorMatrix operator/(double scalar){
        ColorMatrix result(red.r(), red.c());
        result.red = red / scalar;
        result.green = green / scalar;
        result.blue = blue / scalar;
        return result;
    }
    ColorMatrix operator+(const ColorMatrix& other){
        if(red.r() != other.red.r() || red.c() != other.red.c()){
            throw std::runtime_error("ColorMatrix addition: invalid dimensions");
        }
        ColorMatrix result(red.r(), red.c());
        result.red = red + other.red;
        result.green = green + other.green;
        result.blue = blue + other.blue;
        return result;
    }
    ColorMatrix operator-(const ColorMatrix& other){
        if(red.r() != other.red.r() || red.c() != other.red.c()){
            throw std::runtime_error("ColorMatrix subtraction: invalid dimensions");
        }
        ColorMatrix result(red.r(), red.c());
        result.red = red - other.red;
        result.green = green - other.green;
        result.blue = blue - other.blue;
        return result;
    }
    ColorMatrix operator*(const ColorMatrix& other){
        if(red.r() != other.red.r() || red.c() != other.red.c()){
            throw std::runtime_error("ColorMatrix multiplication: invalid dimensions");
        }
        ColorMatrix result(red.r(), red.c());
        result.red = red * other.red;
        result.green = green * other.green;
        result.blue = blue * other.blue;
        return result;
    }

};

ColorMatrix from_cvMat_to_ColorMatrix(cv::Mat img){
    ColorMatrix result(img.rows, img.cols);
    for(int i = 0; i < img.rows; i++){
        for(int j = 0; j < img.cols; j++){
            cv::Vec3b pixel = img.at<cv::Vec3b>(i, j);
            result.red(i, j) = pixel[2];
            result.green(i, j) = pixel[1];
            result.blue(i, j) = pixel[0];
        }
    }
    return result;
}

cv::Mat from_ColorMatrix_to_cvMat(ColorMatrix m){
    auto clamp = [](double x, double y, double z) -> double
    {
        return x > z ? z : (x < y ? y : x);
    };
    cv::Mat result(m.red.r(), m.red.c(), CV_8UC3);
    for(int i = 0; i < m.red.r(); i++){
        for(int j = 0; j < m.red.c(); j++){
            cv::Vec3b pixel;
            pixel[2] = clamp(m.red(i, j), 0, 255);
            pixel[1] = clamp(m.green(i, j), 0, 255);
            pixel[0] = clamp(m.blue(i, j), 0, 255);
            result.at<cv::Vec3b>(i, j) = pixel;
        }
    }
    return result;
}