
// CPU Scheduling Algorithms Simulator
// Supports: FCFS, SJF, SRTF, Priority Scheduling
// Outputs: Process Table, Gantt Chart, Summary Statistics
// Logs results to files with run numbers

#include <fstream>  // For file operations
#include <iostream> // For input/output
#include <list> // For linked list
#include <queue> // For queue
#include <limits> // For numeric limits
#include <iomanip> // For output formatting
#include <string> // For string operations
#include <ctime> // For timestamping

// ANSI color codes for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


using namespace std;

// ==================== Data Structures ====================
struct Process {
	int pid; // Process ID
	int arrivalTime; // Arrival Time
	int burstTime; // Burst Time
	int remainingTime; // Remaining Time for SRTF
	int priority; // Priority for Priority Scheduling
	int startTime; // Start Time
	int completionTime; // Completion Time
	int waitingTime; // Waiting Time
	int turnaroundTime; // Turnaround Time
	bool isCompleted; // Completion Status
	int responseTime; // Response Time
};

// Gantt chart segment
struct Segment {
	int pid; // Process ID (-1 for idle)
	int start; // Start time
	int end; // End time
	bool isIdle; // Is idle segment
	bool isPreempted; // Was the process preempted
};

// ==================== Process Input & Sorting ====================
list<Process> inputProcesses(int n); // Input process details
void sortByArrival(list<Process>& p); // Sort processes by arrival time

// ==================== Output & Display ====================
void printProcessTable(list<Process>& p); // Print process table
void printGanttChart(list<Segment>& gantt); // Print Gantt chart

// ==================== Process Selection Helpers ====================
Process* findShortestJob(list<Process>& p, int currentTime); // Find shortest job
Process* findShortestRemainingTime(list<Process>& p, int currentTime); // Find shortest remaining time
Process* findHighestPriority(list<Process>& p, int currentTime); // Find highest priority
int nextArrivalTime(list<Process>& p, int currentTime); // Find next arrival time
bool allCompleted(list<Process>& p); // Check if all processes are completed

// ==================== Time Calculations ====================
void calculateTimes(list<Process>& p); // Calculate waiting and turnaround times
double calculateCPUUtilization(list<Segment>& gantt); // Calculate CPU utilization
double calculateThroughput(list<Process>& p, int totalTime); // Calculate throughput
double calculateAverageWaitingTime(list<Process>& p); // Calculate average waiting time
double calculateAverageTurnaroundTime(list<Process>& p); // Calculate average turnaround time
int calculateMaxWaitingTime(list<Process>& p); // Calculate maximum waiting time
int calculateMaxTurnaroundTime(list<Process>& p); // Calculate maximum turnaround time

// ==================== Scheduling Algorithms ====================
void FCFS(list<Process>& p, list<Segment>& gantt); // FCFS Scheduling
void SJF(list<Process>& p, list<Segment>& gantt); // SJF Scheduling
void SRTF(list<Process>& p, list<Segment>& gantt); // SRTF Scheduling
void PriorityScheduling(list<Process>& p, list<Segment>& gantt); // Priority Scheduling

// ==================== File Saving / Logging ====================
void saveRunToFile(const list<Process>& p, const list<Segment>& gantt, const string& algorithm); // Save run details to a text file
void exportRunToXML(const list<Process>& p, const list<Segment>& gantt, const string& algorithm); // Export run details to an XML file
int getNextRunNumber(const string& filename); // Get next run number from file

// ================================================================
//                     PROCESS INPUT & SORTING
// ================================================================
// Function to input process details
list<Process> inputProcesses(int n) {
	list<Process> processes; // List to hold processes
    for (int i = 0; i < n; ++i) {
		Process p; // Create a new process
		p.pid = i + 1; // Process ID starts from 1
		while (true) { // Input validation loop for arrival time
            cout << "\nEnter arrival time for process " << p.pid << ": ";
			if (cin >> p.arrivalTime && p.arrivalTime >= 0) break; // Valid input
            else {
                cout << "Invalid input! Arrival time must be >= 0.\n";
                cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            }
        }

        while (true) {
            cout << "Enter burst time for process " << p.pid << ": ";
			if (cin >> p.burstTime && p.burstTime > 0) break; // Valid input
            else {
                cout << "Invalid input! Burst time must be > 0.\n";
                cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            }
        }

        while (true) {
            cout << "Enter priority for process " << p.pid << " (lower value = higher priority): ";
			if (cin >> p.priority && p.priority >= 0) break; // Valid input
            else {
                cout << "Invalid input! Priority must be >= 0.\n";
                cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            }
        }

		p.remainingTime = p.burstTime; // Initialize remaining time
		p.isCompleted = false; // Initialize completion status
		p.startTime = -1; // Initialize start time
		p.responseTime = -1; // Initialize response time
		processes.push_back(p); // Add process to the list
    }
	return processes; // Return the list of processes
}

