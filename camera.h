//
//  camera.h
//  MetalTest
//
//  Created by Dot32  on 19/6/2026.
//  Adapted from https://github.com/Dot32Dev/terrain originally built 12/4/2026.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::mat4;
using std::vector;

/// @brief A Camera Target
using CameraTarget = std::size_t;

/// @brief Camera
/// Manages the creation of "camera targets", switching between them and
/// modifying them with user input.
///
/// This class just handles a view matrix, use get_view_matrix() and
/// send it to your shader's view uniform to use the camera.
class Camera {
    public:
        /// @brief Creates a camera with the specified location + direction
        /// A camera target is created with the specified location + direction,
        /// and that camera target is set as the active camera target.
        /// @param pos The initial positiln
        /// @param dir The initial dir
        Camera(vec3 pos, vec2 dir);

        /// @brief Spawns a camera at the location provided
        /// Will face the direction of the identity matrix.
        /// A camera target is created with the specified location + direction,
        /// and that camera target is set as the active camera target.
        /// @param pos The position to place the camera at
        Camera(vec3 pos);

        /// @brief Spawns a camera at the origin
        /// Will face the direction of the identity matrix.
        /// A camera target is created with the specified location + direction,
        /// and that camera target is set as the active camera target.
        Camera();

        /// @brief Returns the cameras view matrix to be sent to the shader
        /// This is a lerped position between the previous camera target and the
        /// current camera target. Call animate_camera(dt) or set_progress(n) to
        /// influence the progress of the camera between its camera targets.
        /// @return The matrix to be sent to the shader
        mat4 get_view_matrix() const;

        /// @brief Move a camera target by some given input
        /// This should reflect the current input that the user is making.
        /// The camera target will move by adding this vector to its current
        /// location, but adjusting for the camera target's current look
        /// direction. If you give an input vector where z = 1, the camera
        /// target will move forward one unit in the direction you are looking.
        /// However, the camera target's vertical movement does not take into
        /// account the look direction as I prefer for up to always be up, and
        /// moving horizontally to always be horizontal.
        /// @param target The target to move
        /// @param input_vector The input movement vector to move by
        void pos_input(CameraTarget target, vec3 input_vector);

        /// @brief Rotate a camera target by some given input
        /// This should reflect the current input that the user is making.
        /// The target's direction will be updated by the value provided
        /// @param target The target to rotate
        /// @param input_vector The input direction vector to rotate by
        void dir_input(CameraTarget target, vec2 input_vector);

        /// @brief Get the position of a camera target
        /// @param target The target to get the position of
        /// @return The vec3 position of the target
        const vec3 get_position(CameraTarget target) const;

        /// @brief Set the position of a camera target
        /// @param target The target to set the position of
        /// @param position The position to set the target to
        void set_position(CameraTarget target, vec3 position);

        /// @brief Get the current camera target
        /// This is necessary for performing actions on the camera
        /// @return The current camera target
        CameraTarget get_target() const;

        /// @brief Create a new camera target
        /// To use the camera target, use set_target() with the returned target,
        /// or next_target() to automatically switch to the next camera target
        /// @param pos The position of the new camera target
        /// @param dir The direction of the new camera target
        /// @return The new camera target
        CameraTarget new_target(vec3 pos, vec2 dir);

        /// @brief Set the active camera target to the specified target
        /// This will not immediately change the camera view. Use animate(dt)
        /// to blend smoothly to the new camera target, or set_progress(1.0) to
        /// teleport there immediately.
        /// @param target The target to activate
        void set_target(CameraTarget target);

        /// @brief Switch the active camera target to the next camera target
        /// As you have no control over the order of camera targets, it is
        /// recommended to create your own array of camera targets and manually
        /// use set_target() if you want more control.
        void next_target();

        /// @brief Animates between the previous camera target and the current
        /// @param dt The delta time between this and the previous frane
        void animate(double dt);

        /// @brief  Sets the animation progress between the previous and current
        /// camera targets.
        /// A progress of 0.0 is the previous target, and a progress of 1.0 is
        /// the current target. Input progresses will be clamped to this range.
        /// @param progress The progress to set
        void set_progress(float progress);
    private:
        struct Target {
            vec3 pos;
            vec2 dir;
        };
        vector<Target> targets;
        CameraTarget current;
        CameraTarget previous;
        float progress;
        double accumulator;
};

#endif /* CAMERA_H */
