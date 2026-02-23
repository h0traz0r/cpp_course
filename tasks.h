#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <optional>

using CodePoint = uint32_t;

class UTF8String {

    char *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;

    static int encode_utf8(CodePoint cp, char* buf);

public:

    UTF8String() = default;
    UTF8String(const char* from_string);
    UTF8String(const UTF8String& other);
    UTF8String(std::vector<CodePoint> points);
    ~UTF8String();

    UTF8String& operator=(const UTF8String& other);
    bool operator==(const UTF8String& other) const;
    bool operator!=(const UTF8String& other) const;
    UTF8String operator+(const UTF8String& other) const;
    void operator+=(const UTF8String& other);
    explicit operator std::string() const;
    std::optional<uint8_t> operator[](size_t index) const;

    size_t get_byte_count() const;
    size_t get_point_count() const;
    std::optional<CodePoint> nth_code_point(size_t n) const;

    void append(char c);
    void append(CodePoint cp);
    void reserve(size_t new_cap);
};
