#include "Triangle.hpp"
#include "global.hpp"
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <format>
#include <iostream>
#include <numbers>

constexpr double MY_PI = 3.1415926;
constexpr float pi = std::numbers::pi_v<float>;
constexpr float degree2radian(float angle) {
    return angle / 180 * pi;
}

struct Timer_v2 {
    using Clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<Clock> p;
    Timer_v2() {
        p = Clock::now();
    };
    float time() {
        std::chrono::time_point<Clock> q = Clock::now();
        std::chrono::duration<float> duration = q - p;
        p = q;
        float ms = duration.count() * 1000.0f;
        // std::cerr << "time took " << ms << "ms" << std::endl;
        return ms;
    }
};

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 
                 0, 1, 0, -eye_pos[1], 
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    float radian = degree2radian(rotation_angle);
    float sinz = std::sin(radian);
    float cosz = std::cos(radian);
    Eigen::Matrix4f translate;
    translate << cosz, -sinz, 0, 0, 
                 sinz, cosz, 0, 0, 
                 0, 0, 1, 0, 
                 0, 0, 0, 1;
    model = translate * model;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Copy-paste your implementation from the previous assignment.

    float radian = degree2radian(eye_fov);
    float cot_fovd2 = -1.0f / std::tan(radian / 2.0f);
    Eigen::Matrix4f aspect_fovY;
    aspect_fovY << (cot_fovd2 / aspect_ratio), 0, 0, 0,
                   0, cot_fovd2, 0, 0,
                   0, 0, (zNear + zFar) / (zNear - zFar), (-2.0f * zNear * zFar) / (zNear - zFar),
                   0, 0, 1, 0;
    projection = aspect_fovY * projection;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc == 2)
    {
        command_line = true;
        filename = std::string(argv[1]);
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0,0,5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2},    {0, 2, -2},     {-2, 0, -2},
                                     {3.5, -1, -5}, {2.5, 1.5, -5}, {-1, 0.5, -5}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}, {3, 4, 5}};

    std::vector<Eigen::Vector3f> cols{{217.0, 238.0, 185.0}, {217.0, 238.0, 185.0},
                                      {217.0, 238.0, 185.0}, {185.0, 217.0, 238.0},
                                      {185.0, 217.0, 238.0}, {185.0, 217.0, 238.0}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    auto col_id = r.load_colors(cols);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    Timer_v2 timer;

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        frame_count++;
        angle += 0.1;
        if (frame_count % 10 == 0) {
            float s = timer.time() / 1000.0f;
            float frame_rate = 10.0f / s;
            std::cout << std::format("frame count: {0}, frame rate {1:.2f}\r", frame_count, frame_rate);
        }
    }

    return 0;
}