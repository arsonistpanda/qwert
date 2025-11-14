#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

int knapsack(int W, vector<int> &wt, vector<int> &val, int n, vector<vector<int>> &dp) {

    // Build the dp table
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                dp[i][w] = 0;
            else if (wt[i - 1] <= w)
                dp[i][w] = max(val[i - 1] + dp[i - 1][w - wt[i - 1]], dp[i - 1][w]);
            else
                dp[i][w] = dp[i - 1][w];
        }
    }

    return dp[n][W];
}

void printDPTable(vector<vector<int>> &dp, int n, int W) {
    cout << "\nDP TABLE:\n\n     ";
    for (int w = 0; w <= W; w++)
        cout << setw(3) << w << " ";
    cout << "\n";

    for (int i = 0; i <= n; i++) {
        cout << "i=" << i << " ";
        for (int w = 0; w <= W; w++) {
            cout << setw(3) << dp[i][w] << " ";
        }
        cout << "\n";
    }
}

vector<int> findSelectedItems(vector<vector<int>> &dp, vector<int> &wt, vector<int> &val, int n, int W) {
    vector<int> selected;
    int w = W;

    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            selected.push_back(i); // item index
            w -= wt[i - 1];
        }
    }

    return selected;
}

int main() {
    int n, W;
    cout << "Enter number of items: ";
    cin >> n;

    vector<int> val(n);
    vector<int> wt(n);

    cout << "Enter value and weight of each item:\n";
    for (int i = 0; i < n; i++)
        cin >> val[i] >> wt[i];

    cout << "Enter capacity of knapsack: ";
    cin >> W;

    // DP table
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    int maxValue = knapsack(W, wt, val, n, dp);
    cout << "\nMaximum value in 0-1 Knapsack = " << maxValue << endl;

    // Print DP table
    printDPTable(dp, n, W);

    // Reconstruct selected items
    vector<int> selected = findSelectedItems(dp, wt, val, n, W);

    cout << "\nItems selected (1-based index): ";
    for (int item : selected)
        cout << item << " ";

    cout << "\n";

    // Print details of selected items
    cout << "\nSelected items details:\n";
    for (int item : selected) {
        cout << "Item " << item << ": Value=" << val[item - 1] 
             << ", Weight=" << wt[item - 1] << "\n";
    }

    return 0;
}

