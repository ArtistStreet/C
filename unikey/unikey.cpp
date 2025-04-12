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

vector<string> buffer; // Change buffer to a single string instead of a vector<string>

std::unordered_map<string, std::string> telexMap = {
    {"a", "â"},
    {"d", "đ"},
    {"e", "ê"},
    {"o", "ô"}
};

unordered_map<string, string> telexMapW = {
    {"a", "ă"},
    {"o", "ơ"},
    {"u", "ư"}, 
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
    return c == 'a' || c == 'e' || c == 'o' || c == 'w' || c == 'd';
}

const std::vector<std::string> priority = {
    "â", "ê", "ô", "ă", "ơ", "ư", "u", "e", "a", "o", "i", "y"
};

std::vector<string> temp; 

std::string applyAccent(const std::string& word, char accent) {
    fflush(stdout);
    // cout << word;
    for (auto &&i : temp)
    {
        // cout << i << " ";
    }
    
    // for (int i = 0; i < buffer.size(); i++) {
    //     cout << temp[i] << " ";
    // }
    std::vector<std::string> chars;
    for (size_t i = 0; i < word.size();) {
        unsigned char c = word[i];
        // cout << word[i] << " ";
        size_t len = 1;
        if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        if (i + len <= word.size()) {
            std::string currentChar = word.substr(i, len);
            // cout << currentChar;
            // if (currentChar == "ư" || currentChar == "ê") {
            //     cout << "found";
            //     exit(0);
            // }
            chars.push_back(word.substr(i, len));
        } else {
            return word;
        }
        i += len;
    }

    std::vector<std::pair<std::string, int>> candidates;
    for (int i = 0; i < chars.size(); ++i) {
        const std::string& ch = chars[i];
        // cout << chars[i] << " ";
        auto it = accentMap.find(ch);
        if (it != accentMap.end() && it->second.count(accent)) {
            candidates.emplace_back(ch, i);
        }
    }
    fflush(stdout);
    // cout << 123;
    for (const auto& vowel : priority) {
        for (const auto& [ch, idx] : candidates) {
            if (ch == vowel) {
                chars[idx] = accentMap[ch][accent];

                std::string result;
                for (const auto& c : chars) result += c;
                // cout << result;
                return result;
            }
        }
    }

    return word;
}

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


#include <cstdio>  // for popen, fgets
#include <memory>  // for unique_ptr

void send_char(Display *display, const string& utf8_char) {
    // cout << "[send_char] utf8_char = \"" << utf8_char << "\"\n";

    string cmd = "echo -n \"" + utf8_char + "\" | xclip -selection clipboard";
    if (system(cmd.c_str()) != 0) {
        cerr << "Failed to copy to clipboard\n";
        return;
    }

    // Đọc lại clipboard để kiểm tra nội dung
    FILE* pipe = popen("xclip -o -selection clipboard", "r");
    if (!pipe) {
        cerr << "Failed to read clipboard\n";
    } else {
        char buffer[128];
        string clipboardContent;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            clipboardContent += buffer;
        }
        pclose(pipe);
        // cout << "[clipboard] = " << clipboardContent << endl;
    }

    usleep(1000); // Delay nhẹ

    // Simulate Ctrl+V
    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L); 
    KeyCode v = XKeysymToKeycode(display, XK_V); 

    XTestFakeKeyEvent(display, ctrl, True, 0); // Ctrl press
    XTestFakeKeyEvent(display, v, True, 0);    // V press
    XTestFakeKeyEvent(display, v, False, 0);   // V release
    XTestFakeKeyEvent(display, ctrl, False, 0);// Ctrl release

    XFlush(display);
}



// void handleBackspace() {
//     if (!buffer.empty()) {
//         // Handle UTF-8 multi-byte characters
//         unsigned char lastChar = buffer.back();
//         size_t len = 1;
//         if ((lastChar & 0xE0) == 0xC0) len = 2;        // 2-byte UTF-8
//         else if ((lastChar & 0xF0) == 0xE0) len = 3;   // 3-byte UTF-8
//         else if ((lastChar & 0xF8) == 0xF0) len = 4;   // 4-byte UTF-8

//         if (buffer.size() >= len) {
//             buffer.erase(buffer.end() - len, buffer.end());
//         }
//     }
// }

// std::vector<std::string> splitUTF8(const std::string& input) {
//     std::vector<std::string> result;
//     for (size_t i = 0; i < input.size();) {
//         unsigned char c = input[i];
//         size_t len = 1;
//         if ((c & 0xE0) == 0xC0) len = 2;
//         else if ((c & 0xF0) == 0xE0) len = 3;
//         else if ((c & 0xF8) == 0xF0) len = 4;

