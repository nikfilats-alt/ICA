#include "include/parser.h"
#include <iostream>
#include <string>

// ═══════════════════════════════════════════════════════
//  ICA — Infinite Calculative Adventure
//  main.cpp — точка входа
// ═══════════════════════════════════════════════════════

int main() {
    std::cout << "\n";
    std::cout << " ██╗ ██████╗ █████╗ \n";
    std::cout << " ██║██╔════╝██╔══██╗\n";
    std::cout << " ██║██║     ███████║\n";
    std::cout << " ██║██║     ██╔══██║\n";
    std::cout << " ██║╚██████╗██║  ██║\n";
    std::cout << " ╚═╝ ╚═════╝╚═╝  ╚═╝\n";
    std::cout << "\n";
    std::cout << " Infinite Calculative Adventure  v0.2\n";
    std::cout << " Числа до 1024 цифр. Никаких ограничений.\n";
    std::cout << " Введите 'help' для справки.\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        std::cout.flush();
        if (!std::getline(std::cin, line)) break;
        try {
            process(line);
        } catch (const std::exception& e) {
            std::cout << "[Ошибка] " << e.what() << "\n";
        }
    }
    return 0;
}