// Function to sort processes by arrival time
void sortByArrival(list<Process>& p) {
	p.sort([](const Process& a, const Process& b) { // Lambda function for sorting
        return a.arrivalTime < b.arrivalTime;
        });
}

// ================================================================
//                     OUTPUT & DISPLAY
// ================================================================
// Function to print process table
void printProcessTable(list<Process>& p) {
    cout << "\n" << CYAN << "Process Table:" << RESET << "\n";
    cout << BLUE << "PID\tAT\tBT\tCT\tWT\tTAT\tRT" << RESET << "\n";
    for (const auto& proc : p) {
        cout << GREEN << proc.pid << RESET << "\t"
            << proc.arrivalTime << "\t"
            << proc.burstTime << "\t"
            << RED << proc.completionTime << RESET << "\t"
            << YELLOW << proc.waitingTime << RESET << "\t"
            << MAGENTA << proc.turnaroundTime << RESET << "\t"
            << CYAN << proc.responseTime << RESET << "\n";
    }
}

// Function to print Gantt chart
void printGanttChart(list<Segment>& gantt) {
    cout << "\n" << CYAN << "Gantt Chart:" << RESET << "\n";
    for (const auto& seg : gantt) {
		if (seg.isIdle) { // Idle segment
            cout << RED << "| Idle (" << seg.start << "-" << seg.end << ") " << RESET;
        }
		else { // Process segment
            cout << GREEN << "| P" << seg.pid
                << (seg.isPreempted ? "*" : "")
                << " (" << seg.start << "-" << seg.end << ") ";
        }
    }
    cout << "|\n" << RESET;
}

// ================================================================
//               PROCESS SELECTION HELPERS
// ================================================================
// Functions to find next process based on criteria
Process* findShortestJob(list<Process>& p, int currentTime) {
	Process* shortest = nullptr; // Pointer to shortest job
    for (auto& proc : p) {
        if (!proc.isCompleted && proc.arrivalTime <= currentTime) {
			if (shortest == nullptr || proc.burstTime < shortest->burstTime) { // Found shorter job
				shortest = &proc; // Update shortest job pointer
            }
        }
    }
    return shortest;
}

// Functions to find next process based on criteria
Process* findShortestRemainingTime(list<Process>& p, int currentTime) {
	Process* shortest = nullptr; // Pointer to shortest remaining time job
    for (auto& proc : p) {
        if (!proc.isCompleted && proc.arrivalTime <= currentTime) {
			if (shortest == nullptr || proc.remainingTime < shortest->remainingTime) {// Found shorter remaining time job
				shortest = &proc; // Update shortest remaining time job pointer
            }
        }
    }
    return shortest;
}

// Functions to find next process based on criteria
Process* findHighestPriority(list<Process>& p, int currentTime) {
	Process* highest = nullptr; // Pointer to highest priority job
    for (auto& proc : p) {
        if (!proc.isCompleted && proc.arrivalTime <= currentTime) {
			if (highest == nullptr || proc.priority < highest->priority) { // Found higher priority job
				highest = &proc; // Update highest priority job pointer
            }
        }
    }
    return highest;
}

// Function to find next arrival time
int nextArrivalTime(list<Process>& p, int currentTime) {
	int nextTime = numeric_limits<int>::max(); // Initialize to max int
    for (const auto& proc : p) {
		if (!proc.isCompleted && proc.arrivalTime > currentTime) { // Future arrival
            if (proc.arrivalTime < nextTime) {
				nextTime = proc.arrivalTime; // Update next arrival time
            }
        }
    }
    return nextTime;
}

// Function to check if all processes are completed
bool allCompleted(list<Process>& p) {
    for (const auto& proc : p) {
        if (!proc.isCompleted) return false;
    }
    return true;
}

