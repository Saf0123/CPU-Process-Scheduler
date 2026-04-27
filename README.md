# ⚙️ CPU Scheduling Algorithms Simulator (C++)

## 📌 Overview
This project is a CPU Scheduling Algorithms Simulator developed in C++. It demonstrates how operating systems manage process execution using different scheduling strategies. The simulator provides a complete analysis of process behavior, including execution order, waiting times, turnaround times, and CPU performance metrics.

It also visualizes execution using a Gantt Chart and logs results into external files for further analysis.

---

## 🚀 Features

- Implementation of multiple CPU scheduling algorithms:
  - First Come First Serve (FCFS)
  - Shortest Job First (SJF)
  - Shortest Remaining Time First (SRTF) – Preemptive
  - Priority Scheduling

- Interactive user input system with validation
- Real-time Gantt Chart visualization in terminal
- Process execution table showing:
  - Arrival Time (AT)
  - Burst Time (BT)
  - Completion Time (CT)
  - Waiting Time (WT)
  - Turnaround Time (TAT)
  - Response Time (RT)

- Performance analysis:
  - CPU Utilization
  - Throughput
  - Average Waiting Time
  - Average Turnaround Time
  - Maximum Waiting Time
  - Maximum Turnaround Time

- File logging system:
  - Saves execution history in `.txt` format
  - Exports structured data in `.xml` format
  - Includes timestamps and run tracking

---

## 🛠️ Tech Stack

- C++
- Standard Template Library (STL)
- File Handling (TXT & XML)
- Console-based UI with ANSI color formatting
