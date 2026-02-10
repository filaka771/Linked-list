#include <iostream>
#include <random>
#include <list>
#include <climits>
#include <cstdint>
#include "linked_list.h"
#include <stdexcept>
/*
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
*/

/*
  void resize_test(){
  std::cout << "List after init:\n";
  Linked_List<int> list(8);
  list.debug_visualize();

  for (int i = 0; i < 5; ++i) {
  list.push_back(static_cast<int>(list.size()));
  }

  std::cout << "\nBefore resize: \n";
  list.debug_visualize();

  std::cout << "\nAfter resize: \n";
  list.resize(15);

  list.debug_visualize();

  list.clear();

  std::cout << "\nLive count after clear: " << list.size() << std::endl;
  std::cout << "\nList after clear: \n";
  list.debug_visualize();

  for(int i = 0; i < list.capacity(); i ++){
  list.push_back(i);
  }
  std::cout << "\n List fully used: \n";
  list.debug_visualize();

  list.resize(list.capacity() * 2);

  std::cout << "\nFull list after resize: \n";
  list.debug_visualize();
  }
*/

// Pass by reference!
void list_comp(Linked_List<int>& my_list, std::list<int>& std_list) {
    // Or Option 2: Keep const if you made get_value() const
    // void list_comp(const Linked_List<int>& my_list, const std::list<int>& std_list) {
    if(my_list.size() != std_list.size()) {
        std::cout << "\nIn my list: " << my_list.size() 
                  << " In std list: " << std_list.size() << std::endl;
        throw std::runtime_error("Wrong num of elements!\n");
    }

    auto std_it = std_list.begin();
    for(std::size_t current = 0; current < my_list.size(); current++) {
        if(my_list.get_value(current) != *std_it) {
            std::cout << "\nOn position " << current << " my list contains: " 
                      << my_list.get_value(current) << " std list contains: " 
                      << *std_it << std::endl;
            throw std::runtime_error("Wrong value!\n");
        }
        ++std_it;
    }
}

void list_op(Linked_List<int>& my_list, std::list<int>& std_list, int op, int value, int stat_counts[]) {
    stat_counts[op] ++;
    switch(op) {
        // Push back
    case 0:
        my_list.push_back(value);
        std_list.push_back(value);
        break;

        // Push forward
    case 1:
        my_list.push_forward(value);
        std_list.push_front(value);
        break;

        // Pop back
    case 2:
        if(!my_list.empty() && !std_list.empty()) {
            my_list.pop_back();
            std_list.pop_back();
        }

        break;

        // Pop forward
    case 3:
        if(!my_list.empty() && !std_list.empty()) {
            my_list.pop_forward();
            std_list.pop_front();
        }
        break;


        // Insert 
    case 4: {
        if(my_list.empty()) {
            my_list.push_back(value);
            std_list.push_back(value);

        } else {
            std::size_t position = value % my_list.size();
            my_list.insert(position, value);
                
            auto it = std_list.begin();
            std::advance(it, position);
            std_list.insert(it, value);
        }
        break;
    }

        // Erase
    case 5:
        if(!my_list.empty() && !std_list.empty()) {
            std::size_t position = value % my_list.size();
            my_list.erase(position);
                
            auto it = std_list.begin();
            std::advance(it, position);
            std_list.erase(it);
        }
        break;

        // Defragmentate
    case 6: 
        my_list.defrag();
        break;

            
    default:
        break;
    }
}

void rand_op_test(uint op_num, uint op_count) {
    const char* op_list[] = {"Push back", "Push front", "Pop back", "Pop front", "Insert",
                             "Erase", "Defrag"};

    int stat_counts[op_num] = {}; 

    // Start with empty lists to match behavior
    Linked_List<int> my_list(10);
    std::list<int> std_list;
    
    // Optionally, add some initial elements
    for(int i = 0; i < 100; i++) {
        my_list.push_back(i);
        std_list.push_back(i);
    }

    // Generate random operations
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> op_dist(0, op_num - 1);  // 0-5 operations
    std::uniform_int_distribution<int> val_dist(INT_MIN, INT_MAX - 1);
    
    for(uint count = 0; count < op_count; count++) {
        //std::cout << count << std::endl;
        int op = op_dist(rng);
        int value = val_dist(rng);
        
        list_op(my_list, std_list, op, value, stat_counts);
        list_comp(my_list, std_list);
        
        // Optional: print progress
        if(count % 1000 == 0) {
            std::cout << "Completed " << count << " operations\n";
        }
    }
    
    std::cout << "All tests passed! Processed " << op_count << " operations.\n";

    for(int op = 0; op < op_num; op ++){
        std::cout << op_list[op] << ": " << stat_counts[op] << std::endl;
    }
}

/*
  void defrag_test(){
  Linked_List<int> my_list(8);
  for(int i = 0; i < 8; i ++){
  my_list.push_back(i);
  }

  std::cout << "Before erase:" << std::endl;
  my_list.debug_visualize();

  my_list.erase(4);
  my_list.erase(3);
  my_list.erase(2);

  std::cout << "After erase:" << std::endl;
  my_list.debug_visualize();

  my_list.defrag();

  std::cout << "After defrag:" << std::endl;
  my_list.debug_visualize();
  }
*/

/*
  void test_swap(){
  Linked_List<int> my_list(8);
  for(int i = 0; i < 8; i ++){
  my_list.push_back(i);
  }

  std::cout << "Before swap:" << std::endl;
  my_list.debug_visualize();

  my_list.swap_nodes(3, 4);
  std::cout << "After swap:" << std::endl;
  my_list.debug_visualize();
  }
*/

int main() {
    rand_op_test(7, 100000);
    return 0;
}