// ================================================================
//                   TIME CALCULATIONS
// ================================================================
// Function to calculate waiting and turnaround times
void calculateTimes(list<Process>& p) {
    for (auto& proc : p) {
		proc.turnaroundTime = proc.completionTime - proc.arrivalTime; // TAT = CT - AT
		proc.waitingTime = proc.turnaroundTime - proc.burstTime; // WT = TAT - BT
    }
}

// Function to calculate CPU utilization
double calculateCPUUtilization(list<Segment>& gantt) {
	int busyTime = 0; // Total busy time
	int totalTime = gantt.back().end; // Total time from Gantt chart
	for (const auto& seg : gantt) { // Iterate through Gantt segments
        if (!seg.isIdle) {
			busyTime += (seg.end - seg.start); // Sum busy time
        }
    }
    return (static_cast<double>(busyTime) / totalTime) * 100.0;
}

// Function to calculate throughput
double calculateThroughput(list<Process>& p, int totalTime) {
	return static_cast<double>(p.size()) / totalTime; // Throughput = number of processes / total time
}

// Function to calculate average waiting time
double calculateAverageWaitingTime(list<Process>& p) {
	double totalWT = 0; // Total waiting time
    for (const auto& proc : p) {
		totalWT += proc.waitingTime; // Sum waiting times
    }
    return totalWT / p.size();
}

// Function to calculate average turnaround time
double calculateAverageTurnaroundTime(list<Process>& p) {
	double totalTAT = 0; // Total turnaround time
    for (const auto& proc : p) {
		totalTAT += proc.turnaroundTime; // Sum turnaround times
    }
    return totalTAT / p.size();
}

// Function to calculate maximum waiting time
int calculateMaxWaitingTime(list<Process>& p) {
	int maxWT = 0; // Initialize max waiting time
    for (const auto& proc : p) {
        if (proc.waitingTime > maxWT) { 
			maxWT = proc.waitingTime; // Update max waiting time
        }
    }
    return maxWT;
}

// Function to calculate maximum turnaround time
int calculateMaxTurnaroundTime(list<Process>& p) {
	int maxTAT = 0; // Initialize max turnaround time
    for (const auto& proc : p) {
        if (proc.turnaroundTime > maxTAT) {
			maxTAT = proc.turnaroundTime; // Update max turnaround time
        }
    }
    return maxTAT;
}

// ================================================================
//               SCHEDULING ALGORITHMS
// ================================================================
// FCFS Scheduling
void FCFS(list<Process>& p, list<Segment>& gantt) {
	queue<Process*> readyQueue; // Ready queue
	int currentTime = 0; // Current time

    while (!allCompleted(p)) {

        for (auto& proc : p) {
			if (!proc.isCompleted && proc.arrivalTime <= currentTime) { // Process has arrived

				bool inQueue = false; // Check if already in queue
				queue<Process*> temp = readyQueue; // Temporary queue to check if process is already in ready queue
                while (!temp.empty()) {
					if (temp.front()->pid == proc.pid) { inQueue = true; break; } // Found in queue
					temp.pop(); // Remove front element
                }
				if (!inQueue) readyQueue.push(&proc); // Add to ready queue
            }
        }

        if (!readyQueue.empty()) {
			Process* currentProc = readyQueue.front(); // Get next process
			readyQueue.pop(); // Remove from ready queue
            if (currentProc->startTime == -1) {
				currentProc->startTime = currentTime; // Set start time
				currentProc->responseTime = currentTime - currentProc->arrivalTime; // Set response time
            }
            gantt.push_back({ currentProc->pid, currentTime, currentTime + currentProc->burstTime, false, false });
			currentTime += currentProc->burstTime; // Update current time
			currentProc->completionTime = currentTime; // Set completion time
			currentProc->isCompleted = true; // Mark as completed
        }
        else {
			int nextArrival = nextArrivalTime(p, currentTime); // Find next arrival time
			if (nextArrival == numeric_limits<int>::max()) break; // No more arrivals
			gantt.push_back({ -1, currentTime, nextArrival, true, false }); // Idle segment
			currentTime = nextArrival; // Update current time
        }

    }
}

