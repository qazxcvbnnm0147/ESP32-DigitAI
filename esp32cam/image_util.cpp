
/*
  * ESPRESSIF MIT License
  *
  * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
  *
  * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
  * it is free of charge, to any person obtaining a copy of this software and associated
  * documentation files (the "Software"), to deal in the Software without restriction, including
  * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all copies or
  * substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "image_util.h"
#include "esp_timer.h"





void image_resize_linear(uint8_t *dst_image, uint8_t *src_image, int dst_w, int dst_h, int dst_c, int src_w, int src_h)
{ /*{{{*/
    float scale_x = (float)src_w / dst_w;
    float scale_y = (float)src_h / dst_h;

    int dst_stride = dst_c * dst_w;
    int src_stride = dst_c * src_w;


        for (int y = 0; y < dst_h; y++)
        {
            float fy[2];
            fy[0] = (float)((y + 0.5) * scale_y - 0.5); // y
            int src_y = (int)fy[0];                     // y1
            fy[0] -= src_y;                             // y - y1
            fy[1] = 1 - fy[0];                          // y2 - y
            src_y = DL_IMAGE_MAX(0, src_y);
            src_y = DL_IMAGE_MIN(src_y, src_h - 2);

            for (int x = 0; x < dst_w; x++)
            {
                float fx[2];
                fx[0] = (float)((x + 0.5) * scale_x - 0.5); // x
                int src_x = (int)fx[0];                     // x1
                fx[0] -= src_x;                             // x - x1
                if (src_x < 0)
                {
                    fx[0] = 0;
                    src_x = 0;
                }
                if (src_x > src_w - 2)
                {
                    fx[0] = 0;
                    src_x = src_w - 2;
                }
                fx[1] = 1 - fx[0]; // x2 - x

                for (int c = 0; c < dst_c; c++)
                {
                    dst_image[y * dst_stride + x * dst_c + c] = round(src_image[src_y * src_stride + src_x * dst_c + c] * fx[1] * fy[1] + src_image[src_y * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[1] + src_image[(src_y + 1) * src_stride + src_x * dst_c + c] * fx[1] * fy[0] + src_image[(src_y + 1) * src_stride + (src_x + 1) * dst_c + c] * fx[0] * fy[0]);
                }
            }
        }
    
} /*}}}*/

uint8_t* accessPixel(uint8_t *imgdata, int width, int height)
{
    int pos = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pos = i * width + j;
            imgdata[pos] = 255 - imgdata[pos];
        }
    }
    return imgdata;
}

uint8_t* accessBinary(uint8_t* imgdata, int width, int height, int threshold)
{
    imgdata = accessPixel(imgdata, height, width);
    
    int pos = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pos = i * width + j;
            if (imgdata[pos] > threshold)
            {
                imgdata[pos] = 255;
            }
            else
            {
                imgdata[pos] = 0;
            }
        }
    }

    return imgdata;
}

return_array extractPeek(int array_vals[], int array_size, int min_vals, int min_rect)
{
    return_array extractPeek;
    int startPoint = NULL;
    int endPoint = NULL;
    int count = 0;
    for (int i = 0; i < array_size; i++)
    {
        if (array_vals[i] > min_vals && startPoint == NULL)
        {
            startPoint = i;
        }
        else if (array_vals[i] < min_vals && startPoint != NULL)
        {
            endPoint = i;
        }
        if (startPoint != NULL and endPoint != NULL)
        {
            count++;
            startPoint = NULL;
            endPoint = NULL;
        }
    }

    int** extrackPoints = (int**)malloc((count) * sizeof(int*));
    
    for (int i = 0; i < count; i++)
    {
        extrackPoints[i] = (int*)malloc(2 * sizeof(int));
    }
    count = 0;
    startPoint = NULL;
    endPoint = NULL;
    for (int i = 0; i < array_size; i++)
    {
        if (array_vals[i] > min_vals && startPoint == NULL)
        {
            startPoint = i;
        }
        else if (array_vals[i] < min_vals && startPoint != NULL)
        {
            endPoint = i;
        }
        if (startPoint != NULL and endPoint != NULL)
        {
            extrackPoints[count][0] = startPoint;
            extrackPoints[count][1] = endPoint;
            count++;
            startPoint = NULL;
            endPoint = NULL;
        }
    }
    for (int i = 0; i < count; i++)
    {
        if (extrackPoints[i][1] - extrackPoints[i][0] < min_rect)
        {
            extrackPoints[i][1] = -1;
            extrackPoints[i][0] = -1;
        }
    }
    extractPeek.twod_array = extrackPoints;
    extractPeek.array_size = count;
    return extractPeek;
}

