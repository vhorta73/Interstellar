#pragma once

#include "Interstellar/Input/IMouseManager.hpp"
#include <GLFW/glfw3.h>

namespace Interstellar::Input::Mouse {

    /**
    * @ingroup EngineInterfaces
     * @brief Concrete mouse input handler for GLFW-based systems.
     *
     * Tracks position, button states, and dragging behavior.
     * Coordinates are in window space (origin top-left; Y increases downwards).
     *
     * @since 1.0
     */
    class GlfwMouseManager : public IMouseManager {
    public:
        /**
         * @brief Constructs the mouse manager with a GLFW window context.
         * @param window Pointer to the GLFW window to track input for.
         * @since 1.0
         */
         GlfwMouseManager(GLFWwindow* window);

        /**
         * @brief Destructor.
         * @since 1.0
         */
        ~GlfwMouseManager() override = default;

        /**
         * @brief Call once per frame to update internal mouse state.
         * @since 1.0
         */
        void Update();

        /**
         * @brief Gets the current X position of the mouse in window coordinates.
         * @return X coordinate.
         * @since 1.0
         */
        double GetX() const override;

        /**
         * @brief Gets the current Y position of the mouse in window coordinates.
         * @return Y coordinate.
         * @since 1.0
         */
        double GetY() const override;

        /**
         * @brief Checks if a specific mouse button is currently pressed.
         * @param button The GLFW button code (e.g., GLFW_MOUSE_BUTTON_LEFT).
         * @return True if the button is pressed.
         * @since 1.0
         */
        bool IsButtonPressed(int button) const override;

        /**
         * @brief Returns true if the mouse is currently dragging.
         * Dragging is defined as holding the left button while moving the cursors across frames.
         * @since 1.0
         */
        bool IsDragging() const override;

        double GetScrollOffsetY() const override;
        void ResetScrollOffset();

    private:
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        GLFWwindow* m_Window = nullptr;
        double m_X = 0.0;
        double m_Y = 0.0;
        double m_LastX = 0.0;
        double m_LastY = 0.0;
        bool m_IsDragging = false;
        double m_ScrollOffsetY = 0.0;
    };

}
