# Linux Antivirus Project

This project is a modular antivirus solution implemented in C for Linux. It combines several detection techniques including signature-based scanning, heuristic analysis, real-time file monitoring, process scanning, network monitoring, and file quarantine. It also features an update module that downloads malware signatures from an online source (in JSON format) using libcurl and parses them with cJSON.


## External Libraries

The project uses several external libraries:
- **OpenSSL** (libcrypto): For computing SHA-256 file hashes.
- **libpcap**: For capturing network packets.
- **libcurl**: For downloading signature updates from an online source.
- **inotify**: A Linux kernel API for real-time file system monitoring (no extra library required).
- **cJSON**: A lightweight JSON parser for C (included as `cJSON.c` and `cJSON.h` in the project).

## Building the Project

### Prerequisites

Ensure you have the following installed:
- GCC and Make (usually available on Linux)
- OpenSSL development libraries
- libpcap development libraries
- libcurl development libraries

On Debian/Ubuntu, you can install dependencies with:
```
sudo apt-get update
sudo apt-get install build-essential libssl-dev libpcap-dev libcurl4-openssl-dev
```
### Compile

An updated Makefile is provided. To build the project, simply run:


```
make
```
This will compile all source files and create an executable called antivirus.

### Running the Project

There are two modes to run the antivirus:

1. Scan Mode
To scan a specific file, run:

```
/antivirus <file_to_scan>
```

The antivirus will:

* Download and load the online signature database.
* Compute the SHA-256 hash of the specified file and compare it with known malware signatures.
* Perform heuristic analysis by calculating the file’s entropy.
* If the file is flagged (by either method), it will be moved to a quarantine directory (default: ./quarantine).
* Additionally, it will start monitoring the current directory (using inotify), list running processes, and capture network packets as a demonstration.
2. Test Mode
To run all dummy test cases and see a demonstration of all modules, run:

```
./antivirus test
```
This will:
* Update and print the signature database.
* Create a dummy file (dummy_test.txt) and scan it.
* Execute heuristic analysis.
* Quarantine the file if flagged.
* Start real-time monitoring of the current directory.
* List running processes and perform a dummy process check.
* Capture network traffic on the loopback interface (lo).



## Further Scope and Improvements
There are many opportunities to enhance this project:

* Enhanced Signature Database \
 ->Integrate a more comprehensive, regularly updated signature database. \
->Use secure HTTPS with certificate validation and digital signatures on updates. \
->Implement efficient data structures (like hash tables or bloom filters) for fast signature lookup. 
* Advanced Heuristics \
->Expand heuristic checks to include analysis of file structure, suspicious API call patterns, or even static disassembly of binaries. \
->Integrate fuzzy hashing (e.g., ssdeep) to catch polymorphic malware. \
->Consider behavioral analysis techniques or sandboxing for dynamic analysis. 
* Improved Real-Time Monitoring \
->Extend inotify-based monitoring to recursively watch multiple directories. \
->Explore Linux’s fanotify API for on-access scanning (which can intercept file open events). \
->Integrate scheduling and caching to avoid re-scanning unchanged files. 
* Process and Network Analysis Enhancements \
->Enhance process scanning to analyze process memory, network connections, and inter-process communications. \
->Build more sophisticated packet analysis with deeper protocol inspection.
Integrate known blacklists or threat intelligence feeds for network destinations. 
* Self-Protection and Hardening \
->Implement privilege separation (e.g., using sandboxing or minimal privileges for different modules). \
->Add watchdog processes to ensure the antivirus remains active and cannot be easily terminated. \
->Harden against common attacks (buffer overflows, format string vulnerabilities) and incorporate code auditing tools. 
* User Interface \
->Develop a user-friendly command-line or graphical interface to view scan results, manage quarantined files, and configure scanning options. \
->Integrate logging and reporting features for system administrators.
* Performance Optimizations \
->Introduce multithreading and asynchronous I/O for improved performance. \
->Optimize file scanning and network monitoring modules to reduce CPU and memory overhead.



