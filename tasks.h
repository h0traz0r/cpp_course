#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <iostream>
#include <utility>
#include <cstring>

using CodePoint = uint32_t;

class UTF8String {

    char *data = nullptr;

    size_t size = 0;

    size_t capacity = 0;

public:

    UTF8String(  char * from_string) {

        size = strlen(from_string);
        capacity = size;
        data = new char[capacity];
        memcpy(data,from_string,size);

        // size_t string_length = strlen(from_string);
        //
        // if (string_length == 0)
        //     data = new char[0];
        // else
        //     data = new char[string_length];
        //
        // memcpy(data,from_string, sizeof(from_string));


    }

    UTF8String(std::vector<CodePoint>) {

    }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity)
            return;


        char * new_data = new char[new_cap];
        std::memcpy(new_data,data,size + 1);
        delete[] data;
        data = new_data;
        capacity = new_cap;

    }


    void append(const char * str) {

        size_t len = std::strlen(str);
        size_t needed = size + len + 1;

        if (needed > capacity)
            reserve(std::max(needed, capacity *2 ));


        std::memcpy(data + size,str,len + 1);

        size += len;

    }

    void append(uint32_t code_point) {
        char buf[5];

        int len;

        encode_utf8(code_point,buf,len);

        append(buf);



    }

    void encode_utf8(uint32_t cp, char* buf, int& len) {
        if (cp <= 0x7F) {
            buf[0] = cp;
            len = 1;
        } else if (cp <= 0x7FF) {
            buf[0] = 0xC0 | (cp >> 6);
            buf[1] = 0x80 | (cp & 0x3F);
            len = 2;
        } else if (cp <= 0xFFFF) {
            buf[0] = 0xE0 | (cp >> 12);
            buf[1] = 0x80 | ((cp >> 6) & 0x3F);
            buf[2] = 0x80 | (cp & 0x3F);
            len = 3;
        } else {
            buf[0] = 0xF0 | (cp >> 18);
            buf[1] = 0x80 | ((cp >> 12) & 0x3F);
            buf[2] = 0x80 | ((cp >> 6) & 0x3F);
            buf[3] = 0x80 | (cp & 0x3F);
            len = 4;
        }
        buf[len] = '\0';
    }





    ~UTF8String() {

    }

    UTF8String(const UTF8String& other) {

    }

    UTF8String& operator=(const UTF8String& other) {

    }

};
