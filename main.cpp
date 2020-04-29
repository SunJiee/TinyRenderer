#include <vector>
#include <cmath>
#include "common/tgaimage.h"
#include "common/model.h"
#include "common/geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

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
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }
    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);

    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        int screen_coords[3][2];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j][0] = (v.x+1.)*width/2.;
            screen_coords[j][1] = (v.y+1.)*height/2.;
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); // ^应该是叉乘
        n.normalize();
        float intensity = n*light_dir; // *应该是点乘
        if (intensity>0) {
            triangle(screen_coords, image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }
    // for (int i=0; i<model->nfaces(); i++) {
    //     std::vector<int> face = model->face(i);
    //     int screen_coords[3][2];
    //     for (int j=0; j<3; j++) {
    //         Vec3f world_coords = model->vert(face[j]);
    //         screen_coords[j][0] = (world_coords.x+1.)*width/2.;
    //         screen_coords[j][1] = (world_coords.y+1.)*height/2.;
    //     }
    //     triangle(screen_coords, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
    // }
    // Fill them with a random color
        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("output.tga");
        delete model;
        return 0;
}