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
    
    Node* node_pull_;
    std::size_t live_count_;
    std::size_t capacity_;

    // index - notation for index in array
    // position - notation for index in linked list

    // ----------------------------------------------------
    bool is_dummy_idx(std::size_t idx){
        if(idx == LIVE_DUMMY_INDEX || idx == FREE_DUMMY_INDEX)
            return true;
        return false;
    }

    bool is_in_range(std::size_t dummy_idx, std::size_t position){
        if(dummy_idx == LIVE_DUMMY_INDEX && position <= live_count_)
            return true;

        if(dummy_idx == FREE_DUMMY_INDEX && position <= capacity_ - live_count_ - 2)
            return true;

        return false;
    }

    // ----------------------------------------------------

    std::size_t get_index(std::size_t dummy_index, size_t position){
        std::size_t index = node_pull_[dummy_index].next;

        if(dummy_index == LIVE_DUMMY_INDEX && position > live_count_)
            throw std::invalid_argument("Index out of the live linked list!");

        if(dummy_index == FREE_DUMMY_INDEX && position > (capacity_ - 2 - live_count_))
            throw std::invalid_argument("Index out of the free linked list!");

        for(std::size_t current = 0; current < position; current ++){
            index = node_pull_[index].next;
        }

        return index;
    }

    // Data deinitialization
    void destroy_element(std::size_t index) {
        if (!std::is_trivially_destructible<ElType>::value) {
            node_pull_[index].data.~ElType();
            }
    }

    void destroy_all_elements() {
        std::size_t current = node_pull_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            current = node_pull_[current].next;
        }
    }



    // Node operations
    void set_neighbors(std::size_t index, std::size_t prev, std::size_t next) {
        node_pull_[index].prev = prev;
        node_pull_[index].next = next;
    }

    template<typename... Args>
    void construct_element(std::size_t position, Args&&... args) {
        std::size_t index = get_index(LIVE_DUMMY_INDEX, position);
        new (&node_pull_[index].data) ElType(std::forward<Args>(args)...);
    }




    void move_node(std::size_t from_dummy_idx, std::size_t from_pos,
                    std::size_t to_dummy_idx,std::size_t to_pos){
        if(!is_dummy_idx(from_dummy_idx))
            throw std::invalid_argument("Wrong dummy idx for \"from\" list!");

        if(!is_dummy_idx(to_dummy_idx))
            throw std::invalid_argument("Wrong dummy idx for \"to\" list!");

        if(!is_in_range(from_dummy_idx, from_pos))
            throw std::invalid_argument("\"From\" position is out of range!");

        if(!is_in_range(to_dummy_idx, to_pos))
            throw std::invalid_argument("\"To\" position is out of range!");

        std::size_t from_idx = get_index(from_dummy_idx, from_pos);
        std::size_t to_idx   = get_index(to_dummy_idx, to_pos);


        size_t prev_idx = node_pull_[from_idx].prev;
        size_t next_idx = node_pull_[from_idx].next;
        // Delete node from live list
        set_neighbors(prev_idx, node_pull_[prev_idx].prev, next_idx);
        set_neighbors(next_idx, prev_idx ,node_pull_[prev_idx].next);

        // Add node to free list
        prev_idx = node_pull_[to_idx].prev;
        next_idx = to_idx;

        set_neighbors(from_idx, prev_idx, next_idx);
        set_neighbors(prev_idx, node_pull_[prev_idx].prev, from_idx);
        set_neighbors(next_idx, from_idx, node_pull_[next_idx].next);

    }

    // Visualization
    void visualize_list(std::size_t dummy_index) {
        std::size_t index = node_pull_[dummy_index].next;
        std::size_t count = 0;

        std::cout << "dummy";
        while (index != dummy_index) {
            if (count % ELEMENTS_PER_LINE == 0 && count != 0) {
                std::cout << "\n";
            }
            std::cout << " <-> " << index;
            index = node_pull_[index].next;
            count++;
            }
            std::cout << " <-> dummy (end of cycle)";
    }

public:
    explicit Linked_List(std::size_t initial_capacity = 16)
        : capacity_(initial_capacity > 2 ? initial_capacity : 16),
            live_count_(0) {

        node_pull_ = new Node[capacity_ + 2];

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);

        if (capacity_ > 0) {
            set_neighbors(FREE_DUMMY_INDEX, capacity_ + 1, 2);

            for (std::size_t i = 2; i <= capacity_; ++i) {
                set_neighbors(i, i - 1, i + 1);
            }

            set_neighbors(capacity_ + 1, capacity_, FREE_DUMMY_INDEX);
        } else {
            set_neighbors(FREE_DUMMY_INDEX, FREE_DUMMY_INDEX, FREE_DUMMY_INDEX);
        }
    }

    ~Linked_List() {
        std::size_t current = node_pull_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            current = node_pull_[current].next;
        }

        delete[] node_pull_;
    }

    void clear() {
        std::size_t current = node_pull_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);

            std::size_t next_free = node_pull_[FREE_DUMMY_INDEX].next;

            set_neighbors(current, FREE_DUMMY_INDEX, next_free);
            set_neighbors(next_free, current, node_pull_[next_free].next);
            set_neighbors(FREE_DUMMY_INDEX, node_pull_[FREE_DUMMY_INDEX].prev, current);

            current = node_pull_[current].next;
        }

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
        live_count_ = 0;
    }

    //------------------Main_el_op------------------
    template<typename... Args>
    void push_back(Args&&... args){
        move_node(FREE_DUMMY_INDEX, 1, LIVE_DUMMY_INDEX, live_count_);

        construct_element(live_count_, std::forward<Args>(args)...);

        live_count_++;
    }
    
    void visualize() {
        std::cout << "Live elements list:\n";
        visualize_list(LIVE_DUMMY_INDEX);
        std::cout << "\n\nFree elements list:\n";
        visualize_list(FREE_DUMMY_INDEX);
        std::cout << "\n";
    }
    
    void debug_node(std::size_t index) {
        if (index >= capacity_ + 2) {
            std::cout << "Index " << index << " out of bounds\n";
            return;
        }
        std::cout << "Node[" << index << "]: ";
        std::cout << "prev=" << node_pull_[index].prev;
        std::cout << ", next=" << node_pull_[index].next;
        std::cout << ", data=" << node_pull_[index].data;
        std::cout << "\n";
    }
};

#endif
