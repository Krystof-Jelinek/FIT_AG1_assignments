#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Point : size_t {};

struct Path {
  Point from, to;
  unsigned length;

  Path(size_t f, size_t t, unsigned l) : from{f}, to{t}, length{l} {}

  friend bool operator == (const Path& a, const Path& b) {
    return std::tie(a.from, a.to, a.length) == std::tie(b.from, b.to, b.length);
  }
  
  friend bool operator != (const Path& a, const Path& b) { return !(a == b); }
};

#endif

const size_t ROOT = static_cast<size_t>(-1);

class Edge{
  public:
  size_t to;
  size_t lenght;

  Edge(size_t t, size_t l) : to(t), lenght(l) {}

};

class Node{
  public:
  size_t index;
  size_t predecessor;
  size_t current_longest_path;
  size_t added_by_predecessor;
  std::vector<Edge> neighbours;

  Node(size_t i) : index(i), predecessor(ROOT), current_longest_path(0), added_by_predecessor(0) {}

};

class Graf{
  public:
  std::vector<Node> all_nodes;
  std::unordered_set<size_t> begin_nodes;
  size_t longest_path;
  size_t longest_path_node_index;

  Graf(size_t number_of_nodes) : longest_path(0), longest_path_node_index(0){
    all_nodes.reserve(number_of_nodes);
    begin_nodes.reserve(number_of_nodes);

    for(size_t i = 0; i < number_of_nodes;i++){
      all_nodes.emplace_back(Node(i));
      begin_nodes.insert(i);
    }
  }

  void add_edge(const Path & input){
    all_nodes[input.from].neighbours.emplace_back(Edge(input.to,input.length));
    
    //here i wanna remove the input.to node from the possible beginigns of the bfs alrgorith
    begin_nodes.erase(input.to);
  }

};

std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths){
    //creating graf and adding all the paths -- preparing for the bfs
    Graf graf(points);
    for(auto itr = all_paths.begin(); itr != all_paths.end(); itr++){
      graf.add_edge((*itr));
    }

    //executing bfs algorithm on the graf
    std::queue<Node> queue;

    for(auto itr = graf.begin_nodes.begin(); itr!= graf.begin_nodes.end();itr++){
        Node start =  graf.all_nodes[*itr];

        queue.push(start);
    }

        while(!queue.empty()){
          Node cur_node = queue.front();
          queue.pop();

          for(auto itr = cur_node.neighbours.begin(); itr != cur_node.neighbours.end(); itr++){
              Node & neigbour = graf.all_nodes[(*itr).to];
              size_t possible_longest_path = cur_node.current_longest_path + (*itr).lenght;

              if(neigbour.current_longest_path < possible_longest_path){
                neigbour.current_longest_path = possible_longest_path;
                neigbour.added_by_predecessor = (*itr).lenght;
                //update the longest path in the whole graf
                if(graf.longest_path < neigbour.current_longest_path){
                  graf.longest_path = neigbour.current_longest_path;
                  graf.longest_path_node_index = neigbour.index;
                }
                neigbour.predecessor = cur_node.index;
                queue.push(neigbour);
              }
          }          
        }
    //creating the path from the results of the bfs
    std::vector<Path> result;
    
    Node cur_node = graf.all_nodes[graf.longest_path_node_index];
    
    while(cur_node.predecessor != ROOT){
      result.emplace_back(Path(cur_node.predecessor,cur_node.index,cur_node.added_by_predecessor));
      cur_node = graf.all_nodes[cur_node.predecessor];
    }
    std::reverse(result.begin(),result.end());
    return result;
}


#ifndef __PROGTEST__


struct Test {
  unsigned longest_track;
  size_t points;
  std::vector<Path> all_paths;
};

inline const Test TESTS[] = {
  {13, 5, { {3,2,10}, {3,0,9}, {0,2,3}, {2,4,1} } },
  {11, 5, { {3,2,10}, {3,1,4}, {1,2,3}, {2,4,1} } },
  {16, 8, { {3,2,10}, {3,1,1}, {1,2,3}, {1,4,15} } },
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Fail: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool run_test(const Test& t) {
  auto sol = longest_track(t.points, t.all_paths);

  unsigned length = 0;
  for (auto [ _, __, l ] : sol) length += l;

  CHECK(t.longest_track == length,
    "Wrong length: got %u but expected %u", length, t.longest_track);

  for (size_t i = 0; i < sol.size(); i++) {
    CHECK(std::count(t.all_paths.begin(), t.all_paths.end(), sol[i]),
      "Solution contains non-existent path: %zu -> %zu (%u)",
      sol[i].from, sol[i].to, sol[i].length);

    if (i > 0) CHECK(sol[i].from == sol[i-1].to,
      "Paths are not consecutive: %zu != %zu", sol[i-1].to, sol[i].from);
  }

  return true;
}
#undef CHECK

int main() {

  int ok = 0, fail = 0;

  for (auto&& t : TESTS) (run_test(t) ? ok : fail)++;
  
  if (!fail) printf("Passed all %i tests!\n", ok);
  else printf("Failed %u of %u tests.\n", fail, fail + ok);
}

#endif


