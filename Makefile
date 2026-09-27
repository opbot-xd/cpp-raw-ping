sample_ip ?= 8.8.8.8

myping: main.cpp socket_connection.h
	g++ main.cpp -o myping

socket_connection.h: checksum.h
	g++ -c checksum.h socket_connection.h

run: myping
	sudo ./myping $(sample_ip)

clean:
	rm -f myping *.gch