#include "lib.h"
#include "config.h"

vector<string> buffer; // Change buffer to a single string instead of a vector<string>

bool isAccentKey(char c) {
    return c == 'a' || c == 'e' || c == 'o' || c == 'w' || c == 'd';
}

bool found = false;


void handleReverseTelexTransform(vector<string> &buffer, char c) {
    if (buffer.empty()) return ;

    for (int i = buffer.size() - 1; i >= 0; i--) {
        auto it = reverseTelexTransform.find(buffer[i]);
        if (it != reverseTelexTransform.end()) {
            if (c == it->second.second) {
                buffer[i] = it->second.first;
            }
        }
    }
}

vector<string> handleTelexTransform(vector<string>& buffer, Display* display, char c, bool &found) {
    vector<string> result;
    string last = string(1, c);
    int count = 0;
    unordered_map<string, string>::iterator it;

    handleReverseTelexTransform(buffer, c);

    for (int i = (int)buffer.size() - 1; i >= 0; --i) {
        if (c == 'w') {
            it = telexMapW.find(buffer[i]);
            if (it != telexMapW.end()) {
                found = true;
                count++;
                result.push_back(it->second);
                continue;
            }
        }
        else {
            it = telexMap.find(last);
            if ((it != telexMap.end()) && (buffer[i] == last)) {
                found = true;
                count++;
                result.push_back(it->second);
                fflush(stdout);
                continue;
            }
        }
        result.push_back(buffer[i]);
    }

    if (found) {
        reverse(result.begin(), result.end());
        // result.pop_back();
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        if (result[result.size() - 1] == "ă" and result[result.size() - 2] == "ư") {
            result[result.size() - 1] = "a";
        }
        cout << "DEBUG ";
        for (auto &&i : result)
        {
            cout << i << " "; 
            temp += i;
        }
        send_char(display, temp);
        return result;
    }
    return buffer;
}

bool isXJSRF(const char c) {
    return c == 's' or c == 'j' or c == 'x' or c == 'r' or c == 'f'; 
}

bool foundXJSRF = false;

bool updateTone(vector<string>& buffer, char newTone,
    const unordered_map<string, unordered_map<char, string>>& accentMap,
    const unordered_map<string, pair<string, char>>& reverseAccentMap) {
    for (int i = buffer.size() - 1; i >= 0; --i) {
        auto it = reverseAccentMap.find(buffer[i]);
        if (it != reverseAccentMap.end()) {
            const string& base = it->second.first;

            auto accentIt = accentMap.find(base);
            if (accentIt != accentMap.end()) {
                auto toneIt = accentIt->second.find(newTone);
                if (toneIt != accentIt->second.end()) {
                    buffer[i] = toneIt->second;
                    cout << "Buffer[i] " << buffer[i] << endl;
                    // return buffer;
                    return true;
                }
            }
        }
    }
    return false;
}

bool applyAccentAtIndex(vector<string>& buffer, int index, char c,
    const unordered_map<string, unordered_map<char, string>>& accentMap) {
    if (index < 0 || index >= (int)buffer.size()) return false;

    auto it = accentMap.find(buffer[index]);
    if (it != accentMap.end()) {
        auto transform = it->second.find(c);
        if (transform != it->second.end()) {
        buffer[index] = transform->second;
        return true;
        }
    }
    return false;
}

unordered_map<string, pair<string, char>> reverseAccentMap;


vector<string> handleXJSRF(vector<string> &buffer, Display *display, char c, bool &found) {
    int8_t countUTF = 0, index = 0;
    found = false;

    if (!updateTone(buffer, c, accentMap, reverseAccentMap)) {
        for (int i = 0; i < (int)buffer.size(); i++) { // If found utf8
            const string& s = buffer[i];
            
            if ((unsigned char)s[0] > 127) {
                for (int j = 0; j < (int)priority.size(); j++) {
                    if (buffer[i] == priority[j]){
                        found = true;
                        countUTF++;
                        index = i;
                        cout << "FOUND " << buffer[i];
                        continue;
                    }
                }
            }
            for (int j = 0; j < (int)priority.size(); j++) {
                if (buffer[i] == priority[j]){
                    found = true;
                    cout << "FOUND " << buffer[i] << endl;
                    continue;
                }
            }
        }
    }
    if (found) {
        if (countUTF == 0) {
            if ((int)buffer.size() <= 2) {
                for (int i = 0; i < 2; i++) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        foundXJSRF = true;
                        break;
                    }
                }
            }
            else if ((int)buffer.size() & 1) {
                for (int i = (int)buffer.size() / 2 - 1; i < (int)buffer.size(); i++) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        cout << "Vao\n";
                        foundXJSRF = true;
                        break;
                    }
                }
            }
            else {
                for (int i = (int)buffer.size() / 2; i < (int)buffer.size(); i++) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        foundXJSRF = true;
                        break;
                    }
                }
            }
        } 
        else if (countUTF > 0) {
            auto it = accentMap.find(buffer[index]);
            if (it != accentMap.end()) {
                auto tranform = it->second.find(c);
                if (tranform != it->second.end()) {
                    buffer[index] = tranform->second;
                    foundXJSRF = true;
                }
            }
        } else {
    
        }
        cout << "Buffer size " << buffer.size() << endl; 
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
    
        string temp = "";
        for (auto &&i : buffer)
        {
            // cout << i;
            temp += i;
        }
        // cout << endl;
        // cout << "Temp" << temp << endl;
        
        send_char(display, temp);
    }
    else if (updateTone(buffer, c, accentMap, reverseAccentMap)) {
        found = true;
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        for (auto &&i : buffer)
        {
            // cout << i;
            temp += i;
        }
        // cout << endl;
        // cout << "Temp" << temp << endl;
        
        send_char(display, temp);
    }

    return buffer;
}


