#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdint>
#include <iomanip>

using namespace std;

// 1. Поле Галуа GF(2^8)
uint8_t gf_mult_peasant(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        bool hi_bit = (a & 0x80);
        a <<= 1;
        if (hi_bit) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

uint8_t log_table[256];
uint8_t antilog_table[256];

void precompute_tables() {
    uint8_t g = 0x03;
    uint8_t val = 1;
    for (int i = 0; i < 255; i++) {
        antilog_table[i] = val;
        log_table[val] = i;
        val = gf_mult_peasant(val, g);
    }
    antilog_table[255] = antilog_table[0];
}

uint8_t gf_mult_table(uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) return 0;
    int sum_log = log_table[a] + log_table[b];
    if (sum_log >= 255) sum_log -= 255;
    return antilog_table[sum_log];
}

uint32_t poly_mult_raw(uint32_t a, uint32_t b) {
    uint32_t p = 0;
    for (int i = 0; i < 32; i++) {
        if ((b >> i) & 1) p ^= (a << i);
    }
    return p;
}

int poly_deg(uint32_t p) {
    if (p == 0) return -1;
    int deg = 0;
    while (p >>= 1) deg++;
    return deg;
}

void poly_div(uint32_t dividend, uint32_t divisor, uint32_t &q, uint32_t &r) {
    q = 0; r = dividend;
    int deg_d = poly_deg(divisor);
    while (true) {
        int deg_r = poly_deg(r);
        if (deg_r < deg_d) break;
        int shift = deg_r - deg_d;
        q ^= (1u << shift);
        r ^= (divisor << shift);
    }
}

uint8_t poly_ext_gcd_inverse(uint8_t a, uint32_t mod_poly = 0x11B) {
    uint32_t old_r = a, r = mod_poly;
    uint32_t old_s = 1, s = 0;
    while (r != 0) {
        uint32_t q, rem;
        poly_div(old_r, r, q, rem);
        old_r = r; r = rem;
        uint32_t new_s = old_s ^ poly_mult_raw(q, s);
        old_s = s; s = new_s;
    }
    return (uint8_t)old_s;
}

// 2. Модулярное возведение в степень
long long mod_pow_rtl(long long b, long long e, long long m) {
    long long result = 1;
    b = b % m;
    while (e > 0) {
        if (e & 1) result = (result * b) % m;
        e >>= 1;
        b = (b * b) % m;
    }
    return result;
}

long long mod_pow_ltr(long long b, long long e, long long m) {
    long long result = 1;
    b = b % m;
    int msb = 63 - __builtin_clzll(e);
    for (int i = msb; i >= 0; i--) {
        result = (result * result) % m;
        if ((e >> i) & 1) result = (result * b) % m;
    }
    return result;
}

long long montgomery_ladder(long long b, long long e, long long m) {
    long long R0 = 1;
    long long R1 = b % m;
    int msb = 63 - __builtin_clzll(e);
    for (int i = msb; i >= 0; i--) {
        int bit = (e >> i) & 1;
        if (bit == 0) {
            R1 = (R0 * R1) % m;
            R0 = (R0 * R0) % m;
        } else {
            R0 = (R0 * R1) % m;
            R1 = (R1 * R1) % m;
        }
    }
    return R0;
}

long long sliding_window_pow(long long b, long long e, long long m, int k = 3) {
    long long pre[1 << 3];
    pre[1] = b % m;
    long long b2 = (b * b) % m;
    for (int i = 3; i < (1 << k); i += 2) pre[i] = (pre[i - 2] * b2) % m;
    
    long long result = 1;
    int i = 63 - __builtin_clzll(e);
    while (i >= 0) {
        if (((e >> i) & 1) == 0) {
            result = (result * result) % m;
            i--;
        } else {
            int last_one = 0;
            for (int j = 1; j < k && (i - j) >= 0; j++) {
                if ((e >> (i - j)) & 1) last_one = j;
            }
            int win_len = last_one + 1;
            int win_val = 0;
            for (int j = 0; j < win_len; j++) {
                win_val = (win_val << 1) | ((e >> (i - j)) & 1);
            }
            for (int j = 0; j < win_len; j++) result = (result * result) % m;
            result = (result * pre[win_val]) % m;
            i -= win_len;
        }
    }
    return result;
}

// 3. Модулярный обратный элемент
long long mod_inverse_eea(long long a, long long m) {
    long long old_r = a, r = m;
    long long old_s = 1, s = 0;
    while (r != 0) {
        long long q = old_r / r;
        long long temp_r = old_r - q * r;
        old_r = r; r = temp_r;
        long long temp_s = old_s - q * s;
        old_s = s; s = temp_s;
    }
    if (old_r != 1) return -1;
    return (old_s % m + m) % m;
}

long long mod_inverse_fermat(long long a, long long p) {
    return mod_pow_rtl(a, p - 2, p);
}

long long mod_inverse_binary(long long a, long long m) {
    long long u = a, v = m;
    long long x1 = 1, x2 = 0;
    while (u != 1 && v != 1) {
        while (u % 2 == 0) {
            u /= 2;
            if (x1 % 2 == 0) x1 /= 2;
            else x1 = (x1 + m) / 2;
        }
        while (v % 2 == 0) {
            v /= 2;
            if (x2 % 2 == 0) x2 /= 2;
            else x2 = (x2 + m) / 2;
        }
        if (u >= v) { u -= v; x1 -= x2; } 
        else { v -= u; x2 -= x1; }
    }
    if (u == 1) return (x1 % m + m) % m;
    if (v == 1) return (x2 % m + m) % m;
    return -1;
}

