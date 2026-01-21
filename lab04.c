#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

// Cấu trúc mới để lưu vết Gantt Chart cho việc in ấn đẹp hơn
typedef struct {
    int iPID;
    int iStart;
    int iFinish;
} GanttSegment;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("\n=== Process %d ===\n", i + 1);
        P[i].iPID = i + 1;
        printf("Arrival Time: ");
        scanf("%d", &P[i].iArrival);
        printf("Burst Time: ");
        scanf("%d", &P[i].iBurst);
    }
}

void generateRandomProcess(int n, PCB P[]) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;
        P[i].iBurst = rand() % 11 + 2;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\n+-----+----------+------------+\n");
    printf("| PID | Arrival  | Burst Time |\n");
    printf("+-----+----------+------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d |    %-5d |      %-5d |\n", 
               P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
    printf("+-----+----------+------------+\n");
}

void printResult(int n, PCB P[]) {
    printf("\n+-----+-------+-------+-------+--------+---------+----------+--------------+\n");
    printf("| PID | Arr.  | Burst | Start | Finish | Waiting | Response | Turnaround   |\n");
    printf("+-----+-------+-------+-------+--------+---------+----------+--------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d |  %-4d |  %-4d |  %-4d |   %-4d |   %-5d |    %-5d |      %-7d |\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst, 
               P[i].iStart, P[i].iFinish, P[i].iWaiting, 
               P[i].iResponse, P[i].iTaT);
    }
    printf("+-----+-------+-------+-------+--------+---------+----------+--------------+\n");
}

// --- HÀM IN GANTT CHART ĐÃ SỬA ---
void exportGanttChart(int count, GanttSegment gantt[]) {
    printf("\n=== GANTT CHART ===\n");
    
    // In thanh trên cùng
    printf(" ");
    for (int i = 0; i < count; i++) printf("-------");
    printf("\n|");

    // In Process ID
    for (int i = 0; i < count; i++) {
        printf("  P%-2d |", gantt[i].iPID);
    }
    printf("\n");

    // In thanh dưới cùng của ô
    printf(" ");
    for (int i = 0; i < count; i++) printf("-------");
    printf("\n");

    // In dòng thời gian (quan trọng!)
    printf("0"); // Thời điểm bắt đầu của biểu đồ (giả sử luôn bắt đầu từ 0 hoặc iStart đầu tiên)
    // Thực tế nên in gantt[0].iStart nếu process đầu không chạy từ 0
    if (gantt[0].iStart != 0) {
        printf("\b%-7d", gantt[0].iStart); // Backspace để xóa số 0 nếu cần, hoặc xử lý logic idle
    }
    
    // In mốc thời gian kết thúc của từng segment
    // Lưu ý: %-7d phải khớp với độ rộng của "  P%-2d |" (khoảng 7 ký tự)
    for (int i = 0; i < count; i++) {
        // Xử lý khoảng cách timeline: Nếu start của task sau > finish task trước => có idle time
        if (i > 0 && gantt[i].iStart > gantt[i-1].iFinish) {
             // Logic in idle time nếu muốn, ở đây ta in liền mạch finish time
        }
        printf("%-7d", gantt[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

// ... (Giữ nguyên các hàm partition, quickSort, calculateAWT, calculateATaT) ...
int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int compare = 0;
        switch (iCriteria) {
            case SORT_BY_ARRIVAL: compare = (P[j].iArrival < pivot.iArrival); break;
            case SORT_BY_PID: compare = (P[j].iPID < pivot.iPID); break;
            case SORT_BY_BURST: compare = (P[j].iBurst < pivot.iBurst); break;
            case SORT_BY_START: compare = (P[j].iStart < pivot.iStart); break;
        }
        if (compare) {
            i++;
            PCB temp = P[i]; P[i] = P[j]; P[j] = temp;
        }
    }
    PCB temp = P[i + 1]; P[i + 1] = P[high]; P[high] = temp;
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    float total = 0;
    for (int i = 0; i < n; i++) total += P[i].iWaiting;
    printf("\nAverage Waiting Time: %.2f\n", total / n);
}

void calculateATaT(int n, PCB P[]) {
    float total = 0;
    for (int i = 0; i < n; i++) total += P[i].iTaT;
    printf("Average Turnaround Time: %.2f\n", total / n);
}

// --- CẬP NHẬT SJF ---
void SJF_Scheduling(int n, PCB P[]) {
    PCB Input[10], ReadyQueue[10], TerminatedArray[10];
    int iRemain = n, iReady = 0, iTerminated = 0;
    int currentTime = 0;
    
    // Mảng lưu vết Gantt
    GanttSegment gantt[100];
    int gCount = 0;

    for (int i = 0; i < n; i++) Input[i] = P[i];
    quickSort(Input, 0, n - 1, SORT_BY_ARRIVAL);
    
    // Xử lý trường hợp process đầu tiên không đến tại 0
    if (Input[0].iArrival > 0) currentTime = Input[0].iArrival;

    while (iTerminated < n) {
        while (iRemain > 0 && Input[0].iArrival <= currentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }
        
        if (iReady == 0 && iRemain > 0) {
            currentTime = Input[0].iArrival;
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }
        
        if (iReady > 1) quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);
        
        if (iReady > 0) {
            ReadyQueue[0].iStart = currentTime;
            ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            
            // Lưu vào Gantt
            gantt[gCount].iPID = ReadyQueue[0].iPID;
            gantt[gCount].iStart = ReadyQueue[0].iStart;
            gantt[gCount].iFinish = ReadyQueue[0].iFinish;
            gCount++;

            currentTime = ReadyQueue[0].iFinish;
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }
    }
    
    printf("\n========== SJF SCHEDULING ==========\n");
    exportGanttChart(gCount, gantt); // Gọi hàm in mới
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printResult(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
}

// --- CẬP NHẬT SRTF ---
void SRTF_Scheduling(int n, PCB P[]) {
    typedef struct {
        PCB process;
        int remainingBurst;
    } ProcessSRTF;
    
    ProcessSRTF processes[10];
    PCB TerminatedArray[10];
    int iTerminated = 0;
    int currentTime = 0;
    int completed = 0;
    int lastProcess = -1; // -1 means IDLE
    
    // Mảng lưu vết Gantt
    GanttSegment gantt[100];
    int gCount = 0;

    for (int i = 0; i < n; i++) {
        processes[i].process = P[i];
        processes[i].remainingBurst = P[i].iBurst;
        processes[i].process.iStart = -1;
    }
    
    // Check min arrival để set currentTime ban đầu (nếu muốn)
    int minArrival = 9999;
    for(int i=0; i<n; i++) if(P[i].iArrival < minArrival) minArrival = P[i].iArrival;
    // currentTime = minArrival; // Nếu muốn tua nhanh đến lúc có process

    while (completed < n) {
        int idx = -1;
        int minRemaining = 9999;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].process.iArrival <= currentTime && 
                processes[i].remainingBurst > 0 &&
                processes[i].remainingBurst < minRemaining) {
                minRemaining = processes[i].remainingBurst;
                idx = i;
            }
        }
        
        if (idx == -1) {
            // Trường hợp IDLE (CPU rảnh)
            if (lastProcess != -1) {
                 // Đóng segment trước đó
                 gantt[gCount-1].iFinish = currentTime;
                 lastProcess = -1; 
            }
            currentTime++;
            continue;
        }
        
        // Phát hiện chuyển đổi ngữ cảnh (Context Switch)
        if (idx != lastProcess) {
            if (lastProcess != -1) {
                // Kết thúc segment cũ
                gantt[gCount-1].iFinish = currentTime;
            }
            // Mở segment mới
            gantt[gCount].iPID = processes[idx].process.iPID;
            gantt[gCount].iStart = currentTime;
            gantt[gCount].iFinish = currentTime + 1; // Tạm tính
            gCount++;
            lastProcess = idx;
        } else {
            // Vẫn là process cũ, cập nhật thời gian finish tạm thời
             if (gCount > 0) gantt[gCount-1].iFinish = currentTime + 1;
        }
        
        if (processes[idx].process.iStart == -1) {
            processes[idx].process.iStart = currentTime;
        }
        
        processes[idx].remainingBurst--;
        currentTime++;
        
        if (processes[idx].remainingBurst == 0) {
            completed++;
            processes[idx].process.iFinish = currentTime;
            processes[idx].process.iWaiting = processes[idx].process.iFinish - 
                                              processes[idx].process.iArrival - 
                                              processes[idx].process.iBurst;
            processes[idx].process.iTaT = processes[idx].process.iFinish - 
                                          processes[idx].process.iArrival;
            processes[idx].process.iResponse = processes[idx].process.iStart - 
                                               processes[idx].process.iArrival;
            TerminatedArray[iTerminated++] = processes[idx].process;
            // Lưu ý: finish của segment đã được update ở dòng gantt...iFinish = currentTime + 1
        }
    }
    
    // Chốt segment cuối cùng nếu cần (thường đã được update trong loop)
    if (gCount > 0) gantt[gCount-1].iFinish = currentTime;

    printf("\n========== SRTF SCHEDULING ==========\n");
    exportGanttChart(gCount, gantt);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printResult(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
}

// --- CẬP NHẬT RR ---
void RR_Scheduling(int n, PCB P[], int quantum) {
    typedef struct {
        PCB process;
        int remainingBurst;
        int inQueue;
    } ProcessRR;
    
    ProcessRR processes[10];
    int queue[100];
    int front = 0, rear = 0;
    PCB TerminatedArray[10];
    int iTerminated = 0;
    int currentTime = 0;
    
    GanttSegment gantt[100];
    int gCount = 0;
    
    for (int i = 0; i < n; i++) {
        processes[i].process = P[i];
        processes[i].remainingBurst = P[i].iBurst;
        processes[i].process.iStart = -1;
        processes[i].inQueue = 0;
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (processes[i].process.iArrival > processes[j].process.iArrival) {
                ProcessRR temp = processes[i]; processes[i] = processes[j]; processes[j] = temp;
            }
        }
    }

    if (processes[0].process.iArrival > 0) currentTime = processes[0].process.iArrival;
    
    for (int i = 0; i < n; i++) {
        if (processes[i].process.iArrival <= currentTime) {
            queue[rear++] = i;
            processes[i].inQueue = 1;
        }
    }

    printf("\n========== RR SCHEDULING (Quantum = %d) ==========\n", quantum);
    
    while (front < rear) {
        int idx = queue[front++];
        
        if (processes[idx].process.iStart == -1) {
            processes[idx].process.iStart = currentTime;
        }
        
        int timeSlice = (processes[idx].remainingBurst < quantum) ? 
                        processes[idx].remainingBurst : quantum;
        
        gantt[gCount].iPID = processes[idx].process.iPID;
        gantt[gCount].iStart = currentTime;
        gantt[gCount].iFinish = currentTime + timeSlice;
        gCount++;

        processes[idx].remainingBurst -= timeSlice;
        currentTime += timeSlice;
        
        for (int i = 0; i < n; i++) {
            if (!processes[i].inQueue && 
                processes[i].process.iArrival <= currentTime && 
                processes[i].remainingBurst > 0) {
                queue[rear++] = i;
                processes[i].inQueue = 1;
            }
        }
        
        if (processes[idx].remainingBurst > 0) {
            queue[rear++] = idx;
        } else {
            processes[idx].process.iFinish = currentTime;
            processes[idx].process.iWaiting = processes[idx].process.iFinish - 
                                              processes[idx].process.iArrival - 
                                              processes[idx].process.iBurst;
            processes[idx].process.iTaT = processes[idx].process.iFinish - 
                                          processes[idx].process.iArrival;
            processes[idx].process.iResponse = processes[idx].process.iStart - 
                                               processes[idx].process.iArrival;
            
            TerminatedArray[iTerminated++] = processes[idx].process;
        }
    }
    
    exportGanttChart(gCount, gantt);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printResult(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
}

int main() {
    PCB P[10];
    int n, choice, quantum;
    
    printf("========================================\n");
    printf("   CPU SCHEDULING ALGORITHMS SIMULATOR  \n");
    printf("========================================\n");
    printf("Nhap so luong process: ");
    scanf("%d", &n);
    
    if (n <= 0 || n > 10) {
        printf("So luong process khong hop le!\n");
        return 1;
    }
    
    generateRandomProcess(n, P);
    printf("\n=== Du lieu dau vao ===");
    printProcess(n, P);
    
    printf("\nChon giai thuat:\n");
    printf("1. SJF (Shortest Job First)\n");
    printf("2. SRTF (Shortest Remaining Time First)\n");
    printf("3. RR (Round Robin)\n");
    printf("4. Chay tat ca\n");
    printf("Lua chon: ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1: SJF_Scheduling(n, P); break;
        case 2: SRTF_Scheduling(n, P); break;
        case 3: 
            printf("\nNhap Quantum Time: ");
            scanf("%d", &quantum);
            RR_Scheduling(n, P, quantum); 
            break;
        case 4:
            SJF_Scheduling(n, P);
            SRTF_Scheduling(n, P);
            printf("\nNhap Quantum Time cho RR: ");
            scanf("%d", &quantum);
            RR_Scheduling(n, P, quantum);
            break;
        default: printf("Lua chon khong hop le!\n");
    }
    return 0;
}