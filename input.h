#pragma once
#include <unordered_map>

using namespace std;

class Input {
    unordered_map<char, bool> keys_pressed;
public:
    Input() = default;
    void on_key_down(char ch) {
        keys_pressed[ch] = true;
    }
    void on_key_up(char ch) {
        keys_pressed[ch] = false;
    }
    bool get_key(char ch) {
        auto& k = keys_pressed.find(ch);
        if (k == keys_pressed.end()) {
            return false;
        }
        return k->second;
    }
};