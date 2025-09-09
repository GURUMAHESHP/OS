#include <stdio.h>
#include <limits.h>

struct Process {
    int pid;    // Process ID
    int at;     // Arrival Time
    int bt;     // Burst Time
    int pr;     // Priority (smaller value = higher priority)
    int ct;     // Completion Time
    int tat;    // Turnaround Time
    int wt;     // Waiting Time
    int done;   // finished or not
};

int main() {
    int n, time = 0, complete = 0;
    float total_tat = 0, total_wt = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter Arrival time, Burst time and Priority for P%d: ", p[i].pid);
        scanf("%d %d %d", &p[i].at, &p[i].bt, &p[i].pr);
        p[i].done = 0; // initially not finished
    }

    printf("\nGantt Chart:\n");

    while (complete < n) {
        int idx = -1;
        int bestPr = INT_MAX;

        // find process with highest priority (lowest pr value) among arrived ones
        for (int i = 0; i < n; i++) {
            if (!p[i].done && p[i].at <= time) {
                if (p[i].pr < bestPr) {
                    bestPr = p[i].pr;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            printf(" | idle ");
            time++;
        } else {
            printf(" | P%d ", p[idx].pid);
            time += p[idx].bt;
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;

            total_tat += p[idx].tat;
            total_wt += p[idx].wt;

            p[idx].done = 1;
            complete++;
        }
    }
    printf("|\n");

    // Print process table
    printf("\nPID\tAT\tBT\tPR\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt, p[i].pr,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f\n", total_tat / n);
    printf("Average WT  = %.2f\n", total_wt / n);

    return 0;
}