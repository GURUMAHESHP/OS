#include <stdio.h>
#include <limits.h>

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
    int n, time = 0, complete = 0, shortest;
    float total_tat = 0, total_wt = 0;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];

    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter Arrival time and Burst time for P%d: ", p[i].pid);
        scanf("%d %d", &p[i].at, &p[i].bt);
        p[i].rt = p[i].bt; // initially remaining = burst time
    }

    printf("\nGantt Chart:\n");

    while (complete != n) {
        shortest = -1;
        int min_rt = INT_MAX;

        // find process with shortest remaining time
        for (int i = 0; i < n; i++) {
            if (p[i].at <= time && p[i].rt > 0) {
                if (p[i].rt < min_rt) {
                    min_rt = p[i].rt;
                    shortest = i;
                }
            }
        }

        if (shortest == -1) {
            // CPU idle
            printf(" | idle ");
            time++;
        } else {
            printf(" | P%d ", p[shortest].pid);
            p[shortest].rt--;
            time++;

            if (p[shortest].rt == 0) {
                complete++;
                p[shortest].ct = time;
                p[shortest].tat = p[shortest].ct - p[shortest].at;
                p[shortest].wt = p[shortest].tat - p[shortest].bt;

                total_tat += p[shortest].tat;
                total_wt += p[shortest].wt;
            }
        }
    }

    printf("|\n");

    // print table
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
