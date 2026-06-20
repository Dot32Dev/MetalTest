//
//  camera.cpp
//  MetalTest
//
//  Created by Dot32  on 19/6/2026.
//  Adapted from https://github.com/Dot32Dev/terrain originally built 12/4/2026.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Camera::Camera(vec3 pos, vec2 dir) :
    targets(vector<Target>()),
    current(0),
    previous(0),
    progress(1.0)
{
    Target target {
        pos,
        dir
    };

    targets.push_back(target);
}

Camera::Camera(vec3 pos) :
    targets(vector<Target>()),
    current(0),
    previous(0),
    progress(1.0)
{
    Target target {
        pos,
        vec2(0.0f, 0.0f)
    };

    targets.push_back(target);
}

Camera::Camera() :
    targets(vector<Target>()),
    current(0),
    previous(0),
    progress(1.0)
{
    Target target {
        vec3(0.0f, 0.0f, 0.0f),
        vec2(0.0f, 0.0f)
    };

    targets.push_back(target);
}

mat4 Camera::get_view_matrix() const {
    // Lerp between previous and current by progress amount
    vec3 pos = targets[previous].pos
        + (targets[current].pos - targets[previous].pos) * progress;
    vec2 dir = targets[previous].dir
        + (targets[current].dir - targets[previous].dir) * progress;

    mat4 matrix = mat4(1.0);
    // Rotate by the camera's dir
    vec3 pitch_axis = vec3(1.0f, 0.0f, 0.0f);
    matrix = rotate(matrix, dir.y, pitch_axis);
    vec3 yaw_axis = vec3(0.0f, 1.0f, 0.0f);
    matrix = rotate(matrix, dir.x, yaw_axis);
    // Translate by the camera's pos
    // I found that negating it made the camera position more natural, e.g a
    // position of Y=10 should put you up in the air, not below the ground
    matrix = translate(matrix, -pos);
    return matrix;
}

void Camera::pos_input(CameraTarget target, vec3 input_vector) {
    // Rotate the input by the direction the camera is looking
    // This is so that if you walk forwards, you move forwards relative to the
    // view, rather than the cardinal direction which is forwards.
    vec3 yaw_axis = vec3(0.0f, -1.0f, 0.0f);
    mat4 rotation = rotate(mat4(1.0), targets[target].dir.x, yaw_axis);
    // This is just rotation * input, except rotation is mat4 and input is vec3
    targets[target].pos += vec3(rotation * vec4(input_vector, 1.0));
}

void Camera::dir_input(CameraTarget target, vec2 input_vector) {
    targets[target].dir += input_vector;
}

const vec3 Camera::get_position(CameraTarget target) const {
    return targets[target].pos;
}

void Camera::set_position(CameraTarget target, vec3 position) {
    targets[target].pos = position;
}

CameraTarget Camera::get_target() const {
    return current;
}

CameraTarget Camera::new_target(vec3 pos, vec2 dir) {
    Target target {
        pos,
        dir
    };
    targets.push_back(target);
    return CameraTarget(targets.size() - 1);
}

void Camera::set_target(CameraTarget target) {
    if (target == previous) {
        progress = 1.0 - progress;
    } else {
        progress = 0;
    }
    previous = current;
    current = target;
}

void Camera::next_target() {
    set_target((current + 1) % targets.size());
}

void Camera::animate(double dt) {
    accumulator += dt;

    while (accumulator > 1.0/60) {
        accumulator -= 1.0/60;
        progress = progress + (1.0 - progress)/10.0;
    }
}

void Camera::set_progress(float progress) {
    if (progress < 0.0)
        this->progress = 0;
    else if (progress > 1.0)
        this->progress = 1.0;
    else
        this->progress = progress;
}
