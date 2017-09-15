#pragma once

#include <cstdint>
//#include <algorithm>

namespace Framework {
	namespace RHI {

struct KeyCode {
    uint8_t bytes[16];
/*
    KeyCode() { }
    KeyCode(const KeyCode &other) { memcpy(bytes, other.bytes, 16); }
    KeyCode(KeyCode &&other) { memmove(bytes, other.bytes, 16); }

    KeyCode& operator =(const KeyCode &other) { memcpy(bytes, other.bytes, 16); return (*this); }
    KeyCode& operator =(KeyCode &&other) { memmove(bytes, other.bytes, 16); return (*this); }*/
};

	} // namespace RHI
} // namespace Framework
