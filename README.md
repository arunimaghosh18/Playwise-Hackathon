# PlayWise – Smart Playlist Management System

A command-line based intelligent music playlist manager built using core Data Structures and Algorithms in C++. Designed as part of the PlayWise Hackathon, this system simulates real-world music engine features like song lookup, rating, playback history, favorites, sorting, and snapshot aggregation.

---

## Features Implemented

### Core Modules
- Playlist Engine – using Doubly Linked List for add, delete, move, and reverse
- Playback History – using Stack for LIFO undo functionality
- Song Rating Tree – using Binary Search Tree (BST) for rating management
- Instant Song Lookup – HashMap-based fast title/ID lookup
- Time-based Sorting – Quick Sort applied to playlist for title, duration, recency
- System Snapshot Module – aggregate dashboard of top 5 longest, recent plays, and rating stats
- Space-Time Optimization – memory-safe architecture using `shared_ptr` and modular sync

### Additional Use Cases
- Blocklist for Artists – prevents play/add of blocked artists using HashSet
- Play Duration Visualizer – computes total, longest, and shortest song durations
- Suggest Songs by Time – greedy fit of songs within a time limit
- Partial Title Search – case-insensitive substring search

---

## Data Structures Used

| Module                | Data Structure(s)                   |
|-----------------------|-------------------------------------|
| Playlist Engine       | Doubly Linked List + Vector         |
| Playback History      | Stack (`std::stack`)                |
| Song Lookup           | HashMap (`unordered_map`)           |
| Song Rating Tree      | Binary Search Tree                  |
| Favorites             | Max Heap (`priority_queue`)         |
| Blocklist             | HashSet (`unordered_set`)           |
| Sorting & Suggestions | Vector + Custom Quick Sort          |

---

## How to Run

### Prerequisites
- C++ compiler supporting C++11 or higher (e.g., g++, clang++)
- Terminal or Command Prompt

### Compile and Run
```bash
g++ -std=c++11 playwise.cpp -o playwise
./playwise
