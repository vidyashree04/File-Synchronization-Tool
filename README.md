Team no 15 

Project Title: File Synchronization Tool

Status: Completed

Project Description:

  This project is a user-space utility developed in C on Linux that keeps two directories in sync — either locally or across machines. It detects file changes (create, modify, delete) and propagates updates in   near real time.
  The tool supports:
  - One-way local synchronization.
  - Bi-directional local synchronization.
  - One-way remote synchronization (across two machines using TCP sockets).

It ensures conflict resolution (latest file wins), preserves file metadata (permissions, timestamps), and provides efficient file-system monitoring using Linux system calls.

Objectives:
-  Detect file system changes in real time.
- Synchronize directories locally and remotely.
- Handle file create, update, and delete events.
- Preserve metadata (timestamps, permissions).

Features:
- Real-time file monitoring using inotify.
- Conflict resolution with timestamps.
- Metadata preservation (chmod, utime).
- Multi-threaded event handling (monitor, scheduler, consumer threads).
- Local and remote sync support.

Project Folder Structure:

file-sync-tool/

src/                # Source files (.c)
- sync_main.c
- cli_parser.c
- event_queue.c
- file_monitor.c
- sync_manager.c
- scheduler.c
- metadata_handler.c
- network_transfer.c

include/            # Header files (.h)
- cli_parser.h
- event_queue.h
- file_monitor.h
- sync_manager.h
- scheduler.h
- metadata_handler.h

docs/             # Project documentation
- File_Synchronization_Tool_Report(Team 15).pdf

README.md           # Project overview and usage instructions



Usage Instructions:

How to compile:
  From the project root directory, run:
    
    gcc -o build/sync_tool src/*.c -Iinclude -lpthread
    
How to Run: 1. Local One-way Synchronization:

    ./build/sync_tool --src=test/watch_dir --dest=test/target_dir --mode=oneway

2.Local Bi-directional Synchronization:

    ./build/sync_tool --src=test/dir1 --dest=test/dir2 --mode=twoway

3.Remote One-way Synchronization: 
On Server(Destination Machine):

    ./build/sync_tool --src=test/target_dir --dest=test/target_dir --mode=oneway --remote --port=8080

On Client(Source Machine):

    ./build/sync_tool --src=test/watch_dir --dest=test/target_dir --mode=oneway --remote --port=8080 --ip=<server_ip>








   