return_array findBorderHistogram(uint8_t* imgdata,int width, int height)
{
    return_array borders_array;
    imgdata = accessBinary(imgdata, width, height, 170);
    return_array hori_vals = sum(imgdata, width, height, 1);
    return_array hori_points = extractPeek(hori_vals.oned_array, hori_vals.array_size,100,10);
    free(hori_vals.oned_array);
    int count = 0;
    for (int i = 0; i < hori_points.array_size; i++)
    {
        if (hori_points.twod_array[i][0] != -1)
        {
            uint8_t* img_ext = extractImg(imgdata, width, height, hori_points.twod_array[i][0], hori_points.twod_array[i][1]);
            return_array vec_vals = sum(img_ext, width, hori_points.twod_array[i][1] - hori_points.twod_array[i][0] + 1, 0);
            return_array vec_points = extractPeek(vec_vals.oned_array, vec_vals.array_size,10,10);
            for (int j = 0; j < vec_points.array_size; j++)
            {
                if (vec_points.twod_array[j][0] != -1)
                {
                    count++;
                }
            }
            free(img_ext);
            free(vec_vals.oned_array);
            free(vec_points.twod_array);
        }
    }
    
    int** borders = (int**)malloc((count) * sizeof(int*));
    for (int i = 0; i < count; i++)
    {
        borders[i] = (int*)malloc(4 * sizeof(int));
    }
    count = 0;
    for (int i = 0; i < hori_points.array_size; i++)
    {
        if (hori_points.twod_array[i][0] != -1)
        {
            uint8_t* img_ext = extractImg(imgdata, width, height, hori_points.twod_array[i][0], hori_points.twod_array[i][1]);
            return_array vec_vals = sum(img_ext, width, hori_points.twod_array[i][1] - hori_points.twod_array[i][0] + 1, 0);
            return_array vec_points = extractPeek(vec_vals.oned_array, vec_vals.array_size,10,10);
            for (int j = 0; j < vec_points.array_size; j++)
            {
                if (vec_points.twod_array[j][0] != -1)
                {
                    borders[count][0] = vec_points.twod_array[j][0];
                    borders[count][1] = hori_points.twod_array[i][0];
                    borders[count][2] = vec_points.twod_array[j][1];
                    borders[count][3] = hori_points.twod_array[i][1];
                    count++;
                }
            }
            free(img_ext);
            free(vec_vals.oned_array);
            free(vec_points.twod_array);
        }
    }
    free(hori_points.twod_array);
    
    borders_array.array_size = count;
    borders_array.twod_array = borders;
    return borders_array;
    

}

return_array sum(uint8_t* imgdata,int width,int height, int axis)
{
    return_array array;
    //int height = img.rows;
    //int width = img.cols;
    int pos = 0;

    if (axis == 1)
    {
        int* sum;
        sum = (int*)malloc(height * sizeof(int));
        for (int i = 0; i < height; i++)
            sum[i] = 0;


        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                pos = i * width + j;
                sum[i] = sum[i] + imgdata[pos];
            }
        }
        array.oned_array = sum;
        array.array_size = height;
        return array;
    }
    if (axis == 0)
    {
        int* sum;
        sum = (int*)malloc(width * sizeof(int));
        for (int i = 0; i < width; i++)
            sum[i] = 0;
        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
                pos = i * width + j;
                sum[j] = sum[j] + imgdata[pos];

            }
        }
        array.oned_array = sum;
        array.array_size = width;
        return array;
    }
}

uint8_t* extractImg(uint8_t* imgdata,int width,int height, int upborder, int downborder)
{
    //int height = img.rows;
    //int width = img.cols;
    int pos = 0;
    int new_pos = 0;
    int new_height = downborder - upborder + 1;
    //Mat new_img(downborder - upborder + 1, width, CV_8U, Scalar(100));
    uint8_t* newimgdata = (uint8_t*)malloc(width * new_height* sizeof(uint8_t));
    for (int i = upborder; i <= downborder; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pos = i * width + j;
            int debug1= imgdata[pos];
            newimgdata[new_pos] = debug1;
            new_pos++;
        }
    }
    return newimgdata;
}


uint8_t* outputImg(uint8_t* imgdata, int width, int height, int* border)
{
    int border_width = border[2] - border[0] + 1;
    int border_height = border[3] - border[1] + 1;
    int pos = 0;
    int new_pos = 0;
    uint8_t* newimgdata = (uint8_t*)malloc(border_width * border_height * sizeof(uint8_t));
    for (int i = border[1]; i <= border[3]; i++)
    {
        for (int j = border[0]; j <= border[2]; j++)
        {
            pos = i * width + j;
            newimgdata[new_pos] = imgdata[pos];
            new_pos++;
        }
    }
    return newimgdata;
}
