#ifndef LINKED_LIST
#define LINKED_LIST

#include <csetjmp>
#include <type_traits>
#include <stdexcept>
#include <cstddef>
#include <iostream>
#include <cassert>
#include <vector>

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
    static const std::size_t ELEMENTS_PER_LINE = 15;
    
    std::size_t live_count_;
    std::vector<Node> node_pool_;

    // index - notation for index in array
    // position - notation for index in linked list

    // ----------------------------------------------------
    bool is_dummy_idx(std::size_t idx){
        if(idx == LIVE_DUMMY_INDEX || idx == FREE_DUMMY_INDEX)
            return true;
        return false;
    }

    // ----------------------------------------------------

    std::size_t traverse(std::size_t start_index, std::size_t steps, bool forward) {
        std::size_t index = forward ? node_pool_[start_index].next 
                            : node_pool_[start_index].prev;
        for (std::size_t i = 0; i < steps; ++i) {
            index = forward ? node_pool_[index].next 
                    : node_pool_[index].prev;
        }
        return index;
    }

    std::size_t get_index(std::size_t dummy_index, std::size_t position) {
        const bool is_live_list = (dummy_index == LIVE_DUMMY_INDEX);
        const std::size_t list_size = is_live_list ? live_count_ 
                                      : node_pool_.size() - live_count_ - 2;
        position = position % (list_size + 1);

        // Choose optimal traversal direction
        const bool forward_traversal = position < (list_size / 2);
        if(position == list_size)
            return dummy_index;
    
        const std::size_t steps = forward_traversal ? position 
                                  : list_size - position - 1;

        return traverse(dummy_index, steps, forward_traversal);
    }

    // Data deinitialization
    void destroy_element(std::size_t index) {
        if (!std::is_trivially_destructible<ElType>::value){
            node_pool_[index].data.~ElType();
        }
    }

    void destroy_all_elements() {
        std::size_t current = node_pool_[LIVE_DUMMY_INDEX].next;

        while(current != LIVE_DUMMY_INDEX){
            destroy_element(current);
            current = node_pool_[current].next;
        }
    }

    // Node operations
    void set_neighbors(std::size_t index, std::size_t prev, std::size_t next) {
        if(index >= node_pool_.size()){
            std::cerr << "Error: index " << index << " >= capacity " << node_pool_.size() << std::endl;
            throw std::out_of_range("Index is out of allocated buffer range!");
        }

        node_pool_[index].prev = prev;
        node_pool_[index].next = next;
    }

    template<typename... Args>
    void construct_element(std::size_t index, Args&&... args) {
        new (&node_pool_[index].data) ElType(std::forward<Args>(args)...);
    }

    // Can move node from any list to any list from any postion
    // on any postion if both of them exists
    void move_node(std::size_t from_idx, std::size_t to_idx){

        // Delete element from from_list
        std::size_t prev = node_pool_[from_idx].prev;
        std::size_t next = node_pool_[from_idx].next;

        node_pool_[prev].next = next;
        node_pool_[next].prev = prev;

        // Add element to the new to_list
        prev = node_pool_[to_idx].prev;
        next = node_pool_[to_idx].next;

        node_pool_[from_idx].prev = prev;
        node_pool_[from_idx].next = to_idx;

        node_pool_[prev].next = from_idx;
        node_pool_[to_idx].prev = from_idx;
    }

    // Swap two any nodes from any lists in the meaning of
    // the array indexes. Doesn't chage anything in both
    // live list and free list.

    // Visualization
    void visualize_list(std::size_t dummy_index) {
        std::size_t index = node_pool_[dummy_index].next;
        std::size_t count = 0;

        std::cout << "dummy";
        int deb_count = 0;
        while (index != dummy_index && deb_count < 300) {
            deb_count ++;
            if (count % ELEMENTS_PER_LINE == 0 && count != 0) {
                std::cout << "\n";
            }
            std::cout << " <-> " << get_value(index);
            index = node_pool_[index].next;
            count++;
        }
        std::cout << " <-> dummy (end of cycle)\n";
    }

    // ----------------------------------------------------
