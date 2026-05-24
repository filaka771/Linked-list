  # Linked List With Defragmentation

  A C++ implementation of a linked list built on top of a contiguous node pool.

  The idea of the project is simple: keep linked-list semantics, but store nodes inside a
  reusable buffer and support explicit defragmentation. After `defrag()`, live nodes are
  packed closer together in memory, which improves cache locality during traversal.

  ## What it supports

  **Write operations**
  - `push_back`
  - `push_forward`
  - `insert`

  **Delete operations**
  - `pop_back`
  - `pop_forward`
  - `erase`

  **Structure / memory**
  - `resize`
  - `clear`
  - `defrag`

  **Access**
  - `get_value`
  - `size`
  - `capacity`
  - `empty`

  **Debug**
  - `visualize`
  - `debug_visualize`

  ## Build

  Basic build:

  ```bash
  cmake -S . -B build
  cmake --build build

  Build with presets:

  cmake --preset asan
  cmake --build --preset asan

  cmake --preset lsan
  cmake --build --preset lsan

  cmake --preset release
  cmake --build --preset release

  ## Testing

  The project includes a randomized test executable that compares this list against
  std::list.

  Run it with:

  ./build/linked_list_op_tests

  Or with a preset build:

  ./build/asan/linked_list_op_tests
  ./build/lsan/linked_list_op_tests
  ./build/release/linked_list_op_testss