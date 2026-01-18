#include <iostream>
#include "linked_list.h"

int main() {
    Linked_List<int> list(200);
    
    std::cout << "Initial state:\n";
    list.visualize();
    
    list.clear();
    std::cout << "\nAfter clear:\n";
    list.visualize();
    
    return 0;
}