// SJF Scheduling
void SJF(list<Process>& p, list<Segment>& gantt) {
	int currentTime = 0; // Current time
    while (!allCompleted(p)) {
        Process* currentProc = findShortestJob(p, currentTime);
        if (currentProc) {
			if (currentProc->startTime == -1) { // First time execution
				currentProc->startTime = currentTime; // Set start time
				currentProc->responseTime = currentTime - currentProc->arrivalTime; // Set response time
            }
			gantt.push_back({ currentProc->pid, currentTime, currentTime + currentProc->burstTime, false, false }); // Gantt segment
			currentTime += currentProc->burstTime; // Update current time
			currentProc->completionTime = currentTime; // Set completion time
			currentProc->isCompleted = true; // Mark as completed
        }
        else {
			int nextArrival = nextArrivalTime(p, currentTime); // Find next arrival time
			if (nextArrival == numeric_limits<int>::max()) break; // No more arrivals
			gantt.push_back({ -1, currentTime, nextArrival, true, false }); // Idle segment
			currentTime = nextArrival; // Update current time
        }

    }
}

// SRTF Scheduling
void SRTF(list<Process>& p, list<Segment>& gantt) {
	int currentTime = 0; // Current time
	Process* lastProc = nullptr; // Last executed process
    while (!allCompleted(p)) {
        Process* currentProc = findShortestRemainingTime(p, currentTime);
        if (currentProc) {
            if (currentProc->startTime == -1) {
				currentProc->startTime = currentTime; // Set start time
				currentProc->responseTime = currentTime - currentProc->arrivalTime; // Set response time
            }
            if (!gantt.empty() && gantt.back().pid == currentProc->pid && !gantt.back().isIdle) {
				gantt.back().end++; // Extend current segment
            }
            else {
                if (lastProc) {
					gantt.back().end = currentTime; // End last segment
					gantt.back().isPreempted = true; // Mark as preempted
                }
				gantt.push_back({ currentProc->pid, currentTime, currentTime + 1, false, false }); // New segment
            }

			currentProc->remainingTime--; // Decrement remaining time
			currentTime++; // Increment current time
			if (currentProc->remainingTime == 0) { // Process completed
                currentProc->completionTime = currentTime;
                currentProc->isCompleted = true;
            }
			lastProc = currentProc; // Update last executed process
        }
        else {
            if (lastProc) {
				gantt.back().end = currentTime; // End last segment
            }
			int nextArrival = nextArrivalTime(p, currentTime); // Find next arrival time
			gantt.push_back({ -1, currentTime, nextArrival, true, false }); // Idle segment
            currentTime = nextArrival;
            lastProc = nullptr;
        }
    }
}

// Priority Scheduling
void PriorityScheduling(list<Process>& p, list<Segment>& gantt) {
	int currentTime = 0; // Current time
    while (!allCompleted(p)) {
        Process* currentProc = findHighestPriority(p, currentTime);
        if (currentProc) {
            if (currentProc->startTime == -1) {
				currentProc->startTime = currentTime; // Set start time
				currentProc->responseTime = currentTime - currentProc->arrivalTime; // Set response time
            }
			gantt.push_back({ currentProc->pid, currentTime, currentTime + currentProc->burstTime, false, false }); // Gantt segment
			currentTime += currentProc->burstTime; // Update current time
			currentProc->completionTime = currentTime; // Set completion time
			currentProc->isCompleted = true; // Mark as completed
        }
        else {
            int nextArrival = nextArrivalTime(p, currentTime);
			if (nextArrival == numeric_limits<int>::max()) break; // No more arrivals
			gantt.push_back({ -1, currentTime, nextArrival, true, false }); // Idle segment
			currentTime = nextArrival; // Update current time
        }

    }
}

