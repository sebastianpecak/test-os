#include "sys_dev_kbrd.h"
#include "sys_stdio.h"
#include "sys_dev_ps2.h"
#include "sys_string.h"
#include "terminal.h"
#include "sys_ascii.h"

#define KBRD_CMD_SCANCODE 0xF0
#define KBRD_CMD_RESET 0xFF

/**
 * @brief Scan Code Set 1 to ASCII map.
 * 
 */
static const uint8_t _scs1_to_ascii[] =
{
    ASCII_INVALID, // SCS1 0x00
    ASCII_ESCAPE, // SCS1 0x01
    ASCII_DIGIT_1, // SCS1 0x02
    ASCII_DIGIT_2, // SCS1 0x03
    ASCII_DIGIT_3, // SCS1 0x04
    ASCII_DIGIT_4, // SCS1 0x05
    ASCII_DIGIT_5, // SCS1 0x06
    ASCII_DIGIT_6, // SCS1 0x07
    ASCII_DIGIT_7, // SCS1 0x08
    ASCII_DIGIT_8, // SCS1 0x09
    ASCII_DIGIT_9, // SCS1 0x0A
    ASCII_DIGIT_0, // SCS1 0x0B
    ASCII_SYMBOL_DASH, // SCS1 0x0C
    ASCII_SYMBOL_EQUAL, // SCS1 0x0D
    ASCII_BACKSPACE, // SCS1 0x0E
    ASCII_HORIZONTAL_TAB, // SCS1 0x0F
    ASCII_CHAR_q, // SCS1 0x10
    ASCII_CHAR_w, // SCS1 0x11
    ASCII_CHAR_e, // SCS1 0x12
    ASCII_CHAR_r, // SCS1 0x13
    ASCII_CHAR_t, // SCS1 0x14
    ASCII_CHAR_y, // SCS1 0x15
    ASCII_CHAR_u, // SCS1 0x16
    ASCII_CHAR_i, // SCS1 0x17
    ASCII_CHAR_o, // SCS1 0x18
    ASCII_CHAR_p, // SCS1 0x19
    ASCII_SYMBOL_SQUARE_BRACKET_OPEN, // SCS1 0x1A
    ASCII_SYMBOL_SQUARE_BRACKET_CLOSE, // SCS1 0x1B
    ASCII_LINE_FEED, // ENTER
    //ASCII_CARRIAGE_RETURN, // SCS1 0x1C
    ASCII_INVALID, // SCS1 0x1D
    ASCII_CHAR_a, // SCS1 0x1E
    ASCII_CHAR_s, // SCS1 0x1F
    ASCII_CHAR_d, // SCS1 0x20
    ASCII_CHAR_f, // SCS1 0x21
    ASCII_CHAR_g, // SCS1 0x22
    ASCII_CHAR_h, // SCS1 0x23
    ASCII_CHAR_j, // SCS1 0x24
    ASCII_CHAR_k, // SCS1 0x25
    ASCII_CHAR_l, // SCS1 0x26
    ASCII_SYMBOL_SEMICOLON, // SCS1 0x27
    ASCII_SYMBOL_APOSTROPHE, // SCS1 0x28
    ASCII_SYMBOL_BACKTICK, // SCS1 0x29
    ASCII_INVALID, // SCS1 0x2A
    ASCII_SYMBOL_BACKSLASH, // SCS1 0x2B
    ASCII_CHAR_z, // SCS1 0x2C
    ASCII_CHAR_x, // SCS1 0x2D
    ASCII_CHAR_c, // SCS1 0x2E
    ASCII_CHAR_v, // SCS1 0x2F
    ASCII_CHAR_b, // SCS1 0x30
    ASCII_CHAR_n, // SCS1 0x31
    ASCII_CHAR_m, // SCS1 0x32
    ASCII_SYMBOL_COMMA, // SCS1 0x33
    ASCII_SYMBOL_DOT, // SCS1 0x34
    ASCII_SYMBOL_SLASH, // SCS1 0x35
    ASCII_INVALID, // SCS1 0x36
    ASCII_INVALID, // SCS1 0x37
    ASCII_INVALID, // SCS1 0x38
    ASCII_SPACE, // SCS1 0x39
};

