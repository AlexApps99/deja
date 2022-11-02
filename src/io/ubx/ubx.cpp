#include <cstdint>
#include <vector>

// TODO NAV + CFG + ACK

struct [[gnu::packed]] UbxHeader {
    unsigned char sync_a = 0xB5;
    unsigned char sync_b = 0x62;
    unsigned char msg_class = 0;
    unsigned char msg_id = 0;
    unsigned short length = 0;
};

// Checks if a message is valid to be parsed
static bool verify_msg(const unsigned char *const data,
                       const std::size_t length) {
    // verify header and length
    if (length < 8 || data[0] != 0xB5 || data[1] != 0x62) return false;
    unsigned short len =
        ((unsigned short)data[2]) | (((unsigned short)data[3]) << 8);
    if (length < len + 8) return false;

    // verify checksum
    unsigned char ck_a = 0, ck_b = 0;
    for (std::size_t i = 0; i < 4 + len; i++) {
        ck_a += data[i + 2];
        ck_b += ck_a;
    }
    if (ck_a != data[len + 6] || ck_b != data[len + 7]) return false;

    return true;
}

class UbxParse {
    std::vector<unsigned char> buf;
};