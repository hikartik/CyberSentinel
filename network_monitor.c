// network_monitor.c
#include "utils.h"
#include "network_monitor.h"
#include <pcap.h>

void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet) {
    printf("[Network Monitor] Captured packet of length: %d\n", header->len);
    // In a real system, packet analysis would occur here.
}

void start_network_monitor(const char *device) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (!handle) {
        fprintf(stderr, "[Network Monitor] Unable to open device %s: %s\n", device, errbuf);
        return;
    }
    printf("[Network Monitor] Monitoring device: %s\n", device);
    // Capture up to 5 packets, but do not block indefinitely
    int packets_captured = 0;
    int timeouts = 0;
    while (packets_captured < 5 && timeouts < 5) {
        struct pcap_pkthdr *header;
        const u_char *packet;
        int res = pcap_next_ex(handle, &header, &packet);
        if (res == 1) {
            packet_handler(NULL, header, packet);
            packets_captured++;
        } else if (res == 0) { // timeout
            timeouts++;
        } else {
            break;
        }
    }
    pcap_close(handle);
}