// ================================================================
//               FILE SAVING / LOGGING
// ================================================================
// Function to save run details to a text file
void saveRunToFile(const list<Process>& p,
	const list<Segment>& gantt, // Gantt chart
	const string& algorithm) { // Scheduling algorithm name

	const string filename = "cpu_scheduling_history.txt"; // Log file name
	int runNum = getNextRunNumber(filename); // Get next run number

	time_t now = time(nullptr); // Get current time
	char timeBuffer[26]; // Buffer for formatted time
	ctime_s(timeBuffer, sizeof(timeBuffer), &now); // Format time

	string timestamp(timeBuffer); // Convert to string
	if (!timestamp.empty() && timestamp.back() == '\n') // Remove trailing newline
        timestamp.pop_back();

    ofstream file(filename, ios::app);
	if (!file.is_open()) { // Check if file opened successfully
        cout << "Failed to open TXT log file.\n";
        return;
    }

    file << "====================================================\n";
    file << "Run " << runNum
        << " | Algorithm: " << algorithm
        << " | Timestamp: " << timestamp << "\n";
    file << "====================================================\n\n";

    file << "PID\tAT\tBT\tCT\tWT\tTAT\tRT\n";
	for (const auto& proc : p) { // Write process details
        file << proc.pid << "\t"
            << proc.arrivalTime << "\t"
            << proc.burstTime << "\t"
            << proc.completionTime << "\t"
            << proc.waitingTime << "\t"
            << proc.turnaroundTime << "\t"
            << proc.responseTime << "\n";
    }

    file << "\nGantt Chart:\n";
    for (const auto& seg : gantt) {
		int len = seg.end - seg.start; // Length of segment
		file << (seg.isIdle ? "Idle " : "P" + to_string(seg.pid) + " "); // Segment label
		for (int i = 0; i < len; i++) file << "█"; // Visual representation
		file << " (" << seg.start << "-" << seg.end << ")\n"; // Segment time
    }

	double cpuUtil = calculateCPUUtilization(const_cast<list<Segment>&>(gantt)); // CPU Utilization
	double throughput = calculateThroughput(const_cast<list<Process>&>(p), gantt.back().end); // Throughput
	double avgWT = calculateAverageWaitingTime(const_cast<list<Process>&>(p)); // Average Waiting Time
	double avgTAT = calculateAverageTurnaroundTime(const_cast<list<Process>&>(p)); // Average Turnaround Time
	int maxWT = calculateMaxWaitingTime(const_cast<list<Process>&>(p)); // Maximum Waiting Time
	int maxTAT = calculateMaxTurnaroundTime(const_cast<list<Process>&>(p)); // Maximum Turnaround Time

	file << fixed << setprecision(2); // Set precision for floating point
    file << "\nCPU Utilization: " << cpuUtil << "%\n";
    file << "Throughput: " << throughput << " processes/unit time\n";
    file << "Average Waiting Time: " << avgWT << "\n";
    file << "Average Turnaround Time: " << avgTAT << "\n";
    file << "Maximum Waiting Time: " << maxWT << "\n";
    file << "Maximum Turnaround Time: " << maxTAT << "\n\n";

	file.close(); // Close file
}

// Function to export run details to an XML file
void exportRunToXML(const list<Process>& p,
	const list<Segment>& gantt, // Gantt chart
    const string& algorithm) {

	const string filename = "cpu_scheduling_history.xml"; // XML log file name
	int runNum = getNextRunNumber(filename); // Get next run number

	ifstream inFile(filename); // Open existing XML file
    string content;

    if (inFile.is_open()) {
		content.assign((istreambuf_iterator<char>(inFile)), // Read entire file content
            istreambuf_iterator<char>());
        inFile.close();
    }

    if (content.find("<Runs>") == string::npos) {
		content = "<?xml version=\"1.0\"?>\n<Runs>\n</Runs>\n"; // Initialize XML structure
    }

    size_t pos = content.rfind("</Runs>");
	if (pos != string::npos) // Find closing tag
        content.erase(pos);

    content += "  <Run number=\"" + to_string(runNum) +
        "\" algorithm=\"" + algorithm + "\">\n";

    content += "    <Processes>\n";
	for (const auto& proc : p) { // Write process details
        content += "      <Process>\n";
        content += "        <PID>" + to_string(proc.pid) + "</PID>\n";
        content += "        <ArrivalTime>" + to_string(proc.arrivalTime) + "</ArrivalTime>\n";
        content += "        <BurstTime>" + to_string(proc.burstTime) + "</BurstTime>\n";
        content += "        <CompletionTime>" + to_string(proc.completionTime) + "</CompletionTime>\n";
        content += "        <WaitingTime>" + to_string(proc.waitingTime) + "</WaitingTime>\n";
        content += "        <TurnaroundTime>" + to_string(proc.turnaroundTime) + "</TurnaroundTime>\n";
        content += "        <ResponseTime>" + to_string(proc.responseTime) + "</ResponseTime>\n";
        content += "      </Process>\n";
    }
    content += "    </Processes>\n";

    content += "    <GanttChart>\n";
	for (const auto& seg : gantt) { // Write Gantt chart segments
        content += "      <Segment>\n";
        content += "        <PID>" + to_string(seg.isIdle ? -1 : seg.pid) + "</PID>\n";
        content += "        <Start>" + to_string(seg.start) + "</Start>\n";
        content += "        <End>" + to_string(seg.end) + "</End>\n";
        content += "        <IsIdle>" + string(seg.isIdle ? "true" : "false") + "</IsIdle>\n";
        content += "        <IsPreempted>" + string(seg.isPreempted ? "true" : "false") + "</IsPreempted>\n";
        content += "      </Segment>\n";
    }
    content += "    </GanttChart>\n";

	content += "  </Run>\n</Runs>\n"; // Close XML structure

    ofstream outFile(filename);
	outFile << content; // Write updated content to file
	outFile.close(); // Close file
}

