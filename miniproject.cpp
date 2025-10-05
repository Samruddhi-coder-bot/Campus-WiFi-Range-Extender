#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_HOSTELS 15   // Limit for DP feasibility (2^15 = 32768)
#define RADIUS 50.0      // Coverage radius in meters

// Structure for hostel coordinates
typedef struct {
    double x, y;
} Point;

// Calculate Euclidean distance
double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.y));
}

// Returns 1 if hostel j is covered by router at hostel i
int covers(Point router, Point hostel) {
    double dx = router.x - hostel.x;
    double dy = router.y - hostel.y;
    return (sqrt(dx*dx + dy*dy) <= RADIUS);
}

// ----------------------------
// GREEDY SET COVER APPROACH
// ----------------------------
int greedyPlacement(Point hostels[], int n, int routerIndex[]) {
    int covered[MAX_HOSTELS] = {0};
    int routers = 0;
    int totalCovered = 0;

    while (totalCovered < n) {
        int best = -1, bestCover = 0;
        int tempCovered[MAX_HOSTELS] = {0};

        for (int i = 0; i < n; i++) {
            int count = 0;
            for (int j = 0; j < n; j++) {
                if (!covered[j] && covers(hostels[i], hostels[j])) {
                    count++;
                    tempCovered[j] = 1;
                }
            }
            if (count > bestCover) {
                bestCover = count;
                best = i;
            }
        }

        if (best == -1) break; // no progress
        routerIndex[routers++] = best;

        for (int j = 0; j < n; j++) {
            if (covers(hostels[best], hostels[j]) && !covered[j]) {
                covered[j] = 1;
                totalCovered++;
            }
        }
    }
    return routers;
}

// ----------------------------
// DP BITMASK OPTIMAL SOLUTION
// ----------------------------
int dpPlacement(Point hostels[], int n, int routerIndex[]) {
    int coverage[MAX_HOSTELS]; // coverage bitmask for each hostel

    for (int i = 0; i < n; i++) {
        coverage[i] = 0;
        for (int j = 0; j < n; j++) {
            if (covers(hostels[i], hostels[j])) {
                coverage[i] |= (1 << j);
            }
        }
    }

    int fullMask = (1 << n) - 1;
    int dp[1 << MAX_HOSTELS];
    int parent[1 << MAX_HOSTELS];
    int usedRouter[1 << MAX_HOSTELS];

    for (int i = 0; i < (1 << n); i++) dp[i] = 1e9;
    dp[0] = 0;

    for (int mask = 0; mask <= fullMask; mask++) {
        if (dp[mask] == 1e9) continue;
        for (int i = 0; i < n; i++) {
            int newMask = mask | coverage[i];
            if (dp[newMask] > dp[mask] + 1) {
                dp[newMask] = dp[mask] + 1;
                parent[newMask] = mask;
                usedRouter[newMask] = i;
            }
        }
    }

    // Trace back chosen routers
    int routers = 0;
    int mask = fullMask;
    while (mask) {
        routerIndex[routers++] = usedRouter[mask];
        mask = parent[mask];
    }

    return routers;
}

// ----------------------------
// MAIN FUNCTION
// ----------------------------
int main() {
    int n;
    Point hostels[MAX_HOSTELS];
    int greedyRouters[MAX_HOSTELS], dpRouters[MAX_HOSTELS];

    printf("Enter number of hostels (<=15 for DP): ");
    scanf("%d", &n);

    printf("Enter hostel coordinates (x y):\n");
    for (int i = 0; i < n; i++) {
        printf("Hostel %d: ", i + 1);
        scanf("%lf %lf", &hostels[i].x, &hostels[i].y);
    }

    // Run Greedy
    int gCount = greedyPlacement(hostels, n, greedyRouters);
    printf("\n--- GREEDY APPROACH ---\n");
    printf("Routers required: %d\n", gCount);
    for (int i = 0; i < gCount; i++) {
        printf("Router %d placed at Hostel %d (%.2f, %.2f)\n", i + 1, greedyRouters[i] + 1,
               hostels[greedyRouters[i]].x, hostels[greedyRouters[i]].y);
    }

    // Run DP
    int dCount = dpPlacement(hostels, n, dpRouters);
    printf("\n--- DP OPTIMAL APPROACH ---\n");
    printf("Routers required: %d\n", dCount);
    for (int i = 0; i < dCount; i++) {
        printf("Router %d placed at Hostel %d (%.2f, %.2f)\n", i + 1, dpRouters[i] + 1,
               hostels[dpRouters[i]].x, hostels[dpRouters[i]].y);
    }

    printf("\nComparison:\n");
    printf("Greedy: %d routers | DP Optimal: %d routers\n", gCount, dCount);

    return 0;
}

