# Custom C++ ICMP Ping

A lightweight, from-scratch implementation of the `ping` utility in modern C++ using Linux raw sockets (`SOCK_RAW`). Built in accordance with **[RFC 792](https://www.rfc-editor.org/info/rfc792/)** (ICMP) and **[RFC 1071](https://www.rfc-editor.org/info/rfc1071/)** (Internet Checksum).

## Current Features and Future enhancements

- [x] Socket + Single Packet Round Trip
- [x] Timing + Real Output Formatting
- [x] Timeout / Packet Loss Handling
- [x] Continuous Loop + Sequence Tracking
- [x] Final Stats Output & CLI args
- [ ] Makefile and CLI args

---