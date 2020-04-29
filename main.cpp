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

float* barycentric(float co[3],int pts[3][2], int P[2]) {
    float u[3] = {(float)((pts[1][0]-pts[0][0])*(pts[0][1]-P[1])-(pts[1][1]-pts[0][1])*(pts[0][0]-P[0])),
                  (float)((pts[0][0]-P[0])*(pts[2][1]-pts[0][1])-(pts[2][0]-pts[0][0])*(pts[0][1]-P[1])),
                  (float)((pts[2][0]-pts[0][0])*(pts[1][1]-pts[0][1])-(pts[2][1]-pts[0][1])*(pts[1][0]-pts[0][0]))};
    if (std::abs(u[2])<1) {
        co[0] = -1;
        co[1] = 1;
        co[2] = 1;
    }else{
        co[0] = 1.f - (u[0] + u[1]) / u[2];
        co[1] = u[1] / u[2];
        co[2] = u[0] / u[2];
    }
    return co;
}

void triangle(int pts[3][2], TGAImage &image, TGAColor color) {
    int bboxmin[2] = {image.get_width()-1,  image.get_height()-1};
    int bboxmax[2] = {0, 0};
    int clamp[2] = {image.get_width() - 1, image.get_height() - 1};
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    int P[2];
    for (P[0]=bboxmin[0]; P[0]<=bboxmax[0]; P[0]++) {
        for (P[1]=bboxmin[1]; P[1]<=bboxmax[1]; P[1]++) {
            float co[3];
            float bc_screen[3] = {barycentric(co,pts, P)[0],barycentric(co,pts, P)[1],barycentric(co,pts, P)[2]};
            if (bc_screen[0]<0 || bc_screen[1]<0 || bc_screen[2]<0) continue;
            image.set(P[0], P[1], color);
        }
    }
}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    int t0[3][2] = {{10, 70},   {50, 160},  {70, 80}};
    int t1[3][2] = {{180, 50},  {150, 1},   {70, 180}};
    int t2[3][2] = {{180, 150}, {120, 160}, {130, 180}};

    triangle(t0, image, red);
    triangle(t1, image, white);
    triangle(t2, image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}