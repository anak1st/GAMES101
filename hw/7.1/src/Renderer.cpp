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
#include <fmt/core.h>

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
    std::vector<Vector3f> framebuffer       (scene.width * scene.height);
    std::vector<Vector3f> framebuffer_new   (scene.width * scene.height);
    std::vector<int>      framebuffer_flag  (scene.width * scene.height, 0);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);

    ThreadPool pool(8);
    pool.init();

    int frame = 0;
    float alpha = 0.1f;

    auto func = [&](int i) {
        for (int j = 0; j < scene.width; j++) {
            Vector3f color;
            {
                float x = get_random_float();
                float y = get_random_float();
                float _x = (2 * (j + x) / (float)scene.width - 1) * imageAspectRatio * scale;
                float _y = (1 - 2 * (i + y) / (float)scene.height) * scale;
                Vector3f dir = normalize(Vector3f(-_x, _y, 1));
                Ray ray = Ray(eye_pos, dir);
                color += 1.0f / spp * scene.castRay(ray, 0);
            }
            int index = i * scene.width + j;

            framebuffer_new[index] += color;
            
            if (framebuffer_flag[index] == 0) {
                framebuffer[index] = framebuffer_new[index];
            }
            if (frame % spp == 0) {
                framebuffer[index] = framebuffer[index] * (1 - alpha) + framebuffer_new[index] * alpha;
                framebuffer_new[index] = Vector3f(0);
                framebuffer_flag[index]++;
            }
        }
    };

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    std::cout << "Rendering scene with spp = " << spp << std::endl;
    
    int key = 0;
    while (true) {
        ++frame;
        for (int i = 0; i < scene.height; i++) {
            pool.submit(func, i);
        }
        pool.wait();
        std::cout << "\rframe: " << frame << ", current spp: " << spp;
        std::cout.flush();

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
        if (key == 27 || frame >= spp + 1000) {
            // save the image
            cv::imwrite(fmt::format("images/{}.png", spp), image);
            break;
        }
    }

    pool.shutdown();
    std::cout << std::endl;
}
