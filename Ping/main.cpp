//
//  main.cpp
//  internship-application-systems
//
//  Created by Harry Tung on 2020/4/13.
//  Copyright © 2020 Harry Tung. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <errno.h>

#define PERIOD          50 // Interval between each ping messages in ms
#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   5 // Maximum wait time in seconds

int main(int argc, const char * argv[]) {

    struct icmp *icmpSend;
    struct sockaddr_in addrSend;
    struct sockaddr_in addrReceive;
    
    socklen_t addrReceiveLength = sizeof(struct sockaddr_in);
    
    int sock;
    long errorCheck;
    
    char sendPacket[PACKET_SIZE];
    char receivePacket[PACKET_SIZE];
    
    struct timeval *time;
    struct timeval timeout;
    
    if(!(argc == 2 || argc == 3)) {
        std::cout << "Usage: " << argv[0] << " <IP>" << std::endl;
    }
    else if(argc == 2) {
        
        bzero(&addrSend, sizeof(addrSend));
        
        // Convert IP address
        inet_pton(AF_INET, argv[1], &(addrSend.sin_addr));
        
        std::cout << "Sending ICMP Echo Requests to " << argv[1] << std::endl;
        
        // Create socket
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        if(sock < 0) {
            perror("socket error");
            return -1;
        }
        
        // Set timeout duration
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        icmpSend = (struct icmp*)sendPacket;
        icmpSend->icmp_type = ICMP_ECHO;
        icmpSend->icmp_code = 0;
        icmpSend->icmp_cksum = 0;
        icmpSend->icmp_seq = 1;
        icmpSend->icmp_id = 0;
        
        time = (struct timeval *)icmpSend->icmp_data;
        gettimeofday(time, NULL);
        
        // Send ICMP header to server
        errorCheck = sendto(sock,
                            sendPacket,
                            64,
                            0,
                            (struct sockaddr*)&addrSend,
                            sizeof(addrSend));
        
        if(errorCheck < 0) {
            perror("sendto error");
            return -1;
        }
        
        errorCheck = recvfrom(sock,
                              receivePacket,
                              sizeof(receivePacket),
                              0,
                              (struct sockaddr*)&addrSend,
                              &addrReceiveLength);
        
        if(errorCheck < 0) {
            perror("recvfrom error");
            return -1;
        }
        
        std::cout << "Sent ICMP Echo Requests to " << argv[1] << ": Received, time: " << std::endl;
        
        
    }
    std::cout << "Hello, World!\n";
    return 0;
}
