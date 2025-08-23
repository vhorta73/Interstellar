#pragma once

/**
 * @file
 * @ingroup Input
 * @brief Umbrella include for the Interstellar Input module.
 * @details Pulls in the core input system, device interfaces, key and mouse
 *          codes, build information, and default device implementations.
 *          Include this header in game code to access all input features.
 * @since 1.0
 */

#include <Interstellar/Input/InputSystem.hpp>
#include <Interstellar/Input/IKeyboard.hpp>
#include <Interstellar/Input/IMouse.hpp>
#include <Interstellar/Input/KeyCodes.hpp>
#include <Interstellar/Input/MouseButtons.hpp>
#include <Interstellar/Input/Build.hpp>

 // Per-device helpers:
#include <Interstellar/Input/Keyboard.hpp>
#include <Interstellar/Input/Mouse.hpp>
