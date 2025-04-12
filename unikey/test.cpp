#include <iostream>
#include <vector>
#include <string>

using namespace std;

std::vector<std::string> splitUTF8(const std::string& input) {
    std::vector<std::string> result;
    for (size_t i = 0; i < input.size();) {
        unsigned char c = input[i];
        size_t len = 1;
        if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        result.push_back(input.substr(i, len));
        i += len;
    }
    return result;
}

vector<string> handleString(string str) {
    char last = str.back();
    vector<string> res;
    // string temp = "";
    for (int i = str.size() - 2; i >= 0; i--) {
        if (last == str[i]) {
            // str.pop_back();
            res.push_back("â");
            continue;
        }
        res.push_back(string(1, str[i]));  // Sửa lại để chuyển `char` thành `string`
    }
    return res;
}

int main() {
    std::string input = "âêô";
    std::vector<std::string> result = splitUTF8(input);
    
    // Chạy handleString trên chuỗi "ana"
    vector<string> handle = handleString("ana");
    string x = "";
    // In kết quả từ handleString
    for (auto it = handle.rbegin(); it != handle.rend(); ++it) {
        x += *it;
    }

    cout << x;

    // Bạn có thể sử dụng đoạn mã sau nếu muốn thay thế ký tự trong chuỗi UTF-8:
    // for (const auto& part : result) {
    //     if (part == "ê") {
    //         cout << "ế";
    //     }
    // }

    cout << endl;
    return 0;
}


// cout << 123;
                        // char last = buffer.back();
                        // std::cout << buffer << " ";
                        // if (std::string("sfrxj").find(last) != std::string::npos) {
                        //     std::string modifiedBuffer = applyAccent(buffer, last);
                        //     if (modifiedBuffer != buffer) {
                        //         // Remove the original characters from the display
                        //         for (size_t i = 0; i < buffer.length(); ++i) {
                        //             sendBackspace(display);
                        //         }
                        //         for (int i = modifiedBuffer.size() - 1; i >= 0; i--) {
                        //             if (modifiedBuffer[i] == last) {
                        //                 modifiedBuffer.erase(i, 1);
                        //                 break;
                        //             }
                        //         }
                        //         // Send the modified buffer (with the applied accent)
                        //         send_char(display, modifiedBuffer);

                        //         // Update the buffer to reflect the modified content
                        //         buffer = modifiedBuffer;
                        //     }
                        // }
                        //  else 
                        //  if (isAccentKey(buffer.back())) {

 // if (buffer.size() > 1) {
    //     string secondLast = buffer[buffer.size() - 2];
    //     cout << "Last: " << last << ", Second Last: " << secondLast << endl;
    //     if (last == secondLast) {
    //         sendBackspace(display);
    //         sendBackspace(display);
    //         send_char(display, it->second);
    //         buffer.pop_back();
    //         buffer.pop_back();
    //         buffer.push_back(it->second);
    //     }
    //     return buffer;
    // }