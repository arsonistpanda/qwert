
#include <bits/stdc++.h>
#include <thread>
#include <atomic>
using namespace std;
using Clock = chrono::high_resolution_clock;
using ms = chrono::duration<double, milli>;

// ---------- Utility ----------
void printArray(const vector<int>& a, const string& name) {
    if (a.size() > 32) {
        cout << name << " (size=" << a.size() << ") [skipped printing]\n";
        return;
    }
    cout << name << ":\n";
    for (size_t i = 0; i < a.size(); ++i) {
        cout << a[i] << (i + 1 < a.size() ? "\t" : "\n");
    }
    cout << '\n';
}

// ---------- Merge (shared) ----------
void mergeRanges(vector<int>& a, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = a[l + i];
    for (int j = 0; j < n2; ++j) R[j] = a[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) a[k++] = L[i++];
        else a[k++] = R[j++];
    }
    while (i < n1) a[k++] = L[i++];
    while (j < n2) a[k++] = R[j++];
}

// ---------- Single-threaded merge sort ----------
void mergeSortSeq(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortSeq(a, l, m);
    mergeSortSeq(a, m + 1, r);
    mergeRanges(a, l, m, r);
}

void mergeSortSeq(vector<int>& a) {
    if (!a.empty()) mergeSortSeq(a, 0, (int)a.size() - 1);
}

// ---------- Multithreaded merge sort ----------
void mergeSortMT_rec(vector<int>& a, int l, int r, int depth, int max_depth) {
    if (l >= r) return;
    int m = l + (r - l) / 2;

    if (depth >= max_depth) {
        // Too deep to create more threads -> do sequentially
        mergeSortSeq(a, l, m);
        mergeSortSeq(a, m + 1, r);
    } else {
        // Spawn a thread for one half and compute the other half here
        std::thread left_thr(mergeSortMT_rec, std::ref(a), l, m, depth + 1, max_depth);
        // Compute right half in current thread
        mergeSortMT_rec(a, m + 1, r, depth + 1, max_depth);
        left_thr.join();
    }
    mergeRanges(a, l, m, r);
}

void mergeSortMT(vector<int>& a, int max_depth) {
    if (a.empty()) return;
    mergeSortMT_rec(a, 0, (int)a.size() - 1, 0, max_depth);
}

// ---------- Helpers for tests ----------
enum class InputType { RANDOM, SORTED, REVERSE };

vector<int> makeInput(size_t N, InputType t, std::mt19937& rng) {
    vector<int> a(N);
    if (t == InputType::RANDOM) {
        uniform_int_distribution<int> d(0, 1'000'000);
        for (size_t i = 0; i < N; ++i) a[i] = d(rng);
    } else if (t == InputType::SORTED) {
        for (size_t i = 0; i < N; ++i) a[i] = (int)i;
    } else { // REVERSE
        for (size_t i = 0; i < N; ++i) a[i] = (int)(N - i);
    }
    return a;
}

double timeFunction(function<void()> fn, int repeats = 1) {
    // run fn repeats times and return average ms
    double total = 0.0;
    for (int i = 0; i < repeats; ++i) {
        auto s = Clock::now();
        fn();
        auto e = Clock::now();
        total += ms(e - s).count();
    }
    return total / repeats;
}

// ---------- Main experiment ----------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Configuration
    vector<int> sizes = {1000, 5000, 20000}; // small -> medium -> large (adjust if you want)
    // You can add larger sizes like 100000 depending on your machine
    int repeats = 3; // average times over repeats
    mt19937 rng((unsigned)chrono::system_clock::now().time_since_epoch().count());

    unsigned hw = thread::hardware_concurrency();
    if (hw == 0) hw = 2;
    int max_depth = 0;
    // Choose max_depth roughly log2(hw). Each additional depth roughly doubles thread count.
    // We pick floor(log2(hw)) so we don't exceed available hardware threads.
    while ((1u << max_depth) < hw) ++max_depth;
    if (max_depth > 0) --max_depth; // be conservative

    cout << "Hardware concurrency (suggested threads): " << hw << "\n";
    cout << "Using max_depth (for MT recursion limit): " << max_depth << "\n\n";

    vector<InputType> types = {InputType::RANDOM, InputType::SORTED, InputType::REVERSE};
    vector<string> typeNames = {"Random", "Sorted", "Reverse"};

    for (size_t si = 0; si < sizes.size(); ++si) {
        int N = sizes[si];
        cout << "========================\n";
        cout << "Array size N = " << N << "\n";
        for (size_t t = 0; t < types.size(); ++t) {
            cout << "---- Input: " << typeNames[t] << " ----\n";
            vector<int> original = makeInput(N, types[t], rng);

            // If N small, print original
            if (N <= 32) printArray(original, "Original");

            // Single-threaded
            auto a1 = original;
            double t_seq = timeFunction([&]() {
                mergeSortSeq(a1);
            }, repeats);
            if (N <= 32) printArray(a1, "Sorted (Seq)");

            // Multithreaded
            auto a2 = original;
            double t_mt = timeFunction([&]() {
                mergeSortMT(a2, max_depth);
            }, repeats);
            if (N <= 32) printArray(a2, "Sorted (MT)");

            // Verify correctness
            if (a1 != a2) {
                cerr << "ERROR: Results differ between seq and mt!\n";
                return 1;
            }

            cout << "Avg Time (single-threaded): " << fixed << setprecision(3) << t_seq << " ms\n";
            cout << "Avg Time (multithreaded ): " << fixed << setprecision(3) << t_mt << " ms\n";

            double speedup = t_seq / t_mt;
            cout << "Speedup (seq / mt): " << setprecision(3) << speedup << "\n\n";
        }
    }

    // Quick summary notes printed
    cout << "Notes:\n";
    cout << "- Merge sort time complexity: O(n log n) in best, average, worst cases.\n";
    cout << "- Multithreading reduces wall-clock time when subproblems are large enough to amortize thread overhead\n";
    cout << "  and when hardware has multiple cores.\n";
    cout << "- For small N, multithreaded version may be slower due to thread creation/context switching.\n";
    cout << "- Tune 'max_depth' or add a cutoff for subarray size to get best practical performance.\n";
    cout << "- Space: merge sort uses O(n) extra space for merging (temporary arrays).\n";

    return 0;
}
