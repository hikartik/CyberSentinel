// main.c
#include "utils.h"
#include "scanner.h"
#include "heuristic.h"
#include "realtime.h"
#include "process_monitor.h"
#include "network_monitor.h"
#include "quarantine.h"
#include "update.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

// Thread function for realtime monitoring.
void *realtime_thread(void *arg) {
    monitor_directory((char *)arg);
    return NULL;
}

void run_tests() {
    printf("Running dummy test cases...\n");
    
    // Test 1: Update signatures.
    update_signatures();
    
    // Create a dummy file for scanning and heuristic analysis.
    const char *dummy_file = "dummy_test.txt";
    FILE *fp = fopen(dummy_file, "w");
    if (fp) {
        fprintf(fp, "This is a dummy file for testing antivirus functionalities.\n");
        fclose(fp);
    } else {
        perror("fopen");
        return;
    }
    
    // Test 2: Signature-based scanning.
    printf("\n[Test] Running signature-based scanning on %s...\n", dummy_file);
    int scan_result = scan_file_signature(dummy_file);
    
    // Test 3: Heuristic analysis.
    printf("\n[Test] Running heuristic analysis on %s...\n", dummy_file);
    int heuristic_result = analyze_file_entropy(dummy_file);
    
    // Test 4: Quarantine if suspicious.
    if (scan_result || heuristic_result) {
        printf("\n[Test] Quarantining %s...\n", dummy_file);
        quarantine_file(dummy_file, "./quarantine");

        char quarantined_path[256];
        snprintf(quarantined_path, sizeof(quarantined_path), "./quarantine/%s", dummy_file);
        if (access(quarantined_path, F_OK) == 0) {
            printf("[Test] Quarantine success: %s exists.\n", quarantined_path);
            remove(quarantined_path); // cleanup after verification
        } else {
            printf("[Test] Quarantine failed: %s not found.\n", quarantined_path);
        }
    } else {
        printf("\n[Test] %s is clean. No quarantine needed.\n", dummy_file);
        remove(dummy_file); // cleanup
    }
    
    // Test 5: Realtime directory monitoring.
    printf("\n[Test] Starting realtime directory monitoring on current directory...\n");
    pthread_t rt_thread;
    if (pthread_create(&rt_thread, NULL, realtime_thread, "./") != 0) {
        perror("pthread_create");
    }
    // Let realtime monitoring run briefly.
    sleep(3);
    
    // Test 6: Process monitoring.
    printf("\n[Test] Listing processes and checking for a suspicious process...\n");
    list_processes();
    check_suspicious_process("malicious_process");
    
    // Test 7: Network monitoring.
    printf("\n[Test] Starting network monitoring on device 'lo'...\n");
    start_network_monitor("lo");
    
    pthread_join(rt_thread, NULL);
    
    printf("\nAll tests completed.\n");
}

int main(int argc, char *argv[]) {
    // Run in test mode if "test" is provided as argument.
    if (argc == 2 && strcmp(argv[1], "test") == 0) {
        run_tests();
        return 0;
    }
    
    // Normal mode: scan the specified file.
    update_signatures();
    if (argc < 2) {
        printf("Usage: %s <file_to_scan> OR %s test\n", argv[0], argv[0]);
        return 1;
    }
    const char *filepath = argv[1];
    
    int sig_result = scan_file_signature(filepath);
    int heur_result = analyze_file_entropy(filepath);
    if (sig_result || heur_result) {
        quarantine_file(filepath, "./quarantine");
        printf("[Main] Malware detected in %s! File quarantined.\n", filepath);
    } else {
        printf("[Main] File %s appears to be clean.\n", filepath);
    }
    
    // Optionally run additional modules.
    pthread_t rt_thread;
    if (pthread_create(&rt_thread, NULL, realtime_thread, "./") != 0) {
        perror("pthread_create");
    }
    list_processes();
    check_suspicious_process("malicious_process");
    start_network_monitor("lo");
    pthread_join(rt_thread, NULL);
    
    return 0;
}

