#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <iostream>
#include <utility>
#include <cstring>
#include <vector>
#include <optional>

using CodePoint = uint32_t;

class UTF8String {

    char *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;

public:

    UTF8String() = default;

    UTF8String(const char* from_string) {
        size = strlen(from_string);
        capacity = size;
        data = new char[capacity];
        memcpy(data, from_string, size);
    }

    UTF8String(const UTF8String& other) {
        size = other.size;
        capacity = other.size;
        data = new char[capacity];
        memcpy(data, other.data, size);
    }

    UTF8String(std::vector<CodePoint> points) {
        for (CodePoint cp : points)
            append(cp);
    }

    ~UTF8String() {
        delete[] data;
    }

    UTF8String& operator=(const UTF8String& other) {
        if (this == &other) return *this;
        delete[] data;
        size = other.size;
        capacity = other.size;
        data = new char[capacity];
        memcpy(data, other.data, size);
        return *this;
    }

    bool operator==(const UTF8String& other) const {
        if (size != other.size) return false;
        if (size == 0) return true;
        return memcmp(data, other.data, size) == 0;
    }

    bool operator!=(const UTF8String& other) const {
        return !(*this == other);
    }

    UTF8String operator+(const UTF8String& other) const {
        UTF8String result = *this;
        result += other;
        return result;
    }

    void operator+=(const UTF8String& other) {
        reserve(size + other.size);
        memcpy(data + size, other.data, other.size);
        size += other.size;
    }

    explicit operator std::string() const {
        return std::string(data, size);
    }

    std::optional<uint8_t> operator[](size_t index) const {
        if (index >= size) return std::nullopt;
        return static_cast<uint8_t>(data[index]);
    }

    size_t get_byte_count() const { return size; }

    size_t get_point_count() const {
        size_t count = 0;
        for (size_t i = 0; i < size; i++) {
            if ((static_cast<uint8_t>(data[i]) & 0xC0) != 0x80)
                count++;
        }
        return count;
    }

    std::optional<CodePoint> nth_code_point(size_t n) const {
        size_t count = 0;
        size_t i = 0;
        while (i < size) {
            uint8_t byte = static_cast<uint8_t>(data[i]);
            CodePoint cp;
            int seq_len;

            if (byte <= 0x7F) {
                cp = byte;
                seq_len = 1;
            } else if ((byte & 0xE0) == 0xC0) {
                cp = byte & 0x1F;
                seq_len = 2;
            } else if ((byte & 0xF0) == 0xE0) {
                cp = byte & 0x0F;
                seq_len = 3;
            } else {
                cp = byte & 0x07;
                seq_len = 4;
            }

            for (int j = 1; j < seq_len; j++)
                cp = (cp << 6) | (static_cast<uint8_t>(data[i + j]) & 0x3F);

            if (count == n) return cp;
            count++;
            i += seq_len;
        }
        return std::nullopt;
    }

    void append(char c) {
        append(static_cast<CodePoint>(static_cast<uint8_t>(c)));
    }

    void append(CodePoint cp) {
        char buf[4];
        int len = encode_utf8(cp, buf);
        if (size + len > capacity)
            reserve(std::max(size + len, capacity * 2));
        memcpy(data + size, buf, len);
        size += len;
    }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity) return;
        char* new_data = new char[new_cap];
        if (size > 0)
            memcpy(new_data, data, size);
        delete[] data;
        data = new_data;
        capacity = new_cap;
    }

private:

    static int encode_utf8(CodePoint cp, char* buf) {
        if (cp <= 0x7F) {
            buf[0] = cp;
            return 1;
        } else if (cp <= 0x7FF) {
            buf[0] = 0xC0 | (cp >> 6);
            buf[1] = 0x80 | (cp & 0x3F);
            return 2;
        } else if (cp <= 0xFFFF) {
            buf[0] = 0xE0 | (cp >> 12);
            buf[1] = 0x80 | ((cp >> 6) & 0x3F);
            buf[2] = 0x80 | (cp & 0x3F);
            return 3;
        } else {
            buf[0] = 0xF0 | (cp >> 18);
            buf[1] = 0x80 | ((cp >> 12) & 0x3F);
            buf[2] = 0x80 | ((cp >> 6) & 0x3F);
            buf[3] = 0x80 | (cp & 0x3F);
            return 4;
        }
    }
};
