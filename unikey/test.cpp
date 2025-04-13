#include "lib.h"

vector<string> buffer; // Change buffer to a single string instead of a vector<string>

bool isAccentKey(char c) {
    return c == 'a' || c == 'e' || c == 'o' || c == 'w' || c == 'd';
}

bool found = false;

vector<string> handleTelexTransform(vector<string>& buffer, Display* display, char c, bool &found) {
    vector<string> result;
    string last = string(1, c);
    int count = 0;
    unordered_map<string, string>::iterator it;

    // for (int i = 0; i < (int)buffer.size(); i++) {
    //     if (buffer[i] == "u" or buffer[i] == "o") {
    //         count++;
    //     }
    // }

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
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
        string temp = "";
        if (result[result.size() - 1] == "ă" and result[result.size() - 2] == "ư") {
            result[result.size() - 1] = "a";
        }
        for (auto &&i : result)
        {
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

vector<string> handleXJSRF(vector<string> &buffer, Display *display, char c) {
    vector<string> result;
    string last = string(1, c);
    int8_t countUTF = 0, index = 0;
    bool foundChar = false;

    for (int i = 0; i < (int)buffer.size(); i++) { // If found utf8
        const string& s = buffer[i];
        
        if ((unsigned char)s[0] > 127) {
            for (int j = 0; j < (int)priority.size(); j++) {
                if (buffer[i] == priority[j]){
                    foundChar = true;
                    countUTF++;
                    index = i;
                    cout << "FOUND " << buffer[i];
                    continue;
                }
            }
        }
        for (int j = 0; j < (int)priority.size(); j++) {
            if (buffer[i] == priority[j]){
                foundChar = true;
                countUTF++;
                index = i;
                cout << "FOUND " << buffer[i];
                continue;
            }
        }
    }

    if (foundChar) {
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
                        fflush(stdout);
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
                    // result.push_back(tranform->second);
                    buffer[index] = tranform->second;
                    foundXJSRF = true;
                }
            }
        } else {
    
        }
    
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
    
        string temp = "";
        for (auto &&i : buffer)
        {
            // cout << i;
            temp += i;
        }
        cout << endl;
        // cout << "Temp" << temp << endl;
        
        send_char(display, temp);
    }

    return buffer;
}

unordered_map<string, pair<string, char>> reverseAccentMap;

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
    buildReverseAccentMap(accentMap, reverseAccentMap);
    // for (const auto& [composedChar, baseAndTone] : reverseAccentMap) {
    //     const string& base = baseAndTone.first;
    //     char tone = baseAndTone.second;
    //     cout << "Composed: " << composedChar
    //          << " => Base: " << base
    //          << ", Tone: " << tone << endl;
    // }
    
    int cnt = 0;
    int fixBackspace = 0;
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

                // Check if space is pressed
                if (xievent->detail == XKeysymToKeycode(display, XK_space)) {
                    cnt = 0;
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
                    cout << "Backspace" << endl;
                    handleBackspace(buffer);
                    continue;
                }

                if (isascii(c) && isprint(c) && c != 'v') {
                    cout << "Lan " << cnt++ << endl; 
                    cout << "Ki tu: " << c << endl; 

                    cout << "Buffer before: ";
                    for (auto &&i : buffer)
                    {
                        cout << i;
                    }
                    cout << endl;
                    found = false;
                    foundXJSRF = false;

                    if ((int)buffer.size() >= 1) {
                        if ((telexMap.find(string(1, c)) != telexMap.end()) || (c == 'w')) { // Xu li khi khong co w
                            bool foundBs = false;
                            buffer = handleTelexTransform(buffer, display, c, foundBs);
                            if (foundBs)
                                fixBackspace = (int)buffer.size();
                        } 
                        else if (isXJSRF(c)) {
                            buffer = handleXJSRF(buffer, display, c);
                        }
                    }  if (found == false && foundXJSRF == false){
                        buffer.push_back(string(1, c)); // Append character to the buffer
                    }
                    for (int i = 0; i < fixBackspace; i++) {
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