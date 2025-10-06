/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <gsl/gsl>
#include <vector>

namespace CS230
{
    /**
     * Manages keyboard input state and provides frame-based input queries for game systems.
     *
     * This class abstracts keyboard input by tracking key states across frames, enabling
     * detection of key press events (just pressed, held down, just released). It maintains
     * both current and previous frame states to provide edge detection for one-frame events.
     *
     * Typical usage involves calling Update() once per frame to synchronize with the current
     * keyboard state, then using the query methods to check for specific input conditions
     * during game logic processing.
     */
    class Input
    {
    public:
        /**
         * Enumeration of supported keyboard keys for input queries.
         *
         * Covers common alphabet keys, navigation keys, and special keys typically
         * used in games and interactive applications. The Count value serves as
         * a sentinel for iteration and array sizing purposes.
         */
        enum class Keys
        {
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            Space,
            Enter,
            Left,
            Up,
            Right,
            Down,
            Escape,
            Tab,
            Count
        };

        /**
         * Initializes the input system and prepares internal state tracking.
         *
         * Sets up data structures to track key states across frames. The system
         * starts with all keys in a released state until the first Update() call.
         */
        Input();

        /**
         * Updates the input system with the current frame's keyboard state.
         *
         * Should be called once per frame to synchronize internal state with the
         * actual keyboard. This captures the current state and preserves the previous
         * frame's state for edge detection. Call this before any input queries.
         */
        void Update();

        /**
         * Checks if a key is currently being held down.
         *
         * @param key The key to check
         * @return True if the key is currently pressed, false otherwise
         *
         * This will return true for as long as the key remains pressed, making it
         * suitable for continuous actions like movement or repeated firing.
         */
        bool KeyDown(Keys key) const;

        /**
         * Checks if a key was released this frame.
         *
         * @param key The key to check
         * @return True if the key was pressed last frame but not this frame
         *
         * Returns true for exactly one frame when a key transitions from pressed
         * to released. Useful for actions that should happen once when stopping
         * an input (like releasing a jump button).
         */
        bool KeyJustReleased(Keys key) const;

        /**
         * Checks if a key was pressed this frame.
         *
         * @param key The key to check
         * @return True if the key was not pressed last frame but is pressed this frame
         *
         * Returns true for exactly one frame when a key transitions from released
         * to pressed. Essential for one-shot actions like jumping, shooting, or
         * menu navigation that should only trigger once per key press.
         */
        bool KeyJustPressed(Keys key) const;

    private:
        // Store both current and previous frame key states for edge detection
        // Consider using vectors or arrays sized by Keys::Count for state tracking
        std::vector<bool> keys_down;
        std::vector<bool> previous_keys_down;

    private:
        // Helper method to update individual key states during frame updates
        void SetKeyDown(Keys key, bool value);
    };

    /**
     * Increment operator for Keys enumeration to enable iteration.
     *
     * @param the_key Reference to the key to increment
     * @return Reference to the incremented key
     *
     * Allows for easy iteration through all keys using range-based loops or
     * traditional for loops. Useful when processing all keys in sequence.
     */
    constexpr Input::Keys& operator++(Input::Keys& the_key) noexcept
    {
        the_key = static_cast<Input::Keys>(static_cast<unsigned>(the_key) + 1);
        return the_key;
    }

    /**
     * Converts a Keys enumeration value to its string representation.
     *
     * @param key The key to convert to a string
     * @return C-style string name of the key
     *
     * Useful for debugging, logging, or displaying key names in user interfaces.
     * Returns human-readable names like "A", "Space", "Enter", etc.
     */
    constexpr gsl::czstring to_string(Input::Keys key) noexcept
    {
        switch (key)
        {
            case Input::Keys::A: return "A";
            case Input::Keys::B: return "B";
            case Input::Keys::C: return "C";
            case Input::Keys::D: return "D";
            case Input::Keys::E: return "E";
            case Input::Keys::F: return "F";
            case Input::Keys::G: return "G";
            case Input::Keys::H: return "H";
            case Input::Keys::I: return "I";
            case Input::Keys::J: return "J";
            case Input::Keys::K: return "K";
            case Input::Keys::L: return "L";
            case Input::Keys::M: return "M";
            case Input::Keys::N: return "N";
            case Input::Keys::O: return "O";
            case Input::Keys::P: return "P";
            case Input::Keys::Q: return "Q";
            case Input::Keys::R: return "R";
            case Input::Keys::S: return "S";
            case Input::Keys::T: return "T";
            case Input::Keys::U: return "U";
            case Input::Keys::V: return "V";
            case Input::Keys::W: return "W";
            case Input::Keys::X: return "X";
            case Input::Keys::Y: return "Y";
            case Input::Keys::Z: return "Z";
            case Input::Keys::Space: return "Space";
            case Input::Keys::Enter: return "Enter";
            case Input::Keys::Left: return "Left";
            case Input::Keys::Up: return "Up";
            case Input::Keys::Right: return "Right";
            case Input::Keys::Down: return "Down";
            case Input::Keys::Escape: return "Escape";
            case Input::Keys::Tab: return "Tab";
            case Input::Keys::Count: return "Count";
        }
        return "Unknown";
    }
}
