#include "socket_connection.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <csignal>
#include <numeric>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

atomic<bool> keep_running{true};

void handle_sigint(int signum) {
    keep_running = false;
}

int main() {
    signal(SIGINT, handle_sigint);
    int received = 0;
    vector<double> rtt;
    int sock = create_icmp_socket();
    if (sock < 0) {
        cerr << "Failed to create socket\n";
        return 1;
    }

    int pid = getpid() & 0xFFFF;
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    string source_ip_address = "";
    cout << "Enter the IP to ping (IPv4): ";
    cin >> source_ip_address; // TODOs: cli args instead of this
    int seq_number=1;
    if (inet_pton(AF_INET, source_ip_address.c_str(), &dest_addr.sin_addr) <= 0) {
        cerr << "Invalid IPv4 address format!\n";
        close(sock);
        return 1;
    }
    while (keep_running){
        auto ping_time =  high_resolution_clock::now() + 1000ms;
        auto start = high_resolution_clock::now();
        if (!send_ping(sock, dest_addr, pid, seq_number)) {
            cerr << "Sending failed";
            close(sock);
            return 1;
        }
        auto res = receive_ping(sock,pid);
        if (res.first!=INT_MAX) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end-start);
            rtt.push_back(duration.count()/1000.0);
            cout<<"64 bytes from "<<source_ip_address<<": seq="<<res.second<<" ttl="<<res.first<<" time="<<duration.count()/1000.0<<" ms"<<endl;
            received++;
        }
        else{
            cout<<"Packet Lost! from "<<source_ip_address<< " for seq="<<seq_number<<endl;
        }
        this_thread::sleep_until(ping_time);
        if (high_resolution_clock::now() > ping_time)    ping_time = high_resolution_clock::now();
        seq_number++;
    }
    int transmitted = seq_number-1;
    if(transmitted==0){
        close(sock);
        return 0;
    }
    cout << "\n--- " << source_ip_address << " ping statistics ---\n";
    double loss = ((transmitted - received) * 100.0) / transmitted;    
    cout << fixed << setprecision(1);
    cout << transmitted << " packets transmitted, " << received << " received, " << loss << "% packet loss\n";
    if (!rtt.empty()) cout << "rtt min/avg/max = " << *min_element(rtt.begin(),rtt.end()) << "/" << accumulate(rtt.begin(), rtt.end(), 0.0)/rtt.size() << "/" << *max_element(rtt.begin(),rtt.end()) << " ms\n";
    close(sock);
    return 0;
}