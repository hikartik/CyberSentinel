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
    // Capture 5 packets for demonstration.
    pcap_loop(handle, 5, packet_handler, NULL);
    pcap_close(handle);
}