enum Scs1SpecialKeys_e
{
    SCS1_LEFT_CONTROL_PRESSED = 0x1D,
    SCS1_LEFT_SHIFT_PRESSED = 0x2A,
    SCS1_RIGHT_SHIFT_PRESSED = 0x36,
    SCS1_LEFT_ALT_PRESSED = 0x38,
    SCS1_CAPSLOCK_PRESSED = 0x3A,
    SCS1_F1_PRESSED = 0x3B,
    SCS1_F2_PRESSED,
    SCS1_F3_PRESSED,
    SCS1_F4_PRESSED,
    SCS1_F5_PRESSED,
    SCS1_F6_PRESSED,
    SCS1_F7_PRESSED,
    SCS1_F8_PRESSED,
    SCS1_F9_PRESSED,
    SCS1_F10_PRESSED,
    SCS1_F11_PRESSED = 0x57,
    SCS1_F12_PRESSED,
    SCS1_LEFT_CONTROL_RELEASED = 0x9D,
    SCS1_LEFT_SHIFT_RELEASED = 0xAA,
    SCS1_LEFT_ALT_RELEASED = 0xB8,
    SCS1_RIGHT_SHIFT_RELEASED = 0xB6
};

static struct DevKbrdState_s _kbrd_state;

void sys_dev_kbrd_init()
{
    uint8_t data = 0;

    sys_dev_ps2_write_to_first_port(KBRD_CMD_RESET);
    (void) sys_dev_ps2_poll_data();

    sys_memset(&_kbrd_state, 0, sizeof(_kbrd_state));
    sys_dev_ps2_write_to_first_port(KBRD_CMD_SCANCODE);
    sys_dev_ps2_write_to_first_port(0);
    data = sys_dev_ps2_poll_data();
    sys_printf("sys_dev_kbrd_init: current SCS is %#x, change to SCS1\n", data);
    sys_dev_ps2_write_to_first_port(KBRD_CMD_SCANCODE);
    sys_dev_ps2_write_to_first_port(1);
    data = sys_dev_ps2_poll_data();
    sys_printf("sys_dev_kbrd_init: SCS change result %#x\n", data);
    sys_dev_ps2_write_to_first_port(KBRD_CMD_SCANCODE);
    sys_dev_ps2_write_to_first_port(0);
    data = sys_dev_ps2_poll_data();
    sys_printf("sys_dev_kbrd_init: current SCS is %#x\n", data);
}

/**
 * @brief Returns non-zero value if action was actually processed by the function.
 * 
 * @param action 
 * @return int 
 */
static int _kbrd_process_special_key(uint8_t action)
{
    switch (action)
    {
    case SCS1_CAPSLOCK_PRESSED:
        _kbrd_state.is_capslock ^= 1;
        return 1;
    case SCS1_LEFT_SHIFT_PRESSED:
        _kbrd_state.is_left_shift = 1;
        return 1;
    case SCS1_LEFT_SHIFT_RELEASED:
        _kbrd_state.is_left_shift = 0;
        return 1;
    case SCS1_LEFT_CONTROL_PRESSED:
        _kbrd_state.is_left_ctrl = 1;
        return 1;
    case SCS1_LEFT_CONTROL_RELEASED:
        _kbrd_state.is_left_ctrl = 0;
        return 1;
    case SCS1_RIGHT_SHIFT_PRESSED:
        _kbrd_state.is_right_shift = 1;
        return 1;
    case SCS1_RIGHT_SHIFT_RELEASED:
        _kbrd_state.is_right_shift = 0;
        return 1;
    }

    // Filter-out unsupported actions.
    const uint8_t ascii = _scs1_to_ascii[action];
    if (action >= sizeof(_scs1_to_ascii) || ascii == ASCII_INVALID)
    {
        // Just swallow.
        return 1;
    }

    // Not a special key.
    return 0;
}

static int _is_small_alpha(uint8_t ascii)
{
    if (ascii >= ASCII_CHAR_a && ascii <= ASCII_CHAR_z)
    {
        return 1;
    }

    return 0;
}

#define ASCII_CAPITAL_LETTER_OFFSET 32

/**
 * @brief This map converts double-char-keys SCS1 with SHIFT pressed.
 * 
 */
static const uint8_t _ascii_shift[] =
{
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_SYMBOL_QUOTATION, //done
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_SYMBOL_LESS, //done
    ASCII_SYMBOL_UNDERSCORE, //done
    ASCII_SYMBOL_GREATER, //done
    ASCII_SYMBOL_QUESTION, //done
    ASCII_SYMBOL_BRACKET_CLOSE, //done
    ASCII_SYMBOL_EXCLAMATION, //done
    ASCII_SYMBOL_AT, //done
    ASCII_SYMBOL_HASH, //done
    ASCII_SYMBOL_DOLAR, //done
    ASCII_SYMBOL_PERCENT, //done
    ASCII_SYMBOL_CARET, //done
    ASCII_SYMBOL_AMPERSAND, //done
    ASCII_SYMBOL_ASTERISK, //done
    ASCII_SYMBOL_BRACKET_OPEN, //done
    ASCII_INVALID,
    ASCII_SYMBOL_COLON, //done
    ASCII_INVALID,
    ASCII_SYMBOL_PLUS, //done
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_SYMBOL_CURLY_BRACKET_OPEN, //done
    ASCII_SYMBOL_PIPE, //done
    ASCII_SYMBOL_CURLY_BRACKET_CLOSE, //done
    ASCII_INVALID,
    ASCII_INVALID,
    ASCII_SYMBOL_TILDE, //done
    ASCII_CHAR_A, //done
    ASCII_CHAR_B, //done
    ASCII_CHAR_C, //done
    ASCII_CHAR_D, //done
    ASCII_CHAR_E, //done
    ASCII_CHAR_F, //done
    ASCII_CHAR_G, //done
    ASCII_CHAR_H, //done
    ASCII_CHAR_I, //done
    ASCII_CHAR_J, //done
    ASCII_CHAR_K, //done
    ASCII_CHAR_L, //done
    ASCII_CHAR_M, //done
    ASCII_CHAR_N, //done
    ASCII_CHAR_O, //done
    ASCII_CHAR_P, //done
    ASCII_CHAR_Q, //done
    ASCII_CHAR_R, //done
    ASCII_CHAR_S, //done
    ASCII_CHAR_T, //done
    ASCII_CHAR_U, //done
    ASCII_CHAR_V, //done
    ASCII_CHAR_W, //done
    ASCII_CHAR_X, //done
    ASCII_CHAR_Y, //done
    ASCII_CHAR_Z //done
};

/**
 * @brief Converts scan code to ASCII based on keyboard state.
 * 
 * @param action 
 * @return uint8_t 
 */
static uint8_t _kbrd_sc_to_ascii(uint8_t action)
{
    // We are sure that ascii here is printable as it was processed by _kbrd_process_special_key.
    const uint8_t ascii = _scs1_to_ascii[action];

    // Process SHIFT pressed.
    if (_kbrd_state.is_left_shift || _kbrd_state.is_right_shift)
    {
        const uint8_t shifted_ascii = _ascii_shift[ascii];
        if (shifted_ascii != ASCII_INVALID && ascii < sizeof(_ascii_shift))
        {
            return shifted_ascii;
        }
        sys_printf(
            "_kbrd_sc_to_ascii: SHIFT failed (shifted_ascii %#x, ascii %#x, sizeof(_ascii_shift) %uB.\n",
            shifted_ascii,
            ascii,
            sizeof(_ascii_shift)
        );
    }

    // Process CAPSLOCK pressed.
    if (_kbrd_state.is_capslock)
    {
        if (_is_small_alpha(ascii) != 0)
        {
            return _ascii_shift[ascii];
        }
    }

    return ascii;
}

uint8_t sys_dev_kbrd_get_ascii()
{
    uint8_t action = 0;

    // Process all special keys.
    do
    {
        action = sys_dev_ps2_poll_data();
    } while (_kbrd_process_special_key(action) != 0);

    return _kbrd_sc_to_ascii(action);
}

void sys_dev_kbrd_test()
{
    uint8_t data = 0;

    sys_printf("sys_dev_kbrd_test: start\n");
    do
    {
        data = sys_dev_kbrd_get_ascii();
        sys_printf("%c", data);
    } while (data != ASCII_ESCAPE);
    sys_printf("sys_dev_kbrd_test: finished\n");
}
