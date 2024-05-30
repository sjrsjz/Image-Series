#pragma once
#include "common.hpp"
#include <functional>
#include <vector>


namespace Blur{
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

    auto SqureBlur = [](ColorMatrix m, size_t level) -> ColorMatrix
    {
        // 假设m是一个正方形矩阵，边长是2^n
        // level是模糊边长为2^level的正方形区域，并且是分块模糊
        // 例如，level=0表示模糊边长为2^0=1的正方形区域，即一个像素点
        // level=1表示模糊边长为2^1=2的正方形区域，即4个像素点，以此类推
        if(m.red.r() != m.red.c() || m.red.r() != m.green.r() || m.red.r() != m.blue.r())
        {
            throw std::runtime_error("Blur: invalid dimensions");
        }
        if((m.red.r() & (m.red.r() - 1)) != 0)
        {
            throw std::runtime_error("Blur: invalid dimensions");
        }
        if (level < 0 || level > count_level_of_image(m.red.r()))
        {
            throw std::runtime_error("Blur: invalid level");
        }
        ColorMatrix result(m.red.r(), m.red.c());
        size_t n = m.red.r();
        size_t step = 1 << level;
        for(size_t i = 0; i < n; i += step)
        {
            for(size_t j = 0; j < n; j += step)
            {
                double sum_red = 0, sum_green = 0, sum_blue = 0;
                for(size_t x = 0; x < step; x++)
                {
                    for(size_t y = 0; y < step; y++)
                    {
                        sum_red += m.red(i + x, j + y);
                        sum_green += m.green(i + x, j + y);
                        sum_blue += m.blue(i + x, j + y);
                    }
                }
                double avg_red = sum_red / (step * step);
                double avg_green = sum_green / (step * step);
                double avg_blue = sum_blue / (step * step);
                for(size_t x = 0; x < step; x++)
                {
                    for(size_t y = 0; y < step; y++)
                    {
                        result.red(i + x, j + y) = avg_red;
                        result.green(i + x, j + y) = avg_green;
                        result.blue(i + x, j + y) = avg_blue;
                    }
                }
            }
        }
        return result;
    };
}

namespace Transformer{
    auto X_minus_BlurX = [](std::function<ColorMatrix(ColorMatrix, size_t)> f, ColorMatrix m, size_t level) -> ColorMatrix
    {
        ColorMatrix blurred = f(m, level);
        return m - blurred;
    };
    auto Series = [](std::function<ColorMatrix(ColorMatrix, size_t)> f, ColorMatrix m, size_t max_level) -> std::vector<ColorMatrix>
    {
        std::vector<ColorMatrix> result;
        ColorMatrix X = m;
        max_level++;
        while(max_level--)
        {
            ColorMatrix blurred = f(X, max_level);
            result.push_back(blurred);
            X = X - blurred;
        }
        result.push_back(X);
        return result;
    };
    auto Reconstruct = [](std::vector<ColorMatrix> series) -> ColorMatrix
    {
        if(series.size()==0){
            return ColorMatrix(64, 64);
        }
        ColorMatrix result = series.back();
        for(int i = series.size() - 2; i >= 0; i--)
        {
            result = result + series[i];
        }
        return result;
    };
    auto Filter = [](std::function<double(double,double)> f, ColorMatrix m, double s) -> ColorMatrix
    {
        ColorMatrix result(m.red.r(), m.red.c());
        for (size_t i{}; i < m.red.r();i++)
        {
            for (size_t j{}; j < m.red.c(); j++)
            {
                result(0)(i, j) = f(m(0)(i, j), s);
                result(1)(i, j) = f(m(1)(i, j), s);
                result(2)(i, j) = f(m(2)(i, j), s);
            }
        }
        return result;
    };
}