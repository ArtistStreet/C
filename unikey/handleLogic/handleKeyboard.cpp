#include "../lib.h"

char keycode_to_char(Display *display, unsigned int keycode) {
    KeySym keysym = XkbKeycodeToKeysym(display, keycode, 0, 0);
    if (keysym >= XK_space && keysym <= XK_asciitilde) {
        return static_cast<char>(keysym);
    }
    return '\0'; 
}


void sendLeftArrow(Display *display) {
    KeyCode kc = XKeysymToKeycode(display, XK_Left); // Get backspace keycode
    XTestFakeKeyEvent(display, kc, True, 0); // Press
    XTestFakeKeyEvent(display, kc, False, 0); // Release
    XFlush(display);
}

void sendRightArrow(Display *display) {
    KeyCode kc = XKeysymToKeycode(display, XK_Right); // Get backspace keycode
    XTestFakeKeyEvent(display, kc, True, 0); // Press
    XTestFakeKeyEvent(display, kc, False, 0); // Release
    XFlush(display);
}

void sendBackspace(Display *display) {
    KeyCode kc = XKeysymToKeycode(display, XK_BackSpace); // Get backspace keycode
    XTestFakeKeyEvent(display, kc, True, 0); // Press
    XTestFakeKeyEvent(display, kc, False, 0); // Release
    XFlush(display);
}

void sendEnd(Display *display) {
    KeyCode kc = XKeysymToKeycode(display, XK_End); // Get backspace keycode
    XTestFakeKeyEvent(display, kc, True, 0); // Press
    XTestFakeKeyEvent(display, kc, False, 0); // Release
    XFlush(display);
}

void sendCtrlBackspace(Display *display) {
    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L);
    KeyCode space = XKeysymToKeycode(display, XK_BackSpace);
    XTestFakeKeyEvent(display, ctrl, True, 0);
    XTestFakeKeyEvent(display, space, True, 0);
    XTestFakeKeyEvent(display, space, False, 0);
    XTestFakeKeyEvent(display, ctrl, False, 0);
    XFlush(display);
}

void sendShiftLeft(Display *display) {
    KeyCode shift = XKeysymToKeycode(display, XK_Shift_L);
    KeyCode left = XKeysymToKeycode(display, XK_Left);
    XTestFakeKeyEvent(display, shift, True, 0);
    XTestFakeKeyEvent(display, left, True, 0);
    XTestFakeKeyEvent(display, left, False, 0);
    XTestFakeKeyEvent(display, shift, False, 0);
    XFlush(display);
}

void sendControlLeft(Display *display) {
    KeyCode control = XKeysymToKeycode(display, XK_Control_L);
    KeyCode shift = XKeysymToKeycode(display, XK_Shift_L);
    KeyCode left = XKeysymToKeycode(display, XK_Left);
    XTestFakeKeyEvent(display, control, True, 0);
    XTestFakeKeyEvent(display, shift, True, 0);
    XTestFakeKeyEvent(display, left, True, 0);
    XTestFakeKeyEvent(display, left, False, 0);
    XTestFakeKeyEvent(display, control, False, 0);
    XTestFakeKeyEvent(display, shift, False, 0);
    XFlush(display);
}

void sendCtrlA(Display* display) {
    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L);
    KeyCode a = XKeysymToKeycode(display, XK_a);

    XTestFakeKeyEvent(display, ctrl, True, 0);
    XTestFakeKeyEvent(display, a, True, 0);
    XTestFakeKeyEvent(display, a, False, 0);
    XTestFakeKeyEvent(display, ctrl, False, 0);
    XFlush(display);
}

void clearWithCtrlA(Display* display) {
    sendCtrlA(display);       // Select all
    sendBackspace(display);   // Delete selection
}

void send_char(Display *display, const string& utf8_char) {
    cout << "Ki tu" <<  utf8_char;
    FILE* pipe = popen("xclip -selection clipboard", "w");
    // FILE* pipe = popen("xclip -selection primary", "w");

    if (!pipe) {
        cerr << "Failed to open xclip for writing\n";
        return;
    }

    fwrite(utf8_char.c_str(), sizeof(char), utf8_char.size(), pipe);
    pclose(pipe);

    usleep(3000); // 10ms

    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L); 
    KeyCode v = XKeysymToKeycode(display, XK_V); 

    XTestFakeKeyEvent(display, ctrl, True, 0); // Ctrl down
    XTestFakeKeyEvent(display, v, True, 0);    // V down
    XTestFakeKeyEvent(display, v, False, 0);   // V up
    XTestFakeKeyEvent(display, ctrl, False, 0);// Ctrl up

    XFlush(display);
}

void handleCtrlBackspace(vector<string> &buffer) {
    buffer.clear();
    isWrong = false; // Clear the entire buffer if no spaces are found
}

void handleBackspace(vector<string>& buffer) {
    if (!buffer.empty()) {
        buffer.pop_back();
    }
}

bool handleModifiersAndControlKeys(XIDeviceEvent* xievent, Display* display, std::vector<std::string>& buffer, bool& isEnglish) {
    KeySym keysym = XkbKeycodeToKeysym(display, xievent->detail, 0, 0);
    bool isCtrl = xievent->mods.effective & ControlMask;

    // Space or Ctrl + Backspace → reset buffer
    if ((isCtrl && keysym == XK_BackSpace) || xievent->detail == XKeysymToKeycode(display, XK_space)) {
        // std::cout << "Space or Ctrl+Backspace\n";
        buffer.clear();
        isWrong = false;
        return true;
    }

    if (xievent->detail == XKeysymToKeycode(display, XK_KP_Enter)) {
        // std::cout << "Enter\n";
        buffer.clear();
        isWrong = false;
        return true;
    }

    // Toggle mode (commented out, optional)
    if (isCtrl && xievent->detail == XKeysymToKeycode(display, XK_Shift_L)) {
        (!isEnglish ? system("notify-send 'English'") : system("notify-send 'Vietnam'"));
        isEnglish = !isEnglish;
        buffer.clear();
        isWrong = false;
        return true;
    }

    // Modifier keys – just skip them
    if (xievent->mods.effective & ControlMask) {
        // std::cout << "Ctrl\n";
        return true;
    }

    if (xievent->mods.effective & Mod1Mask) {
        // std::cout << "Alt\n";
        return true;
    }

    if (xievent->mods.effective & ShiftMask) {
        // std::cout << "Shift\n";
        return true;
    }

    if (xievent->mods.effective & Mod4Mask) {
        // std::cout << "Win\n";
        return true;
    }

    if (xievent->mods.effective & LockMask) {
        // std::cout << "Caps Lock\n";
        return true;
    }

    // Backspace
    if (xievent->detail == XKeysymToKeycode(display, XK_BackSpace)) {
        // std::cout << "Backspace\n";
        handleBackspace(buffer);
        return true;
    }

    return false; // not handled
}