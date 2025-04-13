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


void send_char(Display *display, const string& utf8_char) {
    // Dùng popen để truyền UTF-8 an toàn hơn
    // cout << "Ki tu" <<  utf8_char;
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (!pipe) {
        cerr << "Failed to open xclip for writing\n";
        return;
    }

    fwrite(utf8_char.c_str(), sizeof(char), utf8_char.size(), pipe);
    pclose(pipe);

    // Thêm delay nhẹ để đảm bảo xclip xong
    // usleep(10000); // 10ms

    // Dán bằng cách mô phỏng Ctrl+V
    KeyCode ctrl = XKeysymToKeycode(display, XK_Control_L); 
    KeyCode v = XKeysymToKeycode(display, XK_V); 

    XTestFakeKeyEvent(display, ctrl, True, 0); // Ctrl down
    XTestFakeKeyEvent(display, v, True, 0);    // V down
    XTestFakeKeyEvent(display, v, False, 0);   // V up
    XTestFakeKeyEvent(display, ctrl, False, 0);// Ctrl up

    XFlush(display);
    // usleep(10000); // Đợi dán xong
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

bool found = false;

vector<string> handleTelexTransformW(vector<string>& buffer, Display* display, char c) {
    // for (auto &&i : buffer)
    // {
    //     cout << i;
    // }
    // cout << endl;
    vector<string> result;
    string last = string(1, c);
    // int cnt = 1;
    int count = 0;
    // bool transformed = false;
    unordered_map<string, string>::iterator it;

    for (int i = buffer.size() - 1; i >= 0; --i) {
        if (c == 'w') {
            it = telexMapW.find(buffer[i]);
            if (it != telexMapW.end()) {
                found = true;
                count++;
                result.push_back(it->second);
                // cout << it->second;
                // fflush(stdout);
                // transformed = true;
                // buffer[i] = it->second; // cập nhật lại buffer
                continue;
            }
        }
        else {
            it = telexMap.find(last);
            if ((it != telexMap.end()) && (buffer[i] == last)) {
                found = true;
                count++;
                result.push_back(it->second);
                // cout << it->second;
                // cout << "tim thay\n";
                fflush(stdout);
                // transformed = true;
                // buffer[i] = it->second; // cập nhật lại buffer
                break;
            }
        }
        // cnt++;
        result.push_back(buffer[i]);
    }

    // for (auto &&i : buffer)
    // {
    //     cout << i;
    //     fflush(stdout);
    // }
    // cout << endl;
    if (found) {
        reverse(result.begin(), result.end());
        // cout << "vao\n";
        // if (!transformed) {
        //     result.push_back(last);
        // } else {
        //     sendBackspace(display); // xoá w
        // }
        // cout << buffer.size();
        // if (c == 'w') {
        //     result.pop_back();
        // }
        cout << "Buffer size: " << buffer.size() << endl;
        for (int i = 0; i < buffer.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        // for (auto &&i : buffer)
        // {
        //     cout << i;
        //     fflush(stdout);
        // }
        // cout << " ";
        // cout << "res ";
        for (auto &&i : result)
        {
            // cout << i;
            // fflush(stdout);
            temp += i;
        }
        // cout << result.size() << " ";
        send_char(display, temp);
        // exit(0);
        // cout << temp;
        // fflush(stdout);
        buffer = result;
        return result;
    }
    return buffer;
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
    int cnt = 0;
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

                if (isascii(c) && isprint(c) && c != 'v') {
                    // cout << "lan " << cnt++; 
                    // cout << "ki tu: " << c << endl; 

                    // cout << "Buffer before: ";
                    // for (auto &&i : buffer)
                    // {
                    //     cout << i;
                    // }
                    // cout << endl;
                  
                    // cout << endl;
                    
                    // fflush(stdout);
                    found = false;

                    if (buffer.size() >= 1) {
                        if ((telexMap.find(string(1, c)) != telexMap.end()) || (c == 'w')) { // Xu li khi khong co w
                            buffer = handleTelexTransformW(buffer, display, c);
                        } 
                    }  if (found == false){
                        cout << "k thay\n";
                        buffer.push_back(string(1, c)); // Append character to the buffer
                    }
                    // cout << "Buffer: ";
                    // for (auto &&i : buffer)
                    // {
                    //     cout << i;
                    // }
                    // cout << endl;
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;
}