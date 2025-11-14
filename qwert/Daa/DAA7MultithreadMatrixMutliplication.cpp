#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

using namespace std;

// Function to multiply matrices (single-threaded)
void multiplySingleThread(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C) {
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int sum = 0;
            for (int k = 0; k < p; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Thread function for row-based multiplication
void multiplyRow(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int row) {
    int m = B[0].size();
    int p = B.size();

    for (int j = 0; j < m; ++j) {
        int sum = 0;
        for (int k = 0; k < p; ++k) {
            sum += A[row][k] * B[k][j];
        }
        C[row][j] = sum;
    }
}

// Thread function for cell-based multiplication
void multiplyCell(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int row, int col) {
    int p = B.size();
    int sum = 0;
    for (int k = 0; k < p; ++k) {
        sum += A[row][k] * B[k][col];
    }
    C[row][col] = sum;
}

int main() {
    int n = 600; // Rows in A
    int p = 600; // Cols in A / Rows in B
    int m = 600; // Cols in B

    // Random matrix generation
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 10);

    vector<vector<int>> A(n, vector<int>(p)), B(p, vector<int>(m)), C1(n, vector<int>(m)), C2(n, vector<int>(m)), C3(n, vector<int>(m));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < p; ++j)
            A[i][j] = dist(gen);

    for (int i = 0; i < p; ++i)
        for (int j = 0; j < m; ++j)
            B[i][j] = dist(gen);

    // Single-threaded
    auto start = chrono::high_resolution_clock::now();
    multiplySingleThread(A, B, C1);
    auto end = chrono::high_resolution_clock::now();
    auto singleThreadTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Single-threaded time: " << singleThreadTime << " ms\n";

    // Multi-threaded: one thread per row
    start = chrono::high_resolution_clock::now();
    {
        vector<thread> threads;
        for (int i = 0; i < n; ++i)
            threads.emplace_back(multiplyRow, cref(A), cref(B), ref(C2), i);
        for (auto& t : threads) t.join();
    }
    end = chrono::high_resolution_clock::now();
    auto rowThreadTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Multi-threaded (1 thread per row) time: " << rowThreadTime << " ms\n";

    // Multi-threaded: one thread per cell
    start = chrono::high_resolution_clock::now();
    {
        vector<thread> threads;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                threads.emplace_back(multiplyCell, cref(A), cref(B), ref(C3), i, j);
            }
        }
        for (auto& t : threads) t.join();
    }
    end = chrono::high_resolution_clock::now();
    auto cellThreadTime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Multi-threaded (1 thread per cell) time: " << cellThreadTime << " ms\n";

    return 0;
}