public:
    explicit Linked_List(std::size_t initial_capacity)
    : live_count_(0),
    node_pool_(initial_capacity + 2)
    {
        if (initial_capacity == 0)
            throw std::invalid_argument("Initial list size must be bigger then 0!");

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
        
        if (node_pool_.size() > 0) {
            set_neighbors(FREE_DUMMY_INDEX, node_pool_.size() - 1, 2);
            
            for (std::size_t i = 2; i < node_pool_.size() - 1; ++i) {
                set_neighbors(i, i - 1, i + 1);
            }
            
            set_neighbors(node_pool_.size() - 1, node_pool_.size() - 2, FREE_DUMMY_INDEX);
        } else {
            set_neighbors(FREE_DUMMY_INDEX, FREE_DUMMY_INDEX, FREE_DUMMY_INDEX);
        }
    }

    ~Linked_List() {
        // Destroy live elements
        std::size_t current = node_pool_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            current = node_pool_[current].next;
        }
    }

    void resize(std::size_t new_size){
        if(new_size <= node_pool_.size())
            throw std::invalid_argument("New size for resize operation must be bigger then privious!\n");

        std::size_t prev_buf_size = node_pool_.size();
        std::size_t free_tail = node_pool_[FREE_DUMMY_INDEX].prev;

        node_pool_.resize(new_size + 2); // Required nodes + two dummies 
        std::size_t new_buf_size = node_pool_.size();

        // Connect new allocated nodes to the free list
        node_pool_[FREE_DUMMY_INDEX].prev = new_buf_size - 1;
        node_pool_[free_tail].next = prev_buf_size;

        set_neighbors(prev_buf_size, free_tail, prev_buf_size + 1);

        for(size_t node = prev_buf_size + 1; node < new_buf_size - 1; node ++){
            set_neighbors(node, node - 1, node + 1);
        }

        set_neighbors(new_buf_size - 1, new_buf_size - 2, FREE_DUMMY_INDEX);
    }


    //------------------Getters------------------

    bool empty() const {return live_count_ == 0;}
    std::size_t size() const {return live_count_;}
    std::size_t capacity() const{return (node_pool_.size() - 2);}

    ElType get_value(std::size_t position){
        std::size_t idx = get_index(LIVE_DUMMY_INDEX, position);
        return node_pool_[idx].data;
    }


    void clear() {
        std::size_t current = node_pool_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);

            std::size_t next = node_pool_[current].next;

            std::size_t free_head = get_index(FREE_DUMMY_INDEX, 0);
            move_node(current, free_head);

            current = next;
            live_count_ -= 1;
        }

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
    }

    //------------------Main_el_op------------------
    template<typename... Args>
    void push_back(Args&&... args){
        if(size() >= capacity()){
            resize(capacity() * 2);
        }

        std::size_t from_idx = get_index(FREE_DUMMY_INDEX, 0);
        std::size_t to_idx   = get_index(LIVE_DUMMY_INDEX, live_count_);

        try{move_node(from_idx, to_idx);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument error: " << e.what() << std::endl;
            abort();
        }

        construct_element(from_idx, std::forward<Args>(args)...);

        live_count_ ++;
    }

    template<typename... Args>
    void push_forward(Args&&... args){
        if(size() >= capacity()){
            resize(capacity() * 2);
        }

        std::size_t from_idx = get_index(FREE_DUMMY_INDEX, 0);
        std::size_t to_idx   = get_index(LIVE_DUMMY_INDEX, 0);

        try{move_node(from_idx, to_idx);
        } catch(const std::invalid_argument& e){
            std::cerr << "Invalid argument error: " << e.what() << std::endl;
            abort();
        }

        construct_element(from_idx, std::forward<Args>(args)...);

            live_count_ ++;
    }

    void pop_back(){
        if(live_count_ != 0){
            std::size_t from_idx = get_index(LIVE_DUMMY_INDEX, live_count_ - 1);
            std::size_t to_idx   = get_index(FREE_DUMMY_INDEX, 0);

            destroy_element(from_idx);

            try{move_node(from_idx, to_idx);
            } catch(const std::invalid_argument& e){
                std::cerr << "Invalid argument error: " << e.what() << std::endl;
                abort();
            }
        }
        else{
            throw std::runtime_error("Linked list is underflowed!\n");
        }

        live_count_ --;
    }

    void pop_forward(){
        if(live_count_ != 0){
            std::size_t from_idx = get_index(LIVE_DUMMY_INDEX, 0);
            std::size_t to_idx   = get_index(FREE_DUMMY_INDEX, 0);
            
            destroy_element(from_idx);

            try{move_node(from_idx, to_idx);
            } catch(const std::invalid_argument& e){
                std::cerr << "Invalid argument error: " << e.what() << std::endl;
                abort();
            }
        }
        else{
            throw std::runtime_error("Linked list is underflowed!\n");
        }

        live_count_ --;
    }

    //------------------Insert_el_op------------------
    template<typename... Args>
    void insert(std::size_t position, Args&&... args){
        if(size() >= capacity()){
            resize(capacity() * 2);
        }
        std::size_t from_idx = get_index(FREE_DUMMY_INDEX, 0);
        std::size_t to_idx   = get_index(LIVE_DUMMY_INDEX, position);
        try{move_node(from_idx, to_idx);
        } catch(const std::invalid_argument& e){
            std::cerr << "Invalid argument error: " << e.what() << std::endl;
            abort();
        }
        construct_element(from_idx, std::forward<Args>(args)...);

        live_count_ ++;
    }

    void erase(std::size_t position){
        std::size_t from_idx = get_index(LIVE_DUMMY_INDEX, position);
        std::size_t to_idx   = get_index(FREE_DUMMY_INDEX, 0);

        destroy_element(from_idx);

        try{move_node(from_idx, to_idx);
        } catch(const std::invalid_argument& e){
            std::cerr << "Invalid argument error: " << e.what() << std::endl;
            abort();
        }

    live_count_ --;
    }

    //------------------Defrag------------------
    void swap_nodes(std::size_t idx_1, std::size_t idx_2) {
        if (idx_1 == idx_2) return;  // No need to swap same node
    
        // Store original neighbors
        std::size_t prev1 = node_pool_[idx_1].prev;
        std::size_t next1 = node_pool_[idx_1].next;
        std::size_t prev2 = node_pool_[idx_2].prev;
        std::size_t next2 = node_pool_[idx_2].next;
    
        // Handle special case: nodes are adjacent
        if (next1 == idx_2) {  // idx_1 -> idx_2
            node_pool_[idx_1].next = next2;
            node_pool_[idx_1].prev = idx_2;
            node_pool_[idx_2].prev = prev1;
            node_pool_[idx_2].next = idx_1;
        
            if (prev1 != idx_2) node_pool_[prev1].next = idx_2;
            if (next2 != idx_1) node_pool_[next2].prev = idx_1;
        } 
        else if (next2 == idx_1) {  // idx_2 -> idx_1
            node_pool_[idx_2].next = next1;
            node_pool_[idx_2].prev = idx_1;
            node_pool_[idx_1].prev = prev2;
            node_pool_[idx_1].next = idx_2;
        
            if (prev2 != idx_1) node_pool_[prev2].next = idx_1;
            if (next1 != idx_2) node_pool_[next1].prev = idx_2;
        }
        else {  // Non-adjacent nodes
            // Update neighbors of idx_1
            node_pool_[idx_1].prev = prev2;
            node_pool_[idx_1].next = next2;
        
            // Update neighbors of idx_2
            node_pool_[idx_2].prev = prev1;
            node_pool_[idx_2].next = next1;
        
            // Update external pointers
            if (prev1 != idx_2) node_pool_[prev1].next = idx_2;
            if (next1 != idx_2) node_pool_[next1].prev = idx_2;
            if (prev2 != idx_1) node_pool_[prev2].next = idx_1;
            if (next2 != idx_1) node_pool_[next2].prev = idx_1;
        }
    
        // Swap data (optional - depends on your requirements)
        ElType tmp = std::move(node_pool_[idx_1].data);
        node_pool_[idx_1].data = std::move(node_pool_[idx_2].data);
        node_pool_[idx_2].data = std::move(tmp);
    }

    void defrag(){
        std::size_t current_idx = std::max(LIVE_DUMMY_INDEX, FREE_DUMMY_INDEX);
        std::size_t next_idx = node_pool_[LIVE_DUMMY_INDEX].next;

        while (next_idx != LIVE_DUMMY_INDEX) {
            if (next_idx != current_idx + 1) {
                swap_nodes(current_idx + 1, next_idx);

            }
            current_idx ++;
            next_idx = node_pool_[current_idx].next;
        }
    }
    //------------------Debug------------------
    void visualize() {
        std::cout << "Live elements list:\n";
        visualize_list(LIVE_DUMMY_INDEX);
        std::cout << "\n\nFree elements list:\n";
        visualize_list(FREE_DUMMY_INDEX);
        std::cout << "\n";
    }
    
    void debug_node(std::size_t index) {
        if (index >= node_pool_.size() + 2) {
            std::cout << "Index " << index << " out of bounds\n";
            return;
        }
        std::cout << "Node[" << index << "]: ";
        std::cout << "prev=" << node_pool_[index].prev;
        std::cout << ", next=" << node_pool_[index].next;
        std::cout << ", data=" << node_pool_[index].data;
        std::cout << "\n";
    }

    void debug_visualize(){
        for(size_t current = 0; current < node_pool_.size(); current ++){
            debug_node(current);
        }
    }
};

#endif
