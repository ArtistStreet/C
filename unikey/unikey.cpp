#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <vector>

using namespace std;

string buffer;

unordered_map<string, string> telexMap = {
    {"aw", "ă"}, {"aa", "â"}, {"dd", "đ"},
    {"ee", "ê"}, {"oo", "ô"}, {"ow", "ơ"},
    {"uw", "ư"}, {"uow", "ươ"}
};

std::unordered_map<std::string, std::unordered_map<char, std::string>> accentMap = {
    {"a", {{'s', "á"}, {'f', "à"}, {'r', "ả"}, {'x', "ã"}, {'j', "ạ"}}},
    {"ă", {{'s', "ắ"}, {'f', "ằ"}, {'r', "ẳ"}, {'x', "ẵ"}, {'j', "ặ"}}},
    {"â", {{'s', "ấ"}, {'f', "ầ"}, {'r', "ẩ"}, {'x', "ẫ"}, {'j', "ậ"}}},
    {"e", {{'s', "é"}, {'f', "è"}, {'r', "ẻ"}, {'x', "ẽ"}, {'j', "ẹ"}}},
    {"ê", {{'s', "ế"}, {'f', "ề"}, {'r', "ể"}, {'x', "ễ"}, {'j', "ệ"}}},
    {"i", {{'s', "í"}, {'f', "ì"}, {'r', "ỉ"}, {'x', "ĩ"}, {'j', "ị"}}},
    {"o", {{'s', "ó"}, {'f', "ò"}, {'r', "ỏ"}, {'x', "õ"}, {'j', "ọ"}}},
    {"ô", {{'s', "ố"}, {'f', "ồ"}, {'r', "ổ"}, {'x', "ỗ"}, {'j', "ộ"}}},
    {"ơ", {{'s', "ớ"}, {'f', "ờ"}, {'r', "ở"}, {'x', "ỡ"}, {'j', "ợ"}}},
    {"u", {{'s', "ú"}, {'f', "ù"}, {'r', "ủ"}, {'x', "ũ"}, {'j', "ụ"}}},
    {"ư", {{'s', "ứ"}, {'f', "ừ"}, {'r', "ử"}, {'x', "ữ"}, {'j', "ự"}}},
    {"y", {{'s', "ý"}, {'f', "ỳ"}, {'r', "ỷ"}, {'x', "ỹ"}, {'j', "ỵ"}}},
};


bool isAccentKey(char c) {
    return c == 's' || c == 'f' || c == 'r' || c == 'x' || c == 'j';
}

std::string applyAccent(const std::string& word, char accent) {
    std::vector<std::string> chars;

    // Decode UTF-8 characters
    for (size_t i = 0; i < word.size();) {
        unsigned char c = word[i];
        size_t len = 1;
        if ((c & 0xE0) == 0xC0) len = 2;        // 2-byte UTF-8
        else if ((c & 0xF0) == 0xE0) len = 3;   // 3-byte UTF-8
        else if ((c & 0xF8) == 0xF0) len = 4;   // 4-byte UTF-8

        if (i + len <= word.size()) { // Ensure valid substring
            chars.push_back(word.substr(i, len));
        } else {
            cerr << "Invalid UTF-8 sequence detected\n";
            return word; // Return original word if invalid sequence
        }
        i += len;
    }

    // Traverse backward to find a vowel for accent application
    for (int i = chars.size() - 1; i >= 0; --i) {
        const std::string& base = chars[i];
        auto it = accentMap.find(base);
        if (it != accentMap.end()) {
            auto mark = it->second.find(accent);
            if (mark != it->second.end()) {
                chars[i] = mark->second;

                // Reconstruct the string
                std::string result;
                for (const auto& ch : chars) result += ch;
                return result;
            }
        }
    }

    return word; // Return original word if no accent applied
}

char keycode_to_char(Display *display, unsigned int keycode) {
    KeySym keysym = XkbKeycodeToKeysym(display, keycode, 0, 0);
    if (keysym >= XK_space && keysym <= XK_asciitilde) {
        return static_cast<char>(keysym);
    }
    return '\0'; 
}

void sendBackspace(Display *display) {
    KeyCode kc = XKeysymToKeycode(display, XK_BackSpace); // Get backspace keycode
    XTestFakeKeyEvent(display, kc, True, 0); // Press
    XTestFakeKeyEvent(display, kc, False, 0); // Release
    XFlush(display);
}

void send_char(Display *display, const string& utf8_char) {
    string cmd = "echo -n \"" + utf8_char + "\" | xclip -selection clipboard"; // Put into clipboard
    if (system(cmd.c_str()) != 0) { // Check if system call fails
        cerr << "Failed to copy to clipboard\n";
        return;
    }
    usleep(10000);
    
    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L); 
    KeyCode v = XKeysymToKeycode(display, XK_V); 

    XTestFakeKeyEvent(display, ctrl, True, 0); // Press
    XTestFakeKeyEvent(display, v, True, 0); // Release
    XTestFakeKeyEvent(display, ctrl, False, 0); // Press
    XTestFakeKeyEvent(display, v, False, 0); // Release

    XFlush(display);
}

int main() {
    Display *display = XOpenDisplay(nullptr); // X11
    if (!display) {
        cerr << "Cannot open display. Ensure X server is running.\n";
        return 1;
    }

    Window root = DefaultRootWindow(display);
    int opcode, event, error;

    if (!XQueryExtension(display, "XInputExtension", &opcode, &event, &error)) {
        cerr << "XInputExtension not available\n";
        XCloseDisplay(display);
        return 1;
    }

    XIEventMask evmask; // Event type
    unsigned char mask[(XI_LASTEVENT + 7) / 8] = {0}; // Create byte array ceil(XI_Lastevent / 8)
    XISetMask(mask, XI_KeyPress); // Listen only for key press events
    evmask.deviceid = XIAllDevices; // Listen from all devices
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISelectEvents(display, root, &evmask, 1);

    cout << "Now running\n";

    while (true) {
        XEvent ev; // Holds data for an event
        XNextEvent(display, &ev);
        if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == opcode) { // Handle event
            XGetEventData(display, &ev.xcookie); // Get data
            if (ev.xcookie.evtype == XI_KeyPress) { // Check if key press
                XIDeviceEvent* xievent = (XIDeviceEvent*)ev.xcookie.data; // Get keycode
                char c = keycode_to_char(display, xievent->detail);
                if (c != '\0' && isascii(c)) {
                    buffer += c;
                    std::cout << "Buffer: " << buffer << "\n";

                    if (buffer.size() >= 2) { // Check single word
                        // cout << 132;
                        std::string last2 = buffer.substr(buffer.size() - 2);
                        auto it = telexMap.find(last2);
                        if (it != telexMap.end()) {
                            sendBackspace(display);
                            sendBackspace(display);
                            send_char(display, it->second);
                            buffer.erase(buffer.size() - 2);
                            buffer += it->second; 
                        }
                        else {
                            char last = buffer.back();
                            // if (std::string("sfrxj").find(last) != std::string::npos) {
                            //     std::string before = buffer.substr(0, buffer.size() - 1); // bỏ dấu
                            //     std::string accented = applyAccent(before, last);
                            //     if (accented != before) {
                            //         for (size_t i = 0; i < buffer.size(); ++i) sendBackspace(display); // xóa hết cả từ + dấu
                            //         send_char(display, accented);
                            //         buffer = accented; // gán lại buffer mới (không chứa dấu)
                            //     }

                            // }
                            cout << last;
                        }
                    }
                }
            }
            XFreeEventData(display, &ev.xcookie);
        }
    }
    
    XCloseDisplay(display);
    return 0;
}