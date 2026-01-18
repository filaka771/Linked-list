#ifndef LINKED_LIST
#define LINKED_LIST

#include <type_traits>
#include <initializer_list>
#include <stdexcept>
#include <cstddef>
#include <iostream>
#include <cassert>

template<typename ElType>
class Linked_List {
private:
    struct Node {
        ElType data;
        std::size_t prev;
        std::size_t next;
        Node() : prev(0), next(0) {}
    };

    static const std::size_t LIVE_DUMMY_INDEX = 0;
    static const std::size_t FREE_DUMMY_INDEX = 1;
    static const std::size_t INVALID_INDEX = static_cast<std::size_t>(-1);
    static const std::size_t ELEMENTS_PER_LINE = 15;
    
    Node* node_pool;
    std::size_t live_count;
    std::size_t capacity;

   void destroy_all_elements() {
        std::size_t current = node_pool[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            if (!std::is_trivially_destructible<ElType>::value) {
                node_pool[current].data.~ElType();
            }
            current = node_pool[current].next;
        }
    }

    void set_neighbors(std::size_t index, std::size_t prev, std::size_t next) {
        node_pool[index].prev = prev;
        node_pool[index].next = next;
    }

    void set_node_value(std::size_t index, const ElType& value) {
        node_pool[index].data = value;
    }

    template<typename... Args>
    void construct_element(std::size_t index, Args&&... args) {
        new (&node_pool[index].data) ElType(std::forward<Args>(args)...);
    }

    void destroy_element(std::size_t index) {
        if (!std::is_trivially_destructible<ElType>::value) {
            node_pool[index].data.~ElType();
            }
    }

    void visualize_list(std::size_t dummy_index) {
        std::size_t index = node_pool[dummy_index].next;
        std::size_t count = 0;
        
        std::cout << "dummy";
        while (index != dummy_index) {
            if (count % ELEMENTS_PER_LINE == 0 && count != 0) {
                std::cout << "\n";
            }
            std::cout << " <-> " << index;
            index = node_pool[index].next;
            count++;
        }
        std::cout << " <-> dummy (end of cycle)";
    }

public:
    explicit Linked_List(std::size_t initial_capacity = 16)
        : capacity(initial_capacity > 2 ? initial_capacity : 16),
          live_count(0) {
        
        node_pool = new Node[capacity + 2];
        
        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
        
        if (capacity > 0) {
            set_neighbors(FREE_DUMMY_INDEX, capacity + 1, 2);
            
            for (std::size_t i = 2; i <= capacity; ++i) {
                set_neighbors(i, i - 1, i + 1);
            }
            
            set_neighbors(capacity + 1, capacity, FREE_DUMMY_INDEX);
        } else {
            set_neighbors(FREE_DUMMY_INDEX, FREE_DUMMY_INDEX, FREE_DUMMY_INDEX);
        }
    }

    ~Linked_List() {
        std::size_t current = node_pool[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            current = node_pool[current].next;
        }
        
        delete[] node_pool;
    }

    void clear() {
        std::size_t current = node_pool[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            
            std::size_t next_free = node_pool[FREE_DUMMY_INDEX].next;
            
            set_neighbors(current, FREE_DUMMY_INDEX, next_free);
            set_neighbors(next_free, current, node_pool[next_free].next);
            set_neighbors(FREE_DUMMY_INDEX, node_pool[FREE_DUMMY_INDEX].prev, current);
            
            current = node_pool[current].next;
        }
        
        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
        live_count = 0;
    }
    
    void visualize() {
        std::cout << "Live elements list:\n";
        visualize_list(LIVE_DUMMY_INDEX);
        std::cout << "\n\nFree elements list:\n";
        visualize_list(FREE_DUMMY_INDEX);
        std::cout << "\n";
    }
    
    void debug_node(std::size_t index) {
        if (index >= capacity + 2) {
            std::cout << "Index " << index << " out of bounds\n";
            return;
        }
        std::cout << "Node[" << index << "]: ";
        std::cout << "prev=" << node_pool[index].prev;
        std::cout << ", next=" << node_pool[index].next;
        std::cout << ", data=" << node_pool[index].data;
        std::cout << "\n";
    }
};

#endif