// Function to get next run number from file
int getNextRunNumber(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return 1;

	int count = 0; // Count existing runs
	string line; // Line buffer
    while (getline(file, line)) {
		if (line.find("Run ") == 0) count++; // Increment count for each run
    }
    file.close();
    return count + 1;
}


// ================================================================
//						   MAIN FUNCTION
// ================================================================
// Main function
int main() {
    char repeat = 'y';
	// Main loop to allow multiple runs
    while (repeat == 'y' || repeat == 'Y') {
        int n;
        while (true) {
            cout << "Enter number of processes: ";
			if (cin >> n && n > 0) break; // Valid input
            else {
                cout << "Invalid input! Please enter a positive integer.\n";
                cin.clear(); 
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            }
        }

		list<Process> processes = inputProcesses(n); // Input processes
		sortByArrival(processes); // Sort processes by arrival time
		list<Segment> gantt; // Gantt chart segments
        int choice;
        while (true) {
            cout << "\nSelect Scheduling Algorithm:\n";
            cout << "1. First-Come, First-Served (FCFS)\n";
            cout << "2. Shortest Job First (SJF)\n";
            cout << "3. Shortest Remaining Time First (SRTF)\n";
            cout << "4. Priority Scheduling\n";
            cout << "Enter choice (1-4): ";
			if (cin >> choice && choice >= 1 && choice <= 4) break; // Valid choice
            else {
                cout << "Invalid choice! Please enter a number between 1 and 4.\n";
                cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear invalid input
            }
        }

		// Execute selected scheduling algorithm
        switch (choice) {
        case 1: FCFS(processes, gantt); break;
        case 2: SJF(processes, gantt); break;
        case 3: SRTF(processes, gantt); break;
        case 4: PriorityScheduling(processes, gantt); break;
        default:
            cout << "Invalid choice!" << endl;
            continue;
        }

		calculateTimes(processes); // Calculate waiting and turnaround times
		printProcessTable(processes); // Print process table
		printGanttChart(gantt); // Print Gantt chart
        double cpuUtil = calculateCPUUtilization(gantt);
        double throughput = calculateThroughput(processes, gantt.back().end);
        double avgWT = calculateAverageWaitingTime(processes);
        double avgTAT = calculateAverageTurnaroundTime(processes);
        int maxWT = calculateMaxWaitingTime(processes);
        int maxTAT = calculateMaxTurnaroundTime(processes);
        cout << fixed << setprecision(2);
        cout << GREEN << "\nCPU Utilization: " << RESET << cpuUtil << "%\n";
        cout << GREEN << "Throughput: " << RESET << throughput << " processes/unit time\n";
        cout << YELLOW << "Average Waiting Time: " << RESET << avgWT << "\n";
        cout << MAGENTA << "Average Turnaround Time: " << RESET << avgTAT << "\n";
        cout << RED << "Maximum Waiting Time: " << RESET << maxWT << "\n";
        cout << BLUE << "Maximum Turnaround Time: " << RESET << maxTAT << "\n";

		// Save results to files
        string algName;
        switch (choice) {
        case 1: algName = "FCFS"; break;
        case 2: algName = "SJF"; break;
        case 3: algName = "SRTF"; break;
        case 4: algName = "Priority"; break;
        }

        saveRunToFile(processes, gantt, algName);
        exportRunToXML(processes, gantt, algName);

		// Prompt to repeat or exit
        while (true) {
            cout << "\nDo you want to schedule another set of processes? (y/n): ";
            cin >> repeat;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (repeat == 'y' || repeat == 'Y' || repeat == 'n' || repeat == 'N') break;
            else cout << "Invalid input! Please enter 'y' or 'n'.\n";
        }
        cout << "\n-----------------------------------------\n";
    }

    cout << "Exiting program.\n";
	return 0; // Exit program
}
