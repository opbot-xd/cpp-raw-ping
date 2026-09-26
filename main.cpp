#include "socket_connection.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    int sock = create_icmp_socket();
    if (sock < 0) {
        cerr << "Failed to create socket\n";
        return 1;
    }

    int pid = getpid() & 0xFFFF;
    int seq = 1;
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    string source_ip_address = "";
    cout << "Enter the IP to ping (IPv4): ";
    cin >> source_ip_address; // TODOs: cli args instead of this
    if (inet_pton(AF_INET, source_ip_address.c_str(), &dest_addr.sin_addr) <= 0) {
        cout << "Invalid IPv4 address format!\n";
        close(sock);
        return 1;
    }
    if (!send_ping(sock, dest_addr, pid, seq)) {
        cout << "Sending failed";
        close(sock);
        return 1;
    }
    auto res = receive_ping(sock,pid);
    if (res.first!=INT_MAX) {
        cout<<"64 bytes from "<<source_ip_address<<" : seq="<<res.second<<" ttl="<<res.first<<endl;
    }
    close(sock);
    return 0;
}