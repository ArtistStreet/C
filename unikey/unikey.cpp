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

unordered_map<string, string> telexMap = {
    {"a", "â"},
    {"d", "đ"},
    {"e", "ê"},
    {"o", "ô"}
};

unordered_map<string, string> telexMapReverse = {
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

unordered_map<string, string> telexMapWReverse = {
    {"a", "ă"},
    {"o", "ơ"},
    {"u", "ư"}, 
};

vector<unordered_map<char, string>> accentMapList = {
    {{'s', "á"}, {'f', "à"}, {'r', "ả"}, {'x', "ã"}, {'j', "ạ"}},
    {{'s', "ắ"}, {'f', "ằ"}, {'r', "ẳ"}, {'x', "ẵ"}, {'j', "ặ"}},
    {{'s', "ấ"}, {'f', "ầ"}, {'r', "ẩ"}, {'x', "ẫ"}, {'j', "ậ"}},
    {{'s', "é"}, {'f', "è"}, {'r', "ẻ"}, {'x', "ẽ"}, {'j', "ẹ"}},
    {{'s', "ế"}, {'f', "ề"}, {'r', "ể"}, {'x', "ễ"}, {'j', "ệ"}},
    {{'s', "í"}, {'f', "ì"}, {'r', "ỉ"}, {'x', "ĩ"}, {'j', "ị"}},
    {{'s', "ó"}, {'f', "ò"}, {'r', "ỏ"}, {'x', "õ"}, {'j', "ọ"}},
    {{'s', "ố"}, {'f', "ồ"}, {'r', "ổ"}, {'x', "ỗ"}, {'j', "ộ"}},
    {{'s', "ớ"}, {'f', "ờ"}, {'r', "ở"}, {'x', "ỡ"}, {'j', "ợ"}},
    {{'s', "ú"}, {'f', "ù"}, {'r', "ủ"}, {'x', "ũ"}, {'j', "ụ"}},
    {{'s', "ứ"}, {'f', "ừ"}, {'r', "ử"}, {'x', "ữ"}, {'j', "ự"}},
    {{'s', "ý"}, {'f', "ỳ"}, {'r', "ỷ"}, {'x', "ỹ"}, {'j', "ỵ"}},
};


bool isAccentKey(char c) {
    return c == 'a' || c == 'e' || c == 'o' || c == 'w' || c == 'd';
}

bool checkDau(char c) {
    return c == 's' or c == 'r' or c == 'j' or c == 'x' or c == 'f'; 
}

const vector<string> priority = {
    "â", "ê", "ô", "ă", "ơ", "ư", "u", "e", "a", "o", "i", "y"
};

vector<string> temp; 



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
    // usleep(10000); // Đợi dán xong
}

void handleBackspace() {
    if (!buffer.empty()) {
        buffer.pop_back();
    }
}

bool found = false;


vector<string> handleTelexTransform(vector<string>& buffer, Display* display, char c) {
    vector<string> result;
    string last = string(1, c);
    unordered_map<string, string>::iterator it;
    bool check_uo = false;

    if (c == 'w') {
        int cnt = 0;
        for (int i = 0; i < buffer.size(); i++) {
            if (buffer[i] == "o" or buffer[i] == "u") {
                cnt++;
            }
        }
        if (cnt == 2) {
            check_uo = true;
        }
    }

    for (int i = buffer.size() - 1; i >= 0; --i) {
        if (c == 'w') {
            it = telexMapW.find(buffer[i]);
            if (it != telexMapW.end() && (check_uo || buffer.size() <= 1)) {
                found = true;
                result.push_back(it->second);
                continue;
            }
            check_uo = true;
        }
        else {
            it = telexMap.find(last);
            if ((it != telexMap.end()) && (buffer[i] == last)) {
                found = true;
                result.push_back(it->second);
                break;
            }
        }
        result.push_back(buffer[i]);
    }

    if (found) {
        reverse(result.begin(), result.end());
        for (int i = 0; i < result.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        for (auto &&i : result)
        {
            temp += i;
        }
        send_char(display, temp);
        return result;
    }
    return buffer;
}

vector<string> applyAccent(vector<string>& buffer, Display *display, char c) {
    vector<string> result;
    string last = string(1, c);

    const vector<string> priority = {
        "â", "ê", "ô", "ă", "ơ", "ư", "u", "e", "a", "o", "i", "y"
    };

    int highest = 100;

    for (int i = buffer.size() - 1; i >= 0; i--) {
        for (int j = 0; j < priority.size(); j++) {
            if (buffer[i] == priority[j]) {
                highest = min(highest, j);
            }
        }
    }

    cout << highest;
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
    XISetMask(mask, XI_FocusIn);
    XISetMask(mask, XI_ButtonPress);
    evmask.deviceid = XIAllDevices; // Listen from all devices
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISelectEvents(display, root, &evmask, 1);

    cout << "Now running\n";
    // int cnt = 0;
    while (true) {
        XEvent ev; // Holds data for an event
        XNextEvent(display, &ev);
        if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == opcode) { // Handle event
            XGetEventData(display, &ev.xcookie); // Get data
            // if (ev.xcookie.evtype == XI_FocusIn) {
            //     buffer.clear();
            //     cout << "[FocusIn] Reset buffer\n";
            // }
            if (ev.xcookie.evtype == XI_ButtonPress) {
                buffer.clear();
                cout << "[Mouse click] Reset buffer\n";
            }
            else if (ev.xcookie.evtype == XI_KeyPress) { // Check if key press
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

                // Handle regular Backspace
                if (xievent->detail == XKeysymToKeycode(display, XK_BackSpace)) {
                    handleBackspace();
                    continue;
                }
                // fflush(stdout);

                if (isascii(c) && isprint(c) && c != 'v') {
                    // cout << "lan " << cnt++; 
                    // cout << "ki tu: " << c << endl; 

                    cout << "Buffer before: ";
                    for (auto &&i : buffer)
                    {
                        cout << i;
                    }
                    cout << endl;
                  
                    // cout << endl;
                    
                    // fflush(stdout);
                    found = false;

                    if (buffer.size() >= 1) {
                        if ((telexMap.find(string(1, c)) != telexMap.end()) || (c == 'w')) { // Xu li khi khong co w
                            vector<string> temp;
                            buffer = handleTelexTransform(buffer, display, c);
                            // for (auto &&i : temp)
                            // {
                            //     buffer.push_back(i);
                            // }
                            buffer.push_back(string(1, c));
                            
                        } 
                        // else if (checkDau(c)) {
                        //     buffer = applyAccent(buffer, display, c);
                        // }
                    }  if (found == false){
                        // cout << "k thay\n";
                        buffer.push_back(string(1, c)); // Append character to the buffer
                    }
                    cout << "Buffer: ";
                    for (auto &&i : buffer)
                    {
                        cout << i;
                    }
                    cout << endl;
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;
}