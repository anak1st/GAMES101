#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char** argv)
{

    // Change the definition here to change resolution
    Scene scene(784, 784);

    Material* red =   new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    Material* light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Vector3f(0.65f);

    Material* mirror = new Material(MIRROR, Vector3f(0.0f));
    mirror->Kd = Vector3f(0.0f);
    mirror->ior = 40.0f;

    std::string folder = "../../models/cornellbox/";

    MeshTriangle floor   (folder + "floor.obj",    white);
    // MeshTriangle shortbox(folder + "shortbox.obj", white);
    MeshTriangle tallbox (folder + "tallbox.obj",  mirror);
    MeshTriangle left    (folder + "left.obj",     red);
    MeshTriangle right   (folder + "right.obj",    green);
    MeshTriangle light_  (folder + "light.obj",    light);
    
    // MeshTriangle sphere  (folder + "sphere2.obj",   mirror);
    Sphere sphere(Vector3f(150, 100, 200), 100, mirror);

    // MeshTriangle bunny("../../models/bunny/bunny.obj", mirror, 
    //     Vector3f(200,-60,150), 
    //     Vector3f(1500,1500,1500), 
    //     Vector3f(-1,0,0), 
    //     Vector3f(0,1,0), 
    //     Vector3f(0,0,-1));

    scene.Add(&floor);
    // scene.Add(&shortbox);
    scene.Add(&sphere);
    // scene.Add(&bunny);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;
    int spp = 16;
    if (argc == 2) spp = std::stoi(argv[1]);

    auto start = std::chrono::system_clock::now();
    r.Render(scene, spp);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}