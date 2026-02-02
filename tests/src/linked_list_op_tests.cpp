#include <iostream>
#include <random>
#include <list>
#include "linked_list.h"
#include <stdexcept>

/*
  void test_pop_push_back(){
  Linked_List<int> my_list(8);
  std::cout << "\nAfter init!\n";
  my_list.visualize();
  //-------------- Push/pop back operations --------------
                   // Push back test
                   std::cout << "\nPush/pop back test:\n";

    while(list.size() < list.capacity()){
    try{
    my_list.push_back(static_cast<int>(list.size()));
    } catch(const std::runtime_error& e){
    std::cerr << "List overflowed while push back test!\n";
    abort();
    }
    //std::cout << std::endl << "Size: " << list.size() << " Capacity: " << list.capacity() << std::endl;
      //list.debug_visualize();
      }

    // Overflow handling test
       try{
       my_list.push_back(static_cast<int>(list.size()));
       } catch(const std::runtime_error& e){
       std::cout << "\n* List overflow successfully handled!\n";
       }

    // Pop back test
       while(my_list.size() != 0){
       try{
       my_list.pop_back();
       } catch(std::runtime_error& e){
       std::cerr << "\nList underflowed while pop back test!\n";
       abort();
       }
       //std::cout << std::endl << "Size: " << list.size() << " Capacity: " << list.capacity() << std::endl;
         //list.debug_visualize();
         }


    // Underflow handling test
       try{
       my_list.pop_back();
       } catch(const std::runtime_error& e){
       std::cout << "\n* List underflow successfully handled!\n";
       }
       }
 */

void test_pop_push_forward(){

    Linked_List<int> list(8);
    std::cout << "\nAfter init!\n";
    list.visualize();

    //-------------- Push/pop forward operations --------------

    // Push forward test
    std::cout << "\nPush/pop forward test:\n";
    while(list.size() < list.capacity()){
        try{
            list.push_forward(static_cast<int>(list.size()));
        } catch(std::runtime_error& e){
            std::cerr << "\nList overflowed while push forward test!\n";
            abort();
        }

        std::cout << std::endl << "Size: " << list.size() << " Capacity: " << list.capacity() << std::endl;
        list.debug_visualize();
    }

    // Overflow test
    try{
        list.push_back(static_cast<int>(list.size()));
    } catch(const std::runtime_error& e){
        std::cout << "\n* List overflow successfully handled!\n";
    }

    list.visualize();

    // Pop forward test
    while(list.size() != 0){
        try{
            list.pop_back();
        } catch(std::runtime_error& e){
            std::cerr << "\nList underflowed while pop forward test!\n";
            abort();
        }

        //std::cout << std::endl << "Size: " << list.size() << " Capacity: " << list.capacity() << std::endl;
        //list.debug_visualize();
    }

    list.visualize();

    // Underflow test
    try{
        list.pop_back();
    } catch(const std::runtime_error& e){
        std::cout << "\n* List underflow successfully handled!\n";
    }
}

/*
  void test_insert_erase() {
  std::cout << "=== Insert/Erase Test ===\n";
    
    // Create list with 1000 elements 0-999
       Linked_List<int> list(2000);
       for (int i = 0; i < 1000; ++i) {
       list.insert(static_cast<int>(list.size()), i);
       }
    
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> pos_dist(1, 998);
    std::uniform_int_distribution<int> val_dist(1000, 9999);
    
    // Test 100 insert operations
       for (int test = 0; test < 100; ++test) {
       std::size_t old_size = list.size();
       int pos = pos_dist(rng);
       int new_val = val_dist(rng);
        
        // Remember neighbors before insert
           int left_neighbor = list.get_value(pos - 1);
           int right_neighbor = list.get_value(pos);
        
        // Insert new value
           list.insert(pos, new_val);
        
        // Verify size increased
           if (list.size() != old_size + 1) {
           std::cerr << "FAIL: Insert test " << test 
           << " - Size mismatch. Expected: " << old_size + 1 
           << ", Got: " << list.size() << "\n";
           std::abort();
           }
        
        // Verify neighbors are correct
           if (list.get_value(pos - 1) != left_neighbor) {
           std::cerr << "FAIL: Insert test " << test 
           << " - Left neighbor changed. Expected: " << left_neighbor 
           << ", Got: " << list.get_value(pos - 1) << "\n";
           std::abort();
           }
        
        if (list.get_value(pos) != new_val) {
        std::cerr << "FAIL: Insert test " << test 
        << " - New value incorrect. Expected: " << new_val 
        << ", Got: " << list.get_value(pos) << "\n";
        std::abort();
        }
        
        if (list.get_value(pos + 1) != right_neighbor) {
        std::cerr << "FAIL: Insert test " << test 
        << " - Right neighbor incorrect. Expected: " << right_neighbor 
        << ", Got: " << list.get_value(pos + 1) << "\n";
        std::abort();
        }
        }
    
    // Test 100 erase operations
       for (int test = 0; test < 100; ++test) {
       std::size_t old_size = list.size();
       int pos = pos_dist(rng);
        
        // Remember neighbors before erase
           int left_neighbor = list.get_value(pos - 1);
           int right_neighbor = list.get_value(pos + 1);
        
        // Erase value
           list.erase(pos);
        
        // Verify size decreased
           if (list.size() != old_size - 1) {
           std::cerr << "FAIL: Erase test " << test 
           << " - Size mismatch. Expected: " << old_size - 1 
           << ", Got: " << list.size() << "\n";
           std::abort();
           }
        
        // Verify neighbors are connected correctly
           if (list.get_value(pos - 1) != left_neighbor) {
           std::cerr << "FAIL: Erase test " << test 
           << " - Left neighbor changed. Expected: " << left_neighbor 
           << ", Got: " << list.get_value(pos - 1) << "\n";
           std::abort();
           }
        
        if (list.get_value(pos) != right_neighbor) {
        std::cerr << "FAIL: Erase test " << test 
        << " - Nodes not connected. Expected: " << right_neighbor 
        << ", Got: " << list.get_value(pos) << "\n";
        std::abort();
        }
        }
    
    std::cout << "All insert/erase tests passed\n";
    }
 */
void test_insert_erase() {
    Linked_List<int> list(18);
    for (int i = 0; i < 14; ++i) {
        list.push_back(59);
    }

    list.debug_visualize();
    std::cout << std::endl;
    list.insert(11, 69);

    list.debug_visualize();

}

void test_defrag() {
    std::cout << "\nDefragmentation test:\n";
    // Create list with 1000 elements 0-999
    Linked_List<int> list(30);
    for (int i = 0; i < 20; ++i) {
        list.insert(static_cast<int>(list.size()), i);
    }
    
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> pos_dist(0, 19);

    // Prepare list for degragmentation test
    for (int i = 0; i < 5; ++i) {
        int pos = pos_dist(rng);
        list.erase(pos);
    }

    std::cout << "\nList before defragmentation: \n";
    list.visualize();

    list.defrag();

    std::cout << "\nList after defragmentation: \n";

    list.visualize();

}

void insert_erase(){

    std::cout << "List init: " << std::endl;
    Linked_List<int> list(30);
    for (int i = 0; i < 10; ++i) {
        list.insert(static_cast<int>(list.size()), i);
    }
    list.debug_visualize();

    std::cout << "\nInsert 0: \n";
    list.insert(0, 20);
    list.debug_visualize();
    list.visualize();


    std::cout << "\nInsert 5: \n";
    list.insert(5, 21);
    list.debug_visualize();
    list.visualize();

    std::cout << "\nInsert 12: \n";
    list.insert(12, 22);
    list.debug_visualize();
    list.visualize();

    std::cout << "\nErase 0: \n";
    list.erase(0);
    list.debug_visualize();
    list.visualize();

    std::cout << "\nErase 5: \n";
    list.erase(5);
    list.debug_visualize();
    list.visualize();

    std::cout << "\nErase 10: \n";
    list.erase(10);
    list.debug_visualize();
    list.visualize();
}

void push_test(){
    Linked_List<int> list(30);
    /*
      for (int i = 0; i < 10; ++i) {
      list.insert(static_cast<int>(list.size()), i);
      }
     */
    list.debug_visualize();
    std::cout << std::endl;

    list.push_back(12);
    list.debug_visualize();

    list.push_forward(22);
    list.debug_visualize();
}

int main() {
    //push_test();
    //test_pop_push_back();
    test_pop_push_forward();
    //insert_erase();
    //test_insert_erase();
    //test_defrag();


    return 0;
}