void buildReverseAccentMap(
    const unordered_map<string, unordered_map<char, string>>& accentMap,
    unordered_map<string, pair<string, char>>& reverseMap
) {
    for (const auto& [baseChar, toneMap] : accentMap) {
        for (const auto& [toneChar, composedChar] : toneMap) {
            reverseMap[composedChar] = {baseChar, toneChar};
        }
    }
}

bool isEnglish = false;
bool modeSwitchPressed = false;

// void daemonize() {
//     pid_t pid = fork();

//     if (pid < 0) exit(EXIT_FAILURE); // fork lỗi
//     if (pid > 0) exit(EXIT_SUCCESS); // thoát tiến trình cha

//     setsid(); // tạo session mới
//     umask(0); // bỏ hạn chế quyền
//     chdir("/"); // chuyển về root directory

//     // Đóng tất cả descriptor
//     close(STDIN_FILENO);
//     close(STDOUT_FILENO);
//     close(STDERR_FILENO);
// }

int main() {
    // loadConfig("config.ini");
    // if (!getConfigBool("enabled")) return 0;
    daemonize();
    // if (getConfigBool("show_notification")) {
    //     system("notify-send 'Gõ tiếng Việt đang chạy ngầm'");
    // }
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
    buildReverseAccentMap(accentMap, reverseAccentMap);
    int cnt = 0;
    int fixBackspace = 0;
    while (true) {
        XEvent ev; // Holds data for an event
        XNextEvent(display, &ev);
        if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == opcode) { // Handle event
            XGetEventData(display, &ev.xcookie); // Get data
            if (ev.xcookie.evtype == XI_FocusIn) {
                buffer.clear();
                cout << "[FocusIn] Reset buffer\n";
            }
            if (ev.xcookie.evtype == XI_ButtonPress) {
                buffer.clear();
                cout << "[Button Press] Reset buffer\n";
            }
            if (ev.xcookie.evtype == XI_KeyPress) { // Check if key press
                XIDeviceEvent* xievent = (XIDeviceEvent*)ev.xcookie.data; // Get keycode

                KeySym keysym = XkbKeycodeToKeysym(display, xievent->detail, 0, 0);
                bool isCtrl  = xievent->mods.effective & ControlMask;

                // Space or Ctrl + Backspace
                if (isCtrl && keysym == XK_BackSpace || (xievent->detail == XKeysymToKeycode(display, XK_space))) {
                    cnt = 0;
                    buffer.clear();
                    continue;
                }

                // Switch mode
                if (isCtrl && xievent->detail == XKeysymToKeycode(display, XK_Shift_L)) {
                    cout << (!isEnglish ? "English Mode\n" : "Vietnamese Mode\n");
                    isEnglish = !isEnglish;
                    cnt = 0;
                    buffer.clear();
                    continue;
                }

                // Ctrl
                if (xievent->mods.effective & ControlMask) {
                    cout << "Ctrl" << endl;
                    continue;
                }

                // Alt
                if (xievent->mods.effective & Mod1Mask) {
                    cout << "Alt" << endl;
                    continue;
                }

                // Shift
                if (xievent->mods.effective & ShiftMask) {
                    cout << "Shift" << endl;
                    continue;
                }
               
                // Win 
                if (xievent->mods.effective & Mod4Mask) {
                    cout << "Win" << endl;
                    continue;
                }

                // Caplock 
                if (xievent->mods.effective & LockMask) {
                    cout << "Caps" << endl;
                    continue;
                }

                // Handle regular Backspace
                if (xievent->detail == XKeysymToKeycode(display, XK_BackSpace)) {
                    cout << "Backspace" << endl;
                    handleBackspace(buffer);
                    continue;
                }

                char c = keycode_to_char(display, xievent->detail);

                if (isascii(c) && isprint(c) && isEnglish == false) {
                    cout << "O day\n";
                    cout << "Lan " << ++cnt << endl; 
                    cout << "Ki tu: " << c << endl; 

                    cout << "Buffer before: ";
                    for (auto &&i : buffer)
                    {
                        cout << i;
                    }
                    cout << endl;
                    bool foundBs = false;

                    if ((int)buffer.size() >= 1) {
                        if ((telexMap.find(string(1, c)) != telexMap.end()) || (c == 'w')) { // Xu li khi khong co w
                            buffer = handleTelexTransform(buffer, display, c, foundBs);
                            if (foundBs)
                                fixBackspace = (int)buffer.size();
                        } 
                        else if (isXJSRF(c)) {
                            buffer = handleXJSRF(buffer, display, c, foundBs);
                            if (foundBs){
                                fixBackspace = (int)buffer.size();
                            }
                        }
                    }  
                    if (foundBs == false) {
                        // cout << "FOUND \n"; 
                        buffer.push_back(string(1, c)); // Append character to the buffer
                    }

                    cout << "FIX " << fixBackspace << endl;
                    if (fixBackspace) 
                        for (int i = 0; i < fixBackspace + 1; i++) {
                            buffer.push_back("~");
                        }
                    fixBackspace = 0;

                    cout << "Buffer: ";
                    for (auto &&i : buffer)
                    {
                        cout << i;
                    }
                    cout << endl << endl;
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
    }
    
    XCloseDisplay(display);
    return 0;
}