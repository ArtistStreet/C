#include "../lib.h"

bool isXJSRF(const char c) {
    return c == 's' or c == 'j' or c == 'x' or c == 'r' or c == 'f'; 
}

bool foundXJSRF = false;

bool updateTone(vector<string>& buffer, char newTone,
    const unordered_map<string, unordered_map<char, string>>& accentMap,
    const unordered_map<string, pair<string, char>>& reverseAccentMap) { // Found in reverseAccentMap
        
    for (int i = buffer.size() - 1; i >= 0; --i) {
        auto it = reverseAccentMap.find(buffer[i]);
        if (it != reverseAccentMap.end()) {
            const string& base = it->second.first;
            
            auto accentIt = accentMap.find(base);
            if (accentIt != accentMap.end()) {
                auto toneIt = accentIt->second.find(newTone);
                if (toneIt != accentIt->second.end()) {
                    buffer[i] = base;
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

string removeAccents(const std::string& input) {
    size_t u32_len;
    uint32_t* u32 = u8_to_u32(reinterpret_cast<const uint8_t*>(input.c_str()), input.size(), nullptr, &u32_len);
    if (!u32) return input;

    size_t norm_len;
    uint32_t* norm = u32_normalize(UNINORM_NFD, u32, u32_len, nullptr, &norm_len);
    free(u32);
    if (!norm) return input;

    std::u32string result;
    for (size_t i = 0; i < norm_len; ++i) {
        if (!(norm[i] >= 0x300 && norm[i] <= 0x36F)) { // Unicode combining diacritical marks range
            if (norm[i] == U'đ') result += U'd';
            else if (norm[i] == U'ă') result += U'a';
            else if (norm[i] == U'ư') result += U'u';
            else if (norm[i] == U'ơ') result += U'o';
            else
            result += norm[i];
        }
    }
    free(norm);

    size_t out_len;
    uint8_t* utf8 = u32_to_u8(reinterpret_cast<const uint32_t*>(result.c_str()), result.size(), nullptr, &out_len);
    if (!utf8) return input;

    std::string output(reinterpret_cast<char*>(utf8), out_len);
    free(utf8);
    return output;
}

vector<string> handleXJSRF(vector<string> &buffer, Display *display, char c, bool &found) {
    int countUTF = 0, index = 0;
    found = false;

    char temp = c;
    string dau = string(1, temp);
    auto x = checkToneExist.find(dau);
    if (x != checkToneExist.end()) {
        for (int i = buffer.size() - 1; i >= 0; i--) {
            for (int j = 0; j < (int)x->second.size(); j++) {
                if (buffer[i] == x->second[j]) {
                    cout << "Debug x: " << x->second[j] << endl;
                    buffer.push_back(string(1, c));
                    buffer[i] = removeAccents(buffer[i]);
                    cout << "Debug buffer: " << buffer[i];
                    isWrong = true;
                    for (int i = 0; i < (int)buffer.size(); i++) {
                        sendBackspace(display);
                    }
                    string temp = "";
                    for (auto &&i : buffer)
                    {
                        temp += i;
                    }

                    send_char(display, temp);
                    return buffer;
                }
            }
        }
    }

    if (!updateTone(buffer, c, accentMap, reverseAccentMap)) {
        for (int i = 0; i < (int)buffer.size(); i++) { // If found utf8
            const string& s = buffer[i];
            
            if ((unsigned char)s[0] > 127) {
                // cout << "Vao day a\n";
                found = true;
                countUTF++;
                index = i;
                // cout << "FOUND " << buffer[i];
                continue;
            }
            for (int j = 0; j < (int)priority.size(); j++) {
                if (buffer[i] == priority[j]){
                    found = true;
                    // cout << "FOUND " << buffer[i] << endl;
                    continue;
                }
            }
        }
    }
    if (found) {
        // cout << "Count " << countUTF << endl;
        if (countUTF == 0) {
            // cout << "ua\n";
            if ((int)buffer.size() <= 2) {
                for (int i = 0; i < 2; i++) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        foundXJSRF = true;
                        break;
                    }
                }
            }
            else if ((int)buffer.size() & 1) {
                for (int i = (int)buffer.size() / 2; i < (int)buffer.size(); i++) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        foundXJSRF = true;
                        break;
                    }
                }
            }
            else {
                for (int i = (int)buffer.size() / 2; i >= 0; i--) {
                    if (applyAccentAtIndex(buffer, i, c, accentMap)) {
                        foundXJSRF = true;
                        break;
                    }
                }
            }
        } 
        else if (countUTF > 0) {
            // auto it = accentMap.find(removeAccents(buffer[index]));
            auto it = accentMap.find((buffer[index]));
            if (it != accentMap.end()) {
                auto tranform = it->second.find(c);
                if (tranform != it->second.end()) {
                    buffer[index] = tranform->second;
                    foundXJSRF = true;
                }
            }
        }
        for (int i = 0; i < (int)buffer.size() + 1; i++) {
            sendBackspace(display);
        }
    
        string temp = "";
        for (auto &&i : buffer) {
            temp += i;
        }
        send_char(display, temp);
    }

    return buffer;
}