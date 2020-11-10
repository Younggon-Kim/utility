/*
 * Copyright (C) 2020  Younggon Kim<dev.ygkim@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PCAP_UTIL_HPP__
#define PCAP_UTIL_HPP__

#include <mutex>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/igmp.h>

namespace util {

class PcapUtil {
public:
    static void printPacket(const struct pcap_pkthdr *header, const uint8_t *packet) {
        std::lock_guard<std::mutex> lock(PcapUtil::mutex_);

        printf("-------------------Packet START------------------\n");
        printf("Length: %d\n",header->len);

        struct ether_header *eptr = (struct ether_header *) packet;
        struct ip * ipptr = (struct ip *) (packet + sizeof(struct ether_header));
        struct tcphdr * tcpptr = (struct tcphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));
        struct udphdr * udpptr = (struct udphdr *) (packet + sizeof(struct ether_header) + sizeof(struct ip));
        u_char * loadptr;
        int length;

        //link layer
        printEthernet(eptr);

        //IP layer
        if(ntohs(eptr->ether_type) == ETHERTYPE_IP){
            printIP(ipptr);

            //transport layer
            if(ipptr->ip_p == IPPROTO_TCP){
                printTCP(tcpptr);
                loadptr = (u_char *)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(tcphdr));
                length = ntohs(ipptr->ip_len) - (sizeof(tcphdr) + sizeof(struct ip));
            } else if(ipptr->ip_p == IPPROTO_UDP){
                printUDP(udpptr);
                loadptr = (u_char *)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(udphdr));
                length = ntohs(ipptr->ip_len) - (sizeof(udphdr) + sizeof(struct ip));
            } else {
                return;
            }

            printPayload(loadptr, length);
        } else {
            //not supported protocol
            printf("Not supported protocol in this util");
        }
        printf("\n--------------------Packet END-------------------\n");
    }
private:
    static std::mutex mutex_;

    //print Ethernet header
    static void printEthernet(struct ether_header *eptr){
        printf("Ethernet Header\n");
        printAddr(eptr->ether_dhost, "MAC Destination");
        printAddr(eptr->ether_shost, "MAC Source");
    }

    //print Ethernet Header Source/Destination hosts in MAC MM:MM:MM:SS:SS:SS format
    static void printAddr(u_char *ptr, std::string str){
        printf("    %s: ", str.c_str());
        for(int i= 0; i < ETHER_ADDR_LEN;i++){
            printf("%x", ptr[i]);
            if(i!= ETHER_ADDR_LEN -1){
                printf(":");
            }
        }
        printf("\n");
    }

    //print IP header
    static void printIP(struct ip * ipptr){
        printf("IP Header\n");
        printf("    Version: %i\n", ipptr->ip_v);
        printf("    IP Destinaton: %s\n", inet_ntoa(ipptr->ip_dst));
        printf("    IP Source: %s\n", inet_ntoa(ipptr->ip_src));
        printf("    IP Protocol: %i\n", ipptr->ip_p);
    }

    //print TCP header
    static void printTCP(struct tcphdr * tcpptr){
        printf("TCP Header\n");
        printf("    Destinaton Port: %i\n", htons(tcpptr->th_dport));
        printf("    Source Port: %i\n", htons(tcpptr->th_sport));
    }

    //print UDP header
    static void printUDP(struct udphdr * udpptr){
        printf("UDP Header\n");
        printf("    Destinaton Port: %i\n", htons(udpptr->uh_dport));
        printf("    Source Port: %i\n", htons(udpptr->uh_sport));
    }

    // Print short payload in Hex and ASCII
    static void printPayload(u_char * loadptr, int length){
        if(length <= 0) {
            return;
        }

        const uint8_t MAX_LEN = 20;
        uint8_t len = length > MAX_LEN ? MAX_LEN : length;

        printf("Payload\n");
        const u_char  *ch = loadptr;

        //print hex
        for(int i = 0; i < len; i++){
            printf("%02x ", *ch);
            ch++;
        }
        printf("%s\n", (len > MAX_LEN ? "..." : ""));

        //print ascii
        ch = loadptr;
        for(int i = 0; i < len; i++) {
            if (isprint(*ch)){
                printf("%c", *ch);
            } else {
                printf(".");
            }
            ch++;
        }
        printf("%s\n", (len > MAX_LEN ? "..." : ""));
    }
};

std::mutex PcapUtil::mutex_;

} //namespace util

#endif //PCAP_UTIL_HPP__
