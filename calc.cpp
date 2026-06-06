/*
  ╔══════════════════════════════════════════════════════╗
  ║         БЕЗЛИМИТНЫЙ КАЛЬКУЛЯТОР  (BigNum)           ║
  ║  Напишите "help" для справки. "exit" для выхода.    ║
  ╚══════════════════════════════════════════════════════╝

  Формат ввода: <число> <оператор> <число>
  Например:  12345678901234567890 * 99999999999999999999
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <sstream>

// ─────────────────────────────────────────────
//  BigInt — целые числа произвольной длины
// ─────────────────────────────────────────────
struct BigInt {
    std::vector<int> d; // цифры, d[0] = младшая
    bool neg = false;

    BigInt() : d(1, 0) {}

    BigInt(long long v) {
        neg = v < 0;
        long long u = neg ? -v : v;
        if (u == 0) d.push_back(0);
        while (u) { d.push_back(u % 10); u /= 10; }
    }

    // Парсинг строки. Возвращает false при ошибке.
    bool parse(const std::string& s) {
        d.clear(); neg = false;
        int start = 0;
        if (s.empty()) return false;
        if (s[0] == '-') { neg = true; start = 1; }
        else if (s[0] == '+') { start = 1; }
        if (start >= (int)s.size()) return false;
        for (int i = (int)s.size() - 1; i >= start; --i) {
            if (!isdigit(s[i])) return false;
            d.push_back(s[i] - '0');
        }
        trim();
        return true;
    }

    void trim() {
        while (d.size() > 1 && d.back() == 0) d.pop_back();
        if (d.size() == 1 && d[0] == 0) neg = false;
    }

    bool isZero() const { return d.size() == 1 && d[0] == 0; }

    std::string str() const {
        std::string r;
        if (neg) r += '-';
        for (int i = (int)d.size() - 1; i >= 0; --i)
            r += ('0' + d[i]);
        return r;
    }

    // Сравнение абсолютных значений
    int cmpAbs(const BigInt& o) const {
        if (d.size() != o.d.size()) return d.size() < o.d.size() ? -1 : 1;
        for (int i = (int)d.size() - 1; i >= 0; --i)
            if (d[i] != o.d[i]) return d[i] < o.d[i] ? -1 : 1;
        return 0;
    }

    bool operator==(const BigInt& o) const { return neg == o.neg && cmpAbs(o) == 0; }
    bool operator< (const BigInt& o) const {
        if (neg != o.neg) return neg;
        int c = cmpAbs(o);
        return neg ? c > 0 : c < 0;
    }
    bool operator<=(const BigInt& o) const { return !(o < *this); }
    bool operator> (const BigInt& o) const { return o < *this; }
    bool operator>=(const BigInt& o) const { return !(*this < o); }
};

// ── Сложение абсолютных значений ──
BigInt addAbs(const BigInt& a, const BigInt& b) {
    BigInt r;
    r.d.clear();
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
    BigInt r;
    r.d.clear();
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

// ── Деление и остаток ──
std::pair<BigInt,BigInt> divmod(const BigInt& a, const BigInt& b) {
    if (b.isZero()) throw std::runtime_error("Деление на ноль!");
    BigInt q, rem;
    q.d.resize(a.d.size(), 0);
    rem.d.clear(); rem.d.push_back(0);
    for (int i = (int)a.d.size()-1; i >= 0; --i) {
        // rem = rem * 10 + a.d[i]
        rem.d.insert(rem.d.begin(), a.d[i]);
        rem.trim();
        // найти цифру: сколько раз |b| влезает в rem
        int lo = 0, hi = 9;
        while (lo < hi) {
            int mid = (lo+hi+1)/2;
            BigInt t = b; t.neg = false;
            BigInt m(mid);
            if ((t*m).cmpAbs(rem) <= 0) lo = mid; else hi = mid-1;
        }
        q.d[i] = lo;
        BigInt t = b; t.neg = false;
        rem = rem - t * BigInt(lo);
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

// ── Степень ──
BigInt power(BigInt base, BigInt exp) {
    if (exp.neg) throw std::runtime_error("Отрицательная степень не поддерживается для целых чисел");
    BigInt result(1);
    while (!exp.isZero()) {
        if (exp.d[0] & 1) result = result * base;
        base = base * base;
        exp = exp / BigInt(2);
    }
    return result;
}

// ── Факториал ──
BigInt factorial(BigInt n) {
    if (n.neg) throw std::runtime_error("Факториал отрицательного числа не определён");
    if (n > BigInt(100000)) throw std::runtime_error("Слишком большое число для факториала (макс. 100000)");
    BigInt r(1), i(1);
    while (i <= n) { r = r * i; i = i + BigInt(1); }
    return r;
}

// ── Мультифакториал n!!...! (k восклицательных знаков) ──
// n!k = n * (n-k) * (n-2k) * ...
BigInt multifactorial(BigInt n, int k) {
    if (n.neg) throw std::runtime_error("Мультифакториал отрицательного числа не определён");
    if (n > BigInt(100000)) throw std::runtime_error("Слишком большое число для мультифакториала (макс. 100000)");
    BigInt r(1);
    BigInt step(k);
    while (!n.isZero() && n > BigInt(0)) {
        r = r * n;
        n = n - step;
    }
    return r;
}

// ── НОД ──
BigInt gcd(BigInt a, BigInt b) {
    a.neg = false; b.neg = false;
    while (!b.isZero()) { BigInt t = a % b; a = b; b = t; }
    return a;
}

// ── НОК ──
BigInt lcm(const BigInt& a, const BigInt& b) {
    if (a.isZero() || b.isZero()) return BigInt(0);
    BigInt ab = a * b; ab.neg = false;
    return ab / gcd(a, b);
}

// ── Квадратный корень (целая часть) ──
BigInt isqrt(const BigInt& n) {
    if (n.neg) throw std::runtime_error("Корень из отрицательного числа не определён");
    if (n.isZero()) return BigInt(0);
    // Начальное приближение: 10^(digits/2)
    BigInt x(1);
    int bits = (int)n.d.size();
    for (int i = 0; i < (bits+1)/2; ++i) x = x * BigInt(10);
    // Метод Ньютона
    while (true) {
        BigInt x1 = (x + n / x) / BigInt(2);
        if (x1 >= x) break;
        x = x1;
    }
    return x;
}

// ─────────────────────────────────────────────
//  РАЗБОР ВВОДА
// ─────────────────────────────────────────────
const int MAX_NUM_LEN = 1024;

void showHelp() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║              БЕЗЛИМИТНЫЙ КАЛЬКУЛЯТОР — СПРАВКА               ║
╠═══════════════════════════════════════════════════════════════╣
║  Числа: целые, до 1024 цифр каждое. Знак минус допустим.     ║
║  Вводите выражение и нажмите Enter.                           ║
╠═══════════════════════════════════════════════════════════════╣
║  ОПЕРАТОРЫ:                                                   ║
║   A + B       сложение                                        ║
║   A - B       вычитание                                       ║
║   A * B       умножение                                       ║
║   A / B       целочисленное деление                           ║
║   A % B       остаток от деления                              ║
║   A ^ B       возведение в степень (B >= 0)                   ║
║   A ** B      то же самое (альтернативный синтаксис)          ║
╠═══════════════════════════════════════════════════════════════╣
║  ФУНКЦИИ (один аргумент):                                     ║
║   A !         факториал (A!)                                  ║
║   A !!        двойной факториал (A!!)                        ║
║   A !!!       тройной мультифакториал                         ║
║   A !4        мультифакториал с шагом 4 (и т.д. до !9)       ║
║   sqrt A      целая часть квадратного корня                   ║
╠═══════════════════════════════════════════════════════════════╣
║  ФУНКЦИИ (два аргумента):                                     ║
║   gcd A B     наибольший общий делитель                       ║
║   lcm A B     наименьшее общее кратное                        ║
╠═══════════════════════════════════════════════════════════════╣
║  КОМАНДЫ:                                                     ║
║   help        эта справка                                     ║
║   exit / quit выход                                           ║
╠═══════════════════════════════════════════════════════════════╣
║  ПРИМЕРЫ:                                                     ║
║   99999999999999999999 * 99999999999999999999                 ║
║   100 !                                                       ║
║   10 !!                                                       ║
║   15 !3                                                       ║
║   gcd 123456789 987654321                                     ║
║   2 ^ 1000                                                    ║
║   sqrt 144                                                    ║
╚═══════════════════════════════════════════════════════════════╝
)";
}

// Убрать пробелы по краям
std::string trim(const std::string& s) {
    int l = 0, r = (int)s.size()-1;
    while (l <= r && isspace((unsigned char)s[l])) ++l;
    while (r >= l && isspace((unsigned char)s[r])) --r;
    return s.substr(l, r-l+1);
}

// Разбить строку на токены
std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> tokens;
    std::string cur;
    for (char c : s) {
        if (isspace((unsigned char)c)) {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

BigInt parseNum(const std::string& s) {
    if (s.size() > MAX_NUM_LEN + 1) // +1 для знака
        throw std::runtime_error("Число превышает 1024 символа: " + s.substr(0, 20) + "...");
    BigInt n;
    if (!n.parse(s))
        throw std::runtime_error("Неверный формат числа: " + s);
    return n;
}

void process(const std::string& line) {
    std::string s = trim(line);
    if (s.empty()) return;
    if (s == "help") { showHelp(); return; }
    if (s == "exit" || s == "quit") { std::cout << "Выход.\n"; exit(0); }

    auto tokens = tokenize(s);
    if (tokens.empty()) return;

    // ── Функции с одним аргументом-числом ──
    // sqrt <N>
    if (tokens[0] == "sqrt" || tokens[0] == "SQRT") {
        if (tokens.size() < 2) throw std::runtime_error("Нужен аргумент: sqrt <N>");
        BigInt n = parseNum(tokens[1]);
        std::cout << "= " << isqrt(n).str() << "\n";
        return;
    }

    // gcd <A> <B>
    if (tokens[0] == "gcd" || tokens[0] == "GCD") {
        if (tokens.size() < 3) throw std::runtime_error("Нужно: gcd <A> <B>");
        BigInt a = parseNum(tokens[1]), b = parseNum(tokens[2]);
        std::cout << "= " << gcd(a, b).str() << "\n";
        return;
    }

    // lcm <A> <B>
    if (tokens[0] == "lcm" || tokens[0] == "LCM") {
        if (tokens.size() < 3) throw std::runtime_error("Нужно: lcm <A> <B>");
        BigInt a = parseNum(tokens[1]), b = parseNum(tokens[2]);
        std::cout << "= " << lcm(a, b).str() << "\n";
        return;
    }

    // ── Формат: <число> <оператор> [<число>] ──
    if (tokens.size() < 2) throw std::runtime_error("Непонятная команда. Введите 'help'.");

    BigInt A = parseNum(tokens[0]);
    std::string op = tokens[1];

    // Мультифакториал: "!" / "!!" / "!!!" / "!2"..."!9"
    if (!op.empty() && op[0] == '!') {
        int excl = 0;
        bool customStep = false;
        int step = 1;
        // Считаем "!"
        for (char c : op) {
            if (c == '!') ++excl;
            else if (isdigit(c)) { step = c - '0'; customStep = true; }
        }
        if (customStep) {
            // !N — мультифакториал с шагом N
            std::cout << "= " << multifactorial(A, step).str() << "\n";
        } else if (excl == 1) {
            std::cout << "= " << factorial(A).str() << "\n";
        } else {
            std::cout << "= " << multifactorial(A, excl).str() << "\n";
        }
        return;
    }

    if (tokens.size() < 3) throw std::runtime_error("Нужен второй операнд. Введите 'help'.");
    BigInt B = parseNum(tokens[2]);

    BigInt result;
    if (op == "+") result = A + B;
    else if (op == "-") result = A - B;
    else if (op == "*") result = A * B;
    else if (op == "/") result = A / B;
    else if (op == "%") result = A % B;
    else if (op == "^" || op == "**") result = power(A, B);
    else throw std::runtime_error("Неизвестный оператор: " + op + ". Введите 'help'.");

    std::cout << "= " << result.str() << "\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║    БЕЗЛИМИТНЫЙ КАЛЬКУЛЯТОР  BigNum      ║\n";
    std::cout << "║  Введите 'help' для справки.            ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        try {
            process(line);
        } catch (const std::exception& e) {
            std::cout << "[Ошибка] " << e.what() << "\n";
        }
    }
    return 0;
}
