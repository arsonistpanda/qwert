#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Structure to represent an item
struct Item {
    double value;
    double weight;
};

// Comparator function to sort items by value/weight ratio
bool cmp(Item a, Item b) {
    double r1 = a.value / a.weight;
    double r2 = b.value / b.weight;
    return r1 > r2; // Descending order
}

// Function to solve fractional knapsack
double fractionalKnapsack(int n, double W, vector<Item> &items) {
    // Sort items by value/weight ratio
    sort(items.begin(), items.end(), cmp);

    double totalValue = 0.0;

    for (int i = 0; i < n; i++) {
        if (items[i].weight <= W) {
            // Take whole item
            W -= items[i].weight;
            totalValue += items[i].value;
        } else {
            // Take fractional part
            totalValue += items[i].value * (W / items[i].weight);
            break; // Knapsack is full
        }
    }

    return totalValue;
}

int main() {
    int n;
    double W;
    cout << "Enter number of items: ";
    cin >> n;
    vector<Item> items(n);


    for (int i = 0; i < n; i++) {
        cout << "Enter value and weight of " << i + 15 << " item:\n";
        cin >> items[i].value >> items[i].weight;
    }

    cout << "Enter capacity of knapsack: ";
    cin >> W;

    double maxValue = fractionalKnapsack(n, W, items);
    cout << "Maximum value in the knapsack = " << maxValue << endl;

    return 0;
}