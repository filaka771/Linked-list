#ifndef LINKED_LIST
#define LINKED_LIST

#include <type_traits>
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
    
    Node* node_pool_;
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

    // ----------------------------------------------------

    std::size_t get_index(std::size_t dummy_index, size_t position){
        std::size_t index = node_pool_[dummy_index].next;

        for(std::size_t current = 0; current < position; current ++){
            index = node_pool_[index].next;
        }

        return index;
    }

    // Data deinitialization
    void destroy_element(std::size_t position) {
        std::size_t index = get_index(LIVE_DUMMY_INDEX, position);

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
        if(index >= capacity_){
            std::cerr << "Error: index " << index << " >= capacity " << capacity_ << std::endl;
            throw std::out_of_range("Index is out of allocated buffer range!");
        }

        node_pool_[index].prev = prev;
        node_pool_[index].next = next;
    }

    template<typename... Args>
    void construct_element(std::size_t position, Args&&... args) {
        std::size_t index = get_index(LIVE_DUMMY_INDEX, position);
        new (&node_pool_[index].data) ElType(std::forward<Args>(args)...);
    }

    void move_node(std::size_t from_dummy_idx, std::size_t from_pos,
                    std::size_t to_dummy_idx,std::size_t to_pos){
        if(!is_dummy_idx(from_dummy_idx))
            throw std::invalid_argument("Wrong dummy idx for \"from\" list!");

        if(!is_dummy_idx(to_dummy_idx))
            throw std::invalid_argument("Wrong dummy idx for \"to\" list!");

        std::size_t from_idx = get_index(from_dummy_idx, from_pos);
        std::size_t to_idx = get_index(to_dummy_idx, to_pos);

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
                std::cout << " <-> " << index;
                index = node_pool_[index].next;
                count++;
            }
            std::cout << " <-> dummy (end of cycle)\n";
    }

public:
    explicit Linked_List(std::size_t initial_capacity)
        : live_count_(0),
        capacity_(initial_capacity + 2){

        node_pool_ = new Node[capacity_];

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);

        if (capacity_ > 0) {
            set_neighbors(FREE_DUMMY_INDEX, capacity_ - 1, 2);

            for (std::size_t i = 2; i < capacity_ - 1; ++i) {
                set_neighbors(i, i - 1, i + 1);
            }

            set_neighbors(capacity_ - 1, capacity_ - 2, FREE_DUMMY_INDEX);
        } else {
            set_neighbors(FREE_DUMMY_INDEX, FREE_DUMMY_INDEX, FREE_DUMMY_INDEX);
        }
    }

    ~Linked_List() {
        std::size_t current = node_pool_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);
            current = node_pool_[current].next;
        }

        delete[] node_pool_;
    }

    //------------------Getters------------------

    bool empty() const {return live_count_ == 0;}
    std::size_t size() const {return live_count_;}
    std::size_t capacity() const{return (capacity_ - 2);}

    ElType get_value(std::size_t position){
        std::size_t idx = get_index(LIVE_DUMMY_INDEX, position);
        return node_pool_[idx].data;
    }


    void clear() {
        std::size_t current = node_pool_[LIVE_DUMMY_INDEX].next;
        while(current != LIVE_DUMMY_INDEX) {
            destroy_element(current);

            std::size_t next_free = node_pool_[FREE_DUMMY_INDEX].next;

            set_neighbors(current, FREE_DUMMY_INDEX, next_free);
            set_neighbors(next_free, current, node_pool_[next_free].next);
            set_neighbors(FREE_DUMMY_INDEX, node_pool_[FREE_DUMMY_INDEX].prev, current);

            current = node_pool_[current].next;
        }

        set_neighbors(LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX, LIVE_DUMMY_INDEX);
        live_count_ = 0;
    }

    //------------------Main_el_op------------------
    template<typename... Args>
    void push_back(Args&&... args){
        if(size() < capacity()){
            try{move_node(FREE_DUMMY_INDEX, 0, LIVE_DUMMY_INDEX, live_count_);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument error: " << e.what() << std::endl;
                abort();
            }

            construct_element(live_count_, std::forward<Args>(args)...);

            live_count_ ++;
        }
        else{
            // Reallocation instead of overflow
            throw std::runtime_error("Linked list is overflowed!\n");
        }
    }

    template<typename... Args>
    void push_forward(Args&&... args){
        if(size() < capacity()){
            try{move_node(FREE_DUMMY_INDEX, 0, LIVE_DUMMY_INDEX, 0);
            } catch(const std::invalid_argument& e){
                    std::cerr << "Invalid argument error: " << e.what() << std::endl;
                    abort();
            }

            construct_element(0, std::forward<Args>(args)...);

            live_count_ ++;
        }
        else{
            // Reallocation instead of overflow
            throw std::runtime_error("Linked list is overflowed!");
        }
    }

    void pop_back(){
        if(live_count_ != 0){
            destroy_element(live_count_);

            try{move_node(LIVE_DUMMY_INDEX, live_count_ - 1, FREE_DUMMY_INDEX, 0);
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
            destroy_element(0);
            try{move_node(LIVE_DUMMY_INDEX, 0, FREE_DUMMY_INDEX, 0);
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
        if(size() <= size()){
            try{move_node(FREE_DUMMY_INDEX, 0, LIVE_DUMMY_INDEX, position);
            } catch(const std::invalid_argument& e){
                std::cerr << "Invalid argument error: " << e.what() << std::endl;
                abort();
            }
            construct_element(position, std::forward<Args>(args)...);
        }
        else{
            throw std::runtime_error("");
        }

        live_count_ ++;
    }

    void erase(std::size_t position){
        destroy_element(position);
        try{move_node(LIVE_DUMMY_INDEX, position, FREE_DUMMY_INDEX, 0);
        } catch(const std::invalid_argument& e){
            std::cerr << "Invalid argument error: " << e.what() << std::endl;
            abort();
        }

        live_count_ --;
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
        if (index >= capacity_ + 2) {
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
    for(size_t current = 0; current < capacity_; current ++){
        debug_node(current);
        }
    }
};

#endif
