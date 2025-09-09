#include <stdio.h>
#include <limits.h>

// Define process structure
struct Process {
    int pid;        // process id
    int at;         // arrival time
    int bt;         // burst time
    int rt;         // remaining time
    int ct;         // completion time
    int tat;        // turnaround time
    int wt;         // waiting time
};

int main() {
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process p[n];
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter Arrival time and Burst time for P%d: ", i + 1);
        scanf("%d %d", &p[i].at, &p[i].bt);
        p[i].rt = p[i].bt;
    }

    int complete = 0, time = 0, min_index;
    int total_tat = 0, total_wt = 0;

    // For Gantt Chart
    int gantt_time[100], gantt_pid[100], gcnt = 0;

    while (complete < n) {
        min_index = -1;
        int min_rt = INT_MAX;

        // find process with shortest remaining time
        for (int i = 0; i < n; i++) {
            if (p[i].at <= time && p[i].rt > 0 && p[i].rt < min_rt) {
                min_rt = p[i].rt;
                min_index = i;
            }
        }

        if (min_index == -1) {
            time++;
            continue;
        }

        // record gantt chart only when process changes
        if (gcnt == 0 || gantt_pid[gcnt - 1] != p[min_index].pid) {
            gantt_time[gcnt] = time;
            gantt_pid[gcnt] = p[min_index].pid;
            gcnt++;
        }

        // execute process for 1 unit time
        p[min_index].rt--;
        time++;

        if (p[min_index].rt == 0) {
            complete++;
            p[min_index].ct = time;
            p[min_index].tat = p[min_index].ct - p[min_index].at;
            p[min_index].wt = p[min_index].tat - p[min_index].bt;

            total_tat += p[min_index].tat;
            total_wt += p[min_index].wt;
        }
    }

    // record final time
    gantt_time[gcnt] = time;

    // print results
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f", (float)total_tat / n);
    printf("\nAverage WT  = %.2f\n", (float)total_wt / n);

    // print Gantt chart
    printf("\nGantt Chart:\n");
    for (int i = 0; i < gcnt; i++) {
        printf("| P%d ", gantt_pid[i]);
    }
    printf("|\n");

    for (int i = 0; i <= gcnt; i++) {
        printf("%d\t", gantt_time[i]);
    }
    printf("\n");

    return 0;
}