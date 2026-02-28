#include "gf2n_demo.h"
#include <iostream>

int main() {
    std::cout << "Введите номер пункта (1-5) или 0 чтобы вывести всё: ";
    int пункт = 0;
    if (!(std::cin >> пункт)) return 0;

    try {
        switch (пункт) {
            case 0: print_all_tasks(); break;
            case 1: print_task_1(); break;
            case 2: print_task_2(); break;
            case 3: print_task_3(); break;
            case 4: print_task_4(); break;
            case 5: print_task_5(); break;
            default:
                std::cout << "Ошибка: нужно ввести 0,1,2,3,4 или 5.\n";
                break;
        }
    } catch (const std::exception& e) {
        std::cout << "Исключение: " << e.what() << "\n";
    }

    return 0;
}