#include "lib.h"

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
    // cout << "Ki tu" <<  utf8_char;
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (!pipe) {
        cerr << "Failed to open xclip for writing\n";
        return;
    }

    fwrite(utf8_char.c_str(), sizeof(char), utf8_char.size(), pipe);
    pclose(pipe);

    // usleep(10000); // 10ms

    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L); 
    KeyCode v = XKeysymToKeycode(display, XK_V); 

    XTestFakeKeyEvent(display, ctrl, True, 0); // Ctrl down
    XTestFakeKeyEvent(display, v, True, 0);    // V down
    XTestFakeKeyEvent(display, v, False, 0);   // V up
    XTestFakeKeyEvent(display, ctrl, False, 0);// Ctrl up

    XFlush(display);
}

void handleCtrlBackspace(vector<string> &buffer) {
    // Remove the last word from the buffer
    // size_t lastSpace = buffer.find_last_of(' ');
    // if (lastSpace != string::npos) {
    //     buffer.erase(lastSpace);
    // } else {
        buffer.clear(); // Clear the entire buffer if no spaces are found
    // }
}

void handleBackspace(vector<string>& buffer) {
    if (!buffer.empty()) {
        buffer.pop_back();
    }
}