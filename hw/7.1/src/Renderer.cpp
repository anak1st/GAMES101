//
// Created by goksu on 2/25/20.
//

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <opencv2/opencv.hpp>

#include "Scene.hpp"
#include "Renderer.hpp"
#include "ThreadPool.hpp"

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene, int spp)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);

    std::cout << "SPP: " << spp << "\n";

    ThreadPool pool(8);
    pool.init();

    int frame = 0;

    auto func = [&](int i) {
        for (int j = 0; j < scene.width; j++) {
            float x = get_random_float();
            float y = get_random_float();
            float _x = (2 * (j + x) / (float)scene.width - 1) * imageAspectRatio * scale;
            float _y = (1 - 2 * (i + y) / (float)scene.height) * scale;
            Vector3f dir = normalize(Vector3f(-_x, _y, 1));
            Ray ray = Ray(eye_pos, dir);
            Vector3f color = scene.castRay(ray, 0, true);
            // framebuffer[i * scene.width + j] = (1 - 1.0f / spp) * framebuffer[i * scene.width + j] + (1.0f / spp) * color;
            if (frame <= spp) {
                framebuffer[i * scene.width + j] += 1.0f / spp * color;
            } else {
                framebuffer[i * scene.width + j] = (1 - 1.0f / spp) * framebuffer[i * scene.width + j] + (1.0f / spp) * color;
            }
        }
    };

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    
    int key = 0;
    while (true) {
        ++frame;
        
        for (int i = 0; i < scene.height; i++) {
            pool.submit(func, i);
        }
        pool.wait();
        std::cerr << "frame: " << frame << ", current spp: " << spp << "\n";
        std::cerr.flush();

        std::vector<Vector3f> image_data(framebuffer.size());
        std::copy(framebuffer.begin(), framebuffer.end(), image_data.begin());
        cv::Mat image(scene.height, scene.width, CV_32FC3, image_data.data());
        // 1. x = clamp(0, 1, x)
        // 2. x = pow(x, 0.6)
        // 3. x = x * 255
        image = cv::max(image, 0.0f);
        image = cv::min(image, 1.0f);
        cv::pow(image, 0.6, image);
        image = 255.f * image;
        
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);

        key = cv::waitKey(10);
        if (key == 27) {
            // save the image
            cv::imwrite("image.png", image);
            break;
        }
    }

    pool.shutdown();
}
