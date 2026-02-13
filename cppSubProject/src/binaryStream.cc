#include "binaryStream.hh"

#include <cstdint> // uint8_t, uint16_t, uint32_t
#include <cstring> // memcpy

bool writeU8(std::ostream& out, uint8_t v)
{
    char c = static_cast<char>(v);
    return static_cast<bool>(out.write(&c, 1));
}

bool writeU16(std::ostream& out, uint16_t v)
{
    uint8_t b[2] = {
        static_cast<uint8_t>((v >> 8) & 0xFF),
        static_cast<uint8_t>(v & 0xFF)
    };

    return static_cast<bool>(
        out.write(reinterpret_cast<const char*>(b), 2)
    );
}

bool writeU32(std::ostream& out, uint32_t v)
{
    uint8_t b[4] = {
        static_cast<uint8_t>((v >> 24) & 0xFF),
        static_cast<uint8_t>((v >> 16) & 0xFF),
        static_cast<uint8_t>((v >> 8) & 0xFF),
        static_cast<uint8_t>(v & 0xFF)
    };

    return static_cast<bool>(
        out.write(reinterpret_cast<const char*>(b), 4)
    );
}

bool writeF32(std::ostream& out, float v)
{
    uint32_t bits;
    std::memcpy(&bits, &v, sizeof(float));
    return writeU32(out, bits);
}

bool writeString(std::ostream& out, const std::string& s)
{
    // Write length (number of bytes, not characters)
    if (!writeU32(out, static_cast<uint32_t>(s.size())))
        return false;

    if (s.empty())
        return true;

    return static_cast<bool>(
        out.write(s.data(), static_cast<std::streamsize>(s.size()))
    );
}

bool readU8(std::istream& in, uint8_t& v)
{
    char c;
    if (!in.read(&c, 1)) return false;
    v = static_cast<uint8_t>(c);
    return true;
}

bool readU16(std::istream& in, uint16_t& v)
{
    uint8_t b[2];
    if (!in.read(reinterpret_cast<char*>(b), 2)) return false;

    v = (uint16_t(b[0]) << 8) |
         uint16_t(b[1]);
    return true;
}

bool readU32(std::istream& in, uint32_t& v)
{
    uint8_t b[4];
    if (!in.read(reinterpret_cast<char*>(b), 4)) return false;

    v = (uint32_t(b[0]) << 24) |
        (uint32_t(b[1]) << 16) |
        (uint32_t(b[2]) << 8)  |
         uint32_t(b[3]);
    return true;
}

bool readF32(std::istream& in, float& v)
{
    uint32_t bits;
    if (!readU32(in, bits)) return false;
    std::memcpy(&v, &bits, sizeof(float));
    return true;
}

bool readString(std::istream& in, std::string& s)
{
    uint32_t length;
    if (!readU32(in, length))
        return false;

    s.clear();

    if (length == 0)
        return true;

    s.resize(length);

    return static_cast<bool>(
        in.read(&s[0], static_cast<std::streamsize>(length))
    );
}
