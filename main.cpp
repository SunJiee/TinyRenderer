#include <vector>
#include <cmath>
#include "common/tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const int width  = 200;
const int height = 200;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void triangle(int t0[], int t1[], int t2[], TGAImage &image, TGAColor color) {
    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!)
    if(t0[1]>t1[1]) std::swap(t0, t1);
    if(t0[1]>t2[1]) std::swap(t0, t2);
    if(t1[1]>t2[1]) std::swap(t1, t2);
    int total_height = t2[1]-t0[1];
    for (int y = t0[1]; y < t1[1]; y++){
        int segment_height = t1[1] - t0[1] + 1;
        float alpha = (float)(y - t0[1]) / total_height;
        float beta = (float)(y - t0[1]) / segment_height; // be careful with divisions by zero
        int A = t0[0] + (t2[0] - t0[0]) * alpha;
        int B = t0[0] + (t1[0] - t0[0]) * beta;
        if (A>B) std::swap(A, B);
        for (int j=A; j<=B; j++) {
            image.set(j, y, color);
        }
    }
    for (int y = t1[1]; y < t2[1]; y++){
        int segment_height = t2[1] - t1[1] + 1;
        float alpha = (float)(y - t0[1]) / total_height;
        float beta = (float)(y - t1[1]) / segment_height; // be careful with divisions by zero
        int A = t0[0] + (t2[0] - t0[0]) * alpha;
        int B = t1[0] + (t2[0] - t1[0]) * beta;
        if (A>B) std::swap(A, B);
        for (int j=A; j<=B; j++) {
            image.set(j, y, color);
        }
    }
}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    int t0[3][2] = {{10, 70},   {50, 160},  {70, 80}};
    int t1[3][2] = {{180, 50},  {150, 1},   {70, 180}};
    int t2[3][2] = {{180, 150}, {120, 160}, {130, 180}};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}