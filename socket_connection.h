#pragma once

#include "checksum.h"

#include <iostream>
#include <vector>
#include <climits>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const int PAYLOAD_SIZE = 56;
const int PACKET_SIZE = sizeof(uint8_t) * 2 + sizeof(uint16_t) * 3 + PAYLOAD_SIZE; // 64 bytes

#pragma pack(push,1)
struct IcmpPacket{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
    char payload[PAYLOAD_SIZE];

    IcmpPacket(): type(8), code(0), checksum(0), id(0), seq(0) {
        for(int i=0;i<PAYLOAD_SIZE;i++)    payload[i]='A';
    }
};
#pragma pack(pop)


inline int create_icmp_socket() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    return sock;
}

inline IcmpPacket build_packet(uint16_t id, uint16_t seq) {
    IcmpPacket pkt;

    pkt.id = htons(id);
    pkt.seq = htons(seq);
    pkt.checksum = calculate_checksum(&pkt, sizeof(pkt));

    return pkt;
}

inline bool send_ping(int sockfd, const sockaddr_in& dest_addr, uint16_t id, uint16_t seq) {
    IcmpPacket pkt = build_packet(id,seq);
    ssize_t i = sendto(sockfd, &pkt, PACKET_SIZE, 0, (struct sockaddr*) &dest_addr, sizeof(dest_addr));
    if(i==-1){
        cerr << "sendto syscall failed\n";
        return false;
    }
    return true;
}

inline vector<int> receive_ping(int sockfd, uint16_t expected_id) {
    uint8_t recv_buffer[1024];
    sockaddr_in sender_addr{};
    socklen_t add_len = sizeof(sender_addr);
    while(1){
        ssize_t bytes_received = recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*) &sender_addr, &add_len);
        if(bytes_received<0){
            return {INT_MAX,-1};
        }
        int ip_header_len = (recv_buffer[0]&0xF)*4;
        IcmpPacket* reply = (IcmpPacket*)&recv_buffer[ip_header_len];
        if (reply->type==0 && ntohs(reply->id)==expected_id) {
            int ttl=recv_buffer[8];
            int out_seq = ntohs(reply->seq);
            return {ttl,out_seq};
        }
    
    }
}
