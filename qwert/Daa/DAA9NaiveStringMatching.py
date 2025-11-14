#!/usr/bin/env python3
"""
string_match_compare.py

Implements:
- naive_search(text, pattern)
- rabin_karp_search(text, pattern, base=256, mod=100000007)

Compares outputs and timings.
"""

import time
import random
import string
from typing import List, Tuple

# ------------------------------
# Naive (Brute-force) algorithm
# ------------------------------
def naive_search(text: str, pattern: str) -> List[int]:
    n, m = len(text), len(pattern)
    matches = []
    if m == 0:
        return list(range(n+1))  # match at every position if empty pattern
    for i in range(0, n - m + 1):
        j = 0
        while j < m and text[i + j] == pattern[j]:
            j += 1
        if j == m:
            matches.append(i)
    return matches

# -----------------------------------
# Rabin-Karp (rolling hash) algorithm
# -----------------------------------
def rabin_karp_search(text: str, pattern: str, base: int = 256, mod: int = 100000007) -> List[int]:
    """
    Returns list of starting indices where pattern matches text.
    Uses a rolling hash (modular arithmetic). Verifies candidate matches to avoid false positives.
    """
    n, m = len(text), len(pattern)
    matches = []
    if m == 0:
        return list(range(n+1))

    # Precompute base^(m-1) % mod
    power = 1
    for _ in range(m - 1):
        power = (power * base) % mod

    # Compute initial hashes
    h_text = 0
    h_pat = 0
    for i in range(m):
        h_text = (h_text * base + ord(text[i])) % mod
        h_pat  = (h_pat  * base + ord(pattern[i])) % mod

    # Slide over text
    for i in range(0, n - m + 1):
        # If hash matches, verify to avoid spurious match due to collision
        if h_text == h_pat:
            if text[i:i+m] == pattern:
                matches.append(i)
        # Roll the hash: remove leading char, add next char
        if i < n - m:
            left = (ord(text[i]) * power) % mod
            h_text = (h_text - left) % mod
            h_text = (h_text * base + ord(text[i + m])) % mod
            # ensure non-negative
            h_text = (h_text + mod) % mod
    return matches

# ------------------------------
# Utility: timing wrapper
# ------------------------------
def time_function(fn, *args, repeats: int = 1) -> Tuple[float, any]:
    """Executes fn(*args) repeats times, returns (avg_time_ms, last_result)"""
    total = 0.0
    last = None
    for _ in range(repeats):
        t0 = time.perf_counter()
        last = fn(*args)
        t1 = time.perf_counter()
        total += (t1 - t0) * 1000.0
    return total / repeats, last

# ------------------------------
# Demonstration harness
# ------------------------------
def demo_case(text: str, pattern: str, repeats: int = 3):
    print("="*60)
    print(f"Text length: {len(text)}, Pattern length: {len(pattern)}")
    print(f"Pattern: {repr(pattern)}\n")

    t_naive, res_naive = time_function(naive_search, text, pattern, repeats=repeats)
    print(f"Naive matches (count={len(res_naive)}): {res_naive[:20]}{'...' if len(res_naive)>20 else ''}")
    print(f"Naive avg time: {t_naive:.3f} ms (over {repeats} runs)")

    t_rk, res_rk = time_function(rabin_karp_search, text, pattern, repeats=repeats)
    print(f"Rabin-Karp matches (count={len(res_rk)}): {res_rk[:20]}{'...' if len(res_rk)>20 else ''}")
    print(f"Rabin-Karp avg time: {t_rk:.3f} ms (over {repeats} runs)")

    # Verify both produce same results
    if res_naive == res_rk:
        print("=> Both algorithms returned the same match positions.")
    else:
        print("! WARNING: Algorithms returned different results (bug or hash collision without verification).")
    print()

# ------------------------------
# Create test inputs
# ------------------------------
def main():
    random.seed(42)

    # 1) Typical random-like input
    text1 = "".join(random.choices(string.ascii_lowercase + " ", k=10000))
    pattern1 = "the"
    print("Case 1: Random-like text (typical case)")
    demo_case(text1, pattern1, repeats=3)

    # 2) Pathological worst-case for naive: text with repeated 'a', pattern 'aaaab'
    n = 20000
    text2 = "a" * n
    pattern2 = "a" * (n//2) + "b"  # large pattern that never matches (forces many comparisons)
    print("Case 2: Pathological worst-case for naive")
    demo_case(text2, pattern2, repeats=1)

    # 3) Pattern occurs many times (repeated overlaps)
    text3 = "abababababababababab" * 500  # repetitive text
    pattern3 = "ababab"
    print("Case 3: Highly repetitive text (many overlapping matches)")
    demo_case(text3, pattern3, repeats=3)

    # 4) Small example showing spurious hash possibility demonstration
    # We'll craft small mod to increase collision probability (simulate)
    # This is to illustrate why Rabin-Karp must verify.
    text4 = "XYZABC"
    pattern4 = "ABC"
    print("Case 4: Small example (no collision expected in standard mod)")
    demo_case(text4, pattern4, repeats=3)

    # Optional: demonstrate forced spurious hash (educational)
    # We'll run a custom Rabin-Karp with tiny mod to show a collision could happen,
    # but note production RK should use large mod and verification step already included.
    print("Case 4b: Demonstrate spurious hash (using tiny modulus to force collisions).")
    def rabin_karp_tiny_mod(text, pattern):
        return rabin_karp_search(text, pattern, base=256, mod=101)  # tiny mod
    t_rk_tiny, res_rk_tiny = time_function(rabin_karp_tiny_mod, text3, pattern3, repeats=1)
    print(f"Rabin-Karp with tiny mod: found {len(res_rk_tiny)} matches (verification still performed).")
    print("Note: RK always verifies matches, so even with collisions it will not report false positives.\n")

    print("Demo complete.")

if __name__ == "__main__":
    main()
