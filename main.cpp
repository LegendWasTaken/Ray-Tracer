#include <iostream>
#include <chrono>
#include "File.h"
#include "World.h"
#include "Vector3.h"
#include "Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader.h"
#include "Texture.h"
#include "DynMat.h"
#include "Lambertian.h"
#include "Glass.h"
#include <thread>
#include "Triangle.h"
#include "Rectangle.h"
#include "NormalMat.h"
#include "Window.h"

constexpr int WIDTH = 1024, HEIGHT = 576, MAX_BOUNCES = 6, SPP = 1;

//#define SPHERES

int* pixels;

void renderAsync(World& world){
    world.buildBvh();
    auto start = std::chrono::high_resolution_clock::now();
    world.render(pixels, 14);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = end - start;
    printf("Total Execution Time %.2fms\n",
           std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() / 1000000.0f);
}

int main(int argc, char **argv) {

//    const char* p = "../skybox/abandoned_parking.jpg";
//    const char* p = "../skybox/modern_buildings_night.jpg";
//    const char* p = "../skybox/kloofendal_48d_partly_cloudy.jpg";
//    const char* p = "../skybox/shanghai_bund.jpg";
    const char* p = {"../skybox/adams_place_bridge.jpg"};
    pixels = new int[WIDTH * HEIGHT]{0};
    Texture t = Texture(p, .875, 0);
//    Texture t = Texture();
    auto world = World(WIDTH, HEIGHT, t, MAX_BOUNCES, SPP);

#ifdef CORNELL
        Vector3 translation(0, 0, -20);
        Vector3 size(12, 35, 12);
        float scale = 2;
        world.addShape(
            new Rectangle(
                    Vector3(-(size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                    new Lambertian(Texture(0.96, 0.96, 0.86))));
        world.addShape(
            new Rectangle(
                    Vector3(-(size.x / 2) * scale, -size.y / 2, (size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, size.y / 2, (size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                    new Lambertian(Texture(0.35,0.70,0.00))));
        world.addShape(
                new Rectangle(
                        Vector3((size.x / 2) * scale, size.y / 2, (size.z / 2) * scale) + translation,
                        Vector3((size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                        Vector3((size.x / 2) * scale, -size.y / 2, (size.z / 2) * scale) + translation,
                        Vector3((size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                        new Lambertian(Texture(1.00,0.30,0.30))));
        world.addShape(
            new Rectangle(
                    Vector3(-(size.x / 2) * scale, size.y / 2, (size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, size.y / 2, (size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, size.y / 2, -(size.z / 2) * scale) + translation,
                    new Lambertian(Texture(0.96, 0.96, 0.86))));
    scale /=2;
        world.addShape(
            new Rectangle(
                    Vector3(-(size.x / 2) * scale, size.y / 2 - 0.2, (size.z / 2) * scale) + translation,
                    Vector3(-(size.x / 2) * scale, size.y / 2 - 0.2, -(size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, size.y / 2 - 0.2, (size.z / 2) * scale) + translation,
                    Vector3((size.x / 2) * scale, size.y / 2 - 0.2, -(size.z / 2) * scale) + translation,
                    new Lambertian(Texture(), Vector3(1, 1, 1))));
    scale *= 2;
        world.addShape(
                new Rectangle(
                        Vector3((size.x / 2) * scale, -size.y / 2, (size.z / 2) * scale) + translation,
                        Vector3((size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                        Vector3(-(size.x / 2) * scale, -size.y / 2, (size.z / 2) * scale) + translation,
                        Vector3(-(size.x / 2) * scale, -size.y / 2, -(size.z / 2) * scale) + translation,
                        new Lambertian(Texture(0.96, 0.96, 0.86))));
#endif

    // Obj loading time

    std::string inputfile = "../objs/rabbit.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    Material* foxMat = new Lambertian(Texture("../objs/texture.png"));

    Material* floorMat = new DynMat(0, 1, Texture(1, 1, 1));

    world.addShape(new Rectangle(
            Vertex(Vector3(10, -0, -10), 0, 0),
            Vertex(Vector3(-10, -0, -10), 0, 0),
            Vertex(Vector3(10, -0, 10), 0, 0),
            Vertex(Vector3(-10, -0, 10), 0, 0), floorMat));

    for(size_t s=0; s<shapes.size(); s++){
//    for(size_t s=0; s<0; s++){
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            Vertex vertices[3];
            for (size_t v = 0; v < fv; v++) {
                // access to Vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
                vertices[v] = Vertex(
                        Vector3(attrib.vertices[3*idx.vertex_index+0],
                        attrib.vertices[3*idx.vertex_index+1],
                        attrib.vertices[3*idx.vertex_index+2]),
                        attrib.texcoords[2*idx.texcoord_index+0],
                        attrib.texcoords[2*idx.texcoord_index+1]);
            }

            world.addShape(new Triangle(
                    vertices[0],
                    vertices[1],
                    vertices[2],
//                    new Lambertian(Texture(0.0, 0.7, 0.7))));
//                    new DynMat(0, 1, Texture(1, 1, 1))
                    new NormalMat(true)));
//                    foxMat));
            indexOffset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }

    Window window(WIDTH, HEIGHT, pixels);
    std::thread renderThread(renderAsync, std::ref(world));
    window.start();
    renderThread.join();
    delete(pixels);
    return 0;
}