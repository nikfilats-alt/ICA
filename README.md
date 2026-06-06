# ICA — Infinite Calculative Adventure

> The fastest arbitrary-precision calculator for everyone. Right in CMD!

No limits. No rounding. No `inf`. No `overflow`. Just pure math.

---

## ✨ Features

- Numbers up to **1024 digits** each — no floating point, no rounding
- **50+ functions** across number theory, combinatorics, sequences, and more
- Variables: `x = 2^100` then `x * x`
- Last result reuse: `ans + 1`
- Clean Russian error messages (English coming soon)
- Works on **Windows** (CMD), **Android** (CxxDroid)

---

## 🚀 Download & Run

### Windows (no install needed)
1. Go to [**Releases**](https://github.com/nikfilats-alt/ICA/releases)
2. Download `ICA.exe`
3. Double-click — enjoy!

### Windows (build from source)
Requires [MSYS2](https://www.msys2.org/) with g++ installed.
```
Double-click build.bat
```
That's it. It compiles and launches automatically.

### Android (CxxDroid)
1. Install [CxxDroid](https://play.google.com/store/apps/details?id=ru.iiec.cxxdroid) from Google Play
2. Open `main.cpp` in CxxDroid
3. In settings, add extra source files: `src/bigint.cpp src/mathfuncs.cpp src/parser.cpp`
4. Add include path: `include`
5. Hit ▶ Run

---

## 📖 Quick Reference

```
> 99999999999999999999 * 99999999999999999999
= 9999999999999999999800000000000000000001

> 2 ^ 1000
= 107150860718626...  (302 digits)

> 100 !
= 933262154439441...  (158 digits)

> 10 !!
= 3840

> 15 !3
= 168168000

> fib 1000
= 434665576869374...  (209 digits)

> isprime 999999999999999877
= Да

> factor 720
= 720 = 2^4 * 3^2 * 5

> tobase 255 16
= FF

> roman 2024
= MMXXIV

> collatz 27
= 111

> x = 2 ^ 100
> x * x
= 2^200  (= ans automatically)
```

---

## 🧮 All Functions

| Category | Commands |
|---|---|
| Arithmetic | `+ - * / % ^` `sqrt` `cbrt` `nrt K N` |
| Modular | `modpow A E M` `modinv A M` `extgcd A B` |
| Factorials | `N!` `N!!` `N!!!` `N!K` `subfact` `primorial` `falling` `rising` |
| Number Theory | `gcd` `lcm` `isprime` `nextprime` `factor` `phi` `mu` `lambda` `sumdiv` `cntdiv` |
| Combinatorics | `C N K` `catalan` `stirling2` `bell` `partitions` `narayana` `eulerian` |
| Sequences | `fib` `lucas` `trib` `pell` `motzkin` |
| Digit Properties | `sumdigits` `digroot` `reverse` `ispalindrome` `isarmstrong` `ishappy` `iskaprekar` |
| Number Properties | `isperfect` `isabundant` `isdeficient` |
| Bases | `tobase` `frombase` `bin` `hex` `oct` `roman` |
| Figurate Numbers | `tri` `sqr` `pent` `hept` `tetra` `pyra` `ctri` `csqr` `chex` |
| Misc | `collatz` `syrmax` `ackermann` `digits` `grouped` `absval` |

Type `help <category>` for details: `help fact`, `help numth`, `help comb`, etc.

---

## 📁 Project Structure

```
ICA/
├── main.cpp          entry point
├── build.bat         Windows one-click build & run
├── include/
│   ├── bigint.h      big integer type
│   ├── mathfuncs.h   all math functions
│   └── parser.h      input parser & variables
└── src/
    ├── bigint.cpp    big integer arithmetic
    ├── mathfuncs.cpp 50+ math functions
    └── parser.cpp    command parser & help system
```

---

## 🛠 Status

🚧 **Alpha v0.2** — actively developed

Planned:
- [ ] Rational numbers (exact fractions: `1/3 + 1/6 = 1/2`)
- [ ] Expression parser with brackets: `(2+3) * (10-4)`
- [ ] English interface
- [ ] More sequences

---

## 👤 Author

**Fill Nik** — [@nikfilats-alt](https://github.com/nikfilats-alt)

> *"An infinite calculative adventure"*
