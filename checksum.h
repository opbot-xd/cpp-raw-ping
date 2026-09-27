#pragma once
#include <cstdint>

using namespace std;

inline uint16_t calculate_checksum(const void* data, int length) {
    const uint16_t* buf=static_cast<const uint16_t*>(data);
    uint32_t sum=0;
    for (;length>1;length-=2) {
        sum += *buf++;
    }
    if (length&1)    sum += *(static_cast<const uint8_t*>(static_cast<const void*>(buf))); // our len is 62 so this would not be executed
    sum=(sum >> 16)+(sum & 0xFFFF);
    sum+=(sum >> 16);
    return ~sum;
}
