#include <stdio.h>

struct Process {
    int pid;    // Process ID
    int at;     // Arrival Time
    int bt;     // Burst Time
    int rt;     // Remaining Time
    int ct;     // Completion Time
    int tat;    // Turnaround Time
    int wt;     // Waiting Time
};

int main() {
    int n, tq, time = 0, complete = 0;
    float total_tat = 0, total_wt = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter Arrival time and Burst time for P%d: ", p[i].pid);
        scanf("%d %d", &p[i].at, &p[i].bt);
        p[i].rt = p[i].bt; // remaining time initially = burst time
    }

    printf("Enter Time Quantum: ");
    scanf("%d", &tq);

    printf("\nGantt Chart:\n");

    int done;
    while (complete < n) {
        done = 1;  // assume all are finished

        for (int i = 0; i < n; i++) {
            if (p[i].rt > 0 && p[i].at <= time) {
                done = 0;  // there is still a process left

                printf(" | P%d ", p[i].pid);

                if (p[i].rt > tq) {
                    time += tq;
                    p[i].rt -= tq;
                } else {
                    time += p[i].rt;
                    p[i].ct = time;
                    p[i].tat = p[i].ct - p[i].at;
                    p[i].wt = p[i].tat - p[i].bt;

                    total_tat += p[i].tat;
                    total_wt += p[i].wt;

                    p[i].rt = 0;
                    complete++;
                }
            }
        }

        if (done == 1) {
            time++; // idle time if no process has arrived
        }
    }
    printf("|\n");

    // Print process table
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f\n", total_tat / n);
    printf("Average WT  = %.2f\n", total_wt / n);

    return 0;
}