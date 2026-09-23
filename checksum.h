#pragma once
#include <cstdint>

using namespace std;


inline uint16_t calculate_checksum(const void* data, int length) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t sum = 0;
    for (int i=0;i+1<length;i+=2) {
        uint16_t word=bytes[i] | (bytes[i+1]<<8); // Assumes little endian architecture
        sum+=word;
    }
    if (length % 2 != 0)    sum += bytes[length-1];    // will not be used in our code anyways as length = 64
    while(sum>>16)    sum=(sum&0xFFFF)+(sum>>16);

    return (uint16_t)(~sum);

}