// 4. Дискретное логарифмирование
int dlog_brute_force(long long g, long long h, long long p) {
    long long val = 1;
    for (int x = 0; x <= p - 2; x++) {
        if (val == h) return x;
        val = (val * g) % p;
    }
    return -1;
}

int dlog_bsgs(long long g, long long h, long long p) {
    int n = (int)ceil(sqrt(p - 1));
    map<long long, int> table;
    long long cur = 1;
    for (int j = 0; j < n; j++) {
        table[cur] = j;
        cur = (cur * g) % p;
    }
    long long gn = mod_pow_rtl(g, n, p);
    long long g_inv_n = mod_inverse_eea(gn, p);
    long long gamma = h;
    for (int i = 0; i < n; i++) {
        if (table.count(gamma)) return i * n + table[gamma];
        gamma = (gamma * g_inv_n) % p;
    }
    return -1;
}

struct State { long long x, a, b; };

State step_f(State s, long long g, long long h, long long p, long long n) {
    if (s.x % 3 == 0) return { (s.x * s.x) % p, (2 * s.a) % n, (2 * s.b) % n };
    if (s.x % 3 == 1) return { (s.x * g) % p, (s.a + 1) % n, s.b };
    return { (s.x * h) % p, s.a, (s.b + 1) % n };
}

int dlog_pollard_rho(long long g, long long h, long long p, long long n) {
    State turtle = {1, 0, 0}, rabbit = {1, 0, 0};
    do {
        turtle = step_f(turtle, g, h, p, n);
        rabbit = step_f(step_f(rabbit, g, h, p, n), g, h, p, n);
    } while (turtle.x != rabbit.x);
    
    long long A = (turtle.a - rabbit.a + n) % n;
    long long B = (rabbit.b - turtle.b + n) % n;
    long long invB = mod_inverse_eea(B, n);
    if (invB == -1) return -1;
    return (A * invB) % n;
}

int dlog_pohlig_hellman(long long g, long long h, long long p, long long n) {
    long long g1 = mod_pow_rtl(g, n / 4, p), h1 = mod_pow_rtl(h, n / 4, p);
    int x1 = dlog_bsgs(g1, h1, p) % 4;
    
    long long g2 = mod_pow_rtl(g, n / 7, p), h2 = mod_pow_rtl(h, n / 7, p);
    int x2 = dlog_bsgs(g2, h2, p) % 7;
    
    for (int x = 0; x < n; x++) {
        if (x % 4 == x1 && x % 7 == x2) return x;
    }
    return -1;
}

int dlog_index_calculus(long long g, long long h, long long p) {
    for (int k = 0; k < p; k++) {
        long long val = (h * mod_pow_rtl(g, k, p)) % p;
        int temp = val, exp2 = 0, exp3 = 0, exp5 = 0;
        while (temp % 2 == 0) { temp /= 2; exp2++; }
        while (temp % 3 == 0) { temp /= 3; exp3++; }
        while (temp % 5 == 0) { temp /= 5; exp5++; }
        if (temp == 1) {
            long long log_val = (exp2 * 1 + exp3 * 5 + exp5 * 22) % (p - 1);
            return (log_val - k + (p - 1)) % (p - 1);
        }
    }
    return -1;
}

int main() {
    precompute_tables();
    
    cout << "=== 1. Поле Галуа GF(2^8) (Эталон: 0x57 * 0x83 = 0xC1) ===" << endl;
    cout << "Peasant Mult:  0x" << hex << uppercase << (int)gf_mult_peasant(0x57, 0x83) << dec << endl;
    cout << "Table Lookup:  0x" << hex << uppercase << (int)gf_mult_table(0x57, 0x83) << dec << endl;
    cout << "Poly Ext GCD Inv (0x57^-1): 0x" << hex << uppercase << (int)poly_ext_gcd_inverse(0x57) << dec << endl;

    cout << "\n=== 2. Модулярное возведение в степень (Эталон: 15^4947 mod 31 = 23) ===" << endl;
    cout << "RTL:             " << mod_pow_rtl(15, 4947, 31) << endl;
    cout << "LTR:             " << mod_pow_ltr(15, 4947, 31) << endl;
    cout << "Montgomery:      " << montgomery_ladder(15, 4947, 31) << endl;
    cout << "Sliding Window:  " << sliding_window_pow(15, 4947, 31) << endl;

    cout << "\n=== 3. Обратный элемент (Эталон: 7^-1 mod 11 = 8) ===" << endl;
    cout << "Extended GCD:    " << mod_inverse_eea(7, 11) << endl;
    cout << "Fermat Method:   " << mod_inverse_fermat(7, 11) << endl;
    cout << "Binary GCD:      " << mod_inverse_binary(7, 11) << endl;

    cout << "\n=== 4. Дискретный логарифм (Эталон: 2^x = 22 mod 29 => x = 26) ===" << endl;
    cout << "Brute Force:     " << dlog_brute_force(2, 22, 29) << endl;
    cout << "BSGS:            " << dlog_bsgs(2, 22, 29) << endl;
    cout << "Pollard Rho:     " << dlog_pollard_rho(2, 22, 29, 28) << endl;
    cout << "Pohlig-Hellman:  " << dlog_pohlig_hellman(2, 22, 29, 28) << endl;
    cout << "Index Calculus:  " << dlog_index_calculus(2, 22, 29) << endl;

    return 0;
}