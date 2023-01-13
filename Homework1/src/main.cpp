#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <format>
#include <numbers>
#include <cmath>

constexpr double MY_PI = 3.1415926;
constexpr float pi = std::numbers::pi_v<float>;
constexpr float degree2radian(float angle) {
    return angle / 180 * pi;
}

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 
                 0, 1, 0, -eye_pos[1], 
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
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

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float angle) 
{   
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    
    float radian = degree2radian(angle);
    Eigen::Matrix4f any_rotation = Eigen::Matrix4f::Zero();
    any_rotation(3, 3) = 1;
    
    Eigen::Vector3f normal_axis = axis.normalized();

    Eigen::Matrix3f mult_factor;
    mult_factor << 0, -normal_axis.z(), normal_axis.y(),
                   normal_axis.z(), 0, -normal_axis.x(),
                   -normal_axis.y(), normal_axis.x(), 0;

    mult_factor = std::cos(radian) * Eigen::Matrix3f::Identity()
                + (1 - std::cos(radian)) * normal_axis * normal_axis.transpose()
                + std::sin(radian) * mult_factor;

    any_rotation.block(0, 0, 3, 3) = mult_factor.block(0, 0, 3, 3);

    projection = any_rotation * projection;

    return projection;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    float radian = degree2radian(eye_fov);
    float ty = -1.0f / std::tan(radian / 2.0f);
    Eigen::Matrix4f aspect_fovY;
    aspect_fovY << (ty / aspect_ratio), 0, 0, 0,
                   0, ty, 0, 0,
                   0, 0, (zNear + zFar) / (zNear - zFar), (-2 * zNear * zFar) / (zNear - zFar),
                   0, 0, 1, 0;
    projection = aspect_fovY * projection;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    std::cout << std::fixed << std::setprecision(4);

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        // r.set_model(get_model_matrix(angle));
        r.set_model(get_rotation(Eigen::Vector3f(0, 0, 1), angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        frame_count++;
        if (frame_count % 100 == 0) {
            std::cout << std::format("frame count: {}\n", frame_count);
            std::cout << get_model_matrix(angle) << "\n\n";
        }   

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
