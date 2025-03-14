# Makefile for the Antivirus Project (Updated with math library)
CC = gcc
CFLAGS = -Wall -I. -pthread -Wno-deprecated-declarations
LIBS = -lssl -lcrypto -lcurl -lpcap -lm

# List of source files, including cJSON.c
SRCS = main.c scanner.c heuristic.c realtime.c process_monitor.c network_monitor.c quarantine.c update.c cJSON.c
OBJS = $(SRCS:.c=.o)
TARGET = antivirus

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

