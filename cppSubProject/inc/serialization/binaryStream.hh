#pragma once

#include <cstdint>
#include <istream>

bool writeU8(std::ostream& out, uint8_t v);
bool writeU16(std::ostream& out, uint16_t v);
bool writeU32(std::ostream& out, uint32_t v);
bool writeF32(std::ostream& out, float v);
bool writeBool(std::ostream& out, bool v);
bool writeString(std::ostream& out, const std::string& s);

bool readU8(std::istream& in, uint8_t& v);
bool readU16(std::istream& in, uint16_t& v);
bool readU32(std::istream& in, uint32_t& v);
bool readF32(std::istream& in, float& v);
bool readBool(std::istream& in, bool& v);
bool readString(std::istream& in, std::string& s);
