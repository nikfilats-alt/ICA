#include "../include/bigint.h"
#include <algorithm>
#include <stdexcept>

// ═══════════════════════════════════════════════════
//  BigInt — реализация
// ═══════════════════════════════════════════════════

BigInt::BigInt() : d(1, 0), neg(false) {}

BigInt::BigInt(long long v) {
    neg = v < 0;
    long long u = neg ? -(unsigned long long)v : v;
    if (u == 0) { d.push_back(0); return; }
    while (u) { d.push_back(u % 10); u /= 10; }
}

bool BigInt::parse(const std::string& s) {
    d.clear(); neg = false;
    int start = 0;
    if (s.empty()) return false;
    if (s[0] == '-') { neg = true; start = 1; }
    else if (s[0] == '+') { start = 1; }
    if (start >= (int)s.size()) return false;
    for (int i = (int)s.size() - 1; i >= start; --i) {
        if (!isdigit((unsigned char)s[i])) return false;
        d.push_back(s[i] - '0');
    }
    trim();
    return true;
}

void BigInt::trim() {
    while (d.size() > 1 && d.back() == 0) d.pop_back();
    if (d.size() == 1 && d[0] == 0) neg = false;
}

bool BigInt::isZero() const {
    return d.size() == 1 && d[0] == 0;
}

std::string BigInt::str() const {
    std::string r;
    if (neg) r += '-';
    for (int i = (int)d.size() - 1; i >= 0; --i)
        r += ('0' + d[i]);
    return r;
}

// Вывод с разбивкой на группы по 3: 1 000 000
std::string BigInt::strGrouped() const {
    std::string raw;
    for (int i = (int)d.size() - 1; i >= 0; --i)
        raw += ('0' + d[i]);
    std::string r;
    if (neg) r += '-';
    int start = (int)raw.size() % 3;
    for (int i = 0; i < (int)raw.size(); ++i) {
        if (i > 0 && i == start) r += ' ';
        else if (i > start && (i - start) % 3 == 0) r += ' ';
        r += raw[i];
    }
    return r;
}

int BigInt::cmpAbs(const BigInt& o) const {
    if (d.size() != o.d.size()) return d.size() < o.d.size() ? -1 : 1;
    for (int i = (int)d.size() - 1; i >= 0; --i)
        if (d[i] != o.d[i]) return d[i] < o.d[i] ? -1 : 1;
    return 0;
}

bool BigInt::operator==(const BigInt& o) const { return neg == o.neg && cmpAbs(o) == 0; }
bool BigInt::operator!=(const BigInt& o) const { return !(*this == o); }
bool BigInt::operator<(const BigInt& o) const {
    if (neg != o.neg) return neg;
    int c = cmpAbs(o);
    return neg ? c > 0 : c < 0;
}
bool BigInt::operator<=(const BigInt& o) const { return !(o < *this); }
bool BigInt::operator>(const BigInt& o) const  { return o < *this; }
bool BigInt::operator>=(const BigInt& o) const { return !(*this < o); }

// ── Сложение абсолютных значений ──
BigInt addAbs(const BigInt& a, const BigInt& b) {
    BigInt r; r.d.clear();
    int carry = 0;
    for (int i = 0; i < (int)std::max(a.d.size(), b.d.size()) || carry; ++i) {
        int s = carry;
        if (i < (int)a.d.size()) s += a.d[i];
        if (i < (int)b.d.size()) s += b.d[i];
        r.d.push_back(s % 10);
        carry = s / 10;
    }
    return r;
}

// ── Вычитание абсолютных значений (|a| >= |b|) ──
BigInt subAbs(const BigInt& a, const BigInt& b) {
    BigInt r; r.d.clear();
    int borrow = 0;
    for (int i = 0; i < (int)a.d.size(); ++i) {
        int s = a.d[i] - borrow - (i < (int)b.d.size() ? b.d[i] : 0);
        if (s < 0) { s += 10; borrow = 1; } else borrow = 0;
        r.d.push_back(s);
    }
    r.trim();
    return r;
}

BigInt operator+(const BigInt& a, const BigInt& b) {
    if (a.neg == b.neg) {
        BigInt r = addAbs(a, b); r.neg = a.neg; r.trim(); return r;
    }
    int c = a.cmpAbs(b);
    if (c == 0) return BigInt(0);
    if (c > 0) { BigInt r = subAbs(a, b); r.neg = a.neg; r.trim(); return r; }
    else       { BigInt r = subAbs(b, a); r.neg = b.neg; r.trim(); return r; }
}

BigInt operator-(const BigInt& a, const BigInt& b) {
    BigInt nb = b; nb.neg = !nb.neg;
    if (nb.isZero()) nb.neg = false;
    return a + nb;
}

BigInt operator*(const BigInt& a, const BigInt& b) {
    BigInt r;
    r.d.assign(a.d.size() + b.d.size(), 0);
    for (int i = 0; i < (int)a.d.size(); ++i)
        for (int j = 0; j < (int)b.d.size(); ++j) {
            r.d[i+j] += a.d[i] * b.d[j];
            if (r.d[i+j] >= 10) {
                r.d[i+j+1] += r.d[i+j] / 10;
                r.d[i+j] %= 10;
            }
        }
    r.neg = a.neg ^ b.neg;
    r.trim();
    return r;
}

std::pair<BigInt,BigInt> divmod(const BigInt& a, const BigInt& b) {
    if (b.isZero()) throw std::runtime_error("Деление на ноль!");
    BigInt bAbs = b; bAbs.neg = false;
    BigInt q, rem;
    q.d.resize(a.d.size(), 0);
    rem.d.clear(); rem.d.push_back(0);
    for (int i = (int)a.d.size()-1; i >= 0; --i) {
        rem.d.insert(rem.d.begin(), a.d[i]);
        rem.trim();
        int lo = 0, hi = 9;
        while (lo < hi) {
            int mid = (lo+hi+1)/2;
            if ((bAbs * BigInt(mid)).cmpAbs(rem) <= 0) lo = mid; else hi = mid-1;
        }
        q.d[i] = lo;
        rem = rem - bAbs * BigInt(lo);
        if (rem.neg) rem = BigInt(0);
    }
    q.neg = a.neg ^ b.neg;
    q.trim();
    rem.neg = a.neg;
    rem.trim();
    return {q, rem};
}

BigInt operator/(const BigInt& a, const BigInt& b) { return divmod(a,b).first; }
BigInt operator%(const BigInt& a, const BigInt& b) { return divmod(a,b).second; }

BigInt abs_val(const BigInt& a) {
    BigInt r = a; r.neg = false; return r;
}