//         result.push_back(input.substr(i, len));
//         i += len;
//     }
//     return result;
// }


vector<string> handleTelexTransform(vector<string>& buffer, Display* display) {
    // if (buffer.size() < 2) return buffer;
    vector<string> result;
    string last = buffer.back();
    auto it = telexMap.find(last);
   
    int cnt = 1;
    string secondLast = buffer[buffer.size() - 2];
    // cout << "Last: " << last << ", Second Last: " << secondLast << endl;

    for (int i = buffer.size() - 2; i >= 0; --i) {
        if (buffer[i] == last) {
            // cout << "Last: " << last << ", Second Last: " << buffer[i] << endl;
            for (int j = 0; j < cnt; j++) {
                sendLeftArrow(display);
            }
            sendBackspace(display);
            send_char(display, it->second);
            result.push_back(it->second);
            break;
        }
        cnt++;
        result.push_back((buffer[i]));
    }

    reverse(result.begin(), result.end());
    for (int i = 0; i < result.size(); i++) {
        sendRightArrow(display);
    }

    if (cnt == buffer.size()) { // Neu khong tim thay ki tu truoc do da xuat hien thi can push lai ki tu do vao trong 
        result.push_back(last);
    } else { // Da xuat hien truoc do roi thi xoa ki tu cuoi cung
        sendBackspace(display);
    }

    // for (auto &&i : result)
    // {
    //     cout << i << " ";
    // }
    cout << endl;

    return result;
}

vector<string> handleTelexTransformW(vector<string>& buffer, Display* display) {
    vector<string> result;
    string last = buffer.back();
    int cnt = 1;
    int count = 0;
    bool transformed = false;

    for (int i = buffer.size() - 2; i >= 0; --i) {
        auto it = telexMapW.find(buffer[i]);
        if (it != telexMapW.end()) {
            count++;
            result.push_back(it->second);
            // transformed = true;
            buffer[i] = it->second; // cập nhật lại buffer
            continue;
        }
        cnt++;
        result.push_back(buffer[i]);
    }

    reverse(result.begin(), result.end());

    // for (int i = 0; i < result.size(); i++) {
    //     sendRightArrow(display);
    // }

    // if (!transformed) {
    //     result.push_back(last);
    // } else {
    //     sendBackspace(display); // xoá w
    // }
    // cout << count;
    // for (int i = 0; i < cnt; i++) {
    //     sendLeftArrow(display);
    // }
    for (int i = 0; i < buffer.size(); i++) {
        sendBackspace(display);
    }
    string temp = "";
    for (auto &&i : result)
    {
        temp += i;
    }
    send_char(display, temp);
    // if (result.size() != 1) {
    //     send_char(display, "ươ");
    // } else {
    //     send_char(display, result[0]);
    // }
    
   return result;
}


void handleCtrlBackspace(string &buffer) {
    // Remove the last word from the buffer
    size_t lastSpace = buffer.find_last_of(' ');
    if (lastSpace != string::npos) {
        buffer.erase(lastSpace);
    } else {
        buffer.clear(); // Clear the entire buffer if no spaces are found
    }
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
                // cout << c;

                // Check if space is pressed
                if (xievent->detail == XKeysymToKeycode(display, XK_space)) {
                    buffer.clear();
                    continue;
                }

                // Check if Ctrl + Backspace is pressed
                // if (xievent->detail == XKeysymToKeycode(display, XK_BackSpace) &&
                //     (xievent->mods.effective & ControlMask)) {
                //     handleCtrlBackspace(buffer); // Handle Ctrl + Backspace
                //     continue;
                // }

                // // Handle regular Backspace
                // if (xievent->detail == XKeysymToKeycode(display, XK_BackSpace)) {
                //     handleBackspace();
                //     continue;
                // }
                // fflush(stdout);

                if (isascii(c) && isprint(c)) {
                    buffer.push_back(string(1, c)); // Append character to the buffer
                    // cout << "Buffer: ";
                    // for (auto &&i : buffer)
                    // {
                    //     cout << i;
                    // }
                    // cout << endl;
                    
                    // fflush(stdout);

                    if (buffer.size() >= 2) {
                        if (telexMap.find(buffer.back()) != telexMap.end()) { // Xu li khi khong co w
                            buffer = handleTelexTransform(buffer, display);
                        } else if (c == 'w') { // Co w
                            // cout << 123;
                            buffer = handleTelexTransformW(buffer, display);
                            fflush(stdout);
                        }
                    }
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;
}