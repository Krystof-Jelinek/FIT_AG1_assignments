#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif

class StupidSolution{
private:
  std::string str;
public:
  StupidSolution(const std::string& text)
  : str(text)
  {}
  virtual ~StupidSolution() = default;
  virtual size_t size() const
  {
    return str.size();
  }
  virtual size_t lines() const
  {
    size_t res = 1;
    for(char c : str){
      if(c == '\n')
        res ++;
    }
    return res;
  }
  virtual std::string show(void) const
  {
    return str;
  }
  virtual char at(size_t i) const
  {
    if(i >= size())
      throw std::out_of_range("at(): argument out of range");
    return str[i];
  }
  virtual void edit(size_t i, char c)
  {
    if(i >= size())
      throw std::out_of_range("edit(): argument out of range");
    str[i] = c;
  }
  virtual void insert(size_t i, char c)
  {
    if(i > size())
      throw std::out_of_range("insert(): argument out of range");
    std::string tmp;
    tmp.push_back(c);
    str.insert(i, tmp);
  }
  virtual void erase(size_t i)
  {
    if(i >= size())
      throw std::out_of_range("erase(): argument out of range");
    str.erase(i, 1);
  }
  virtual size_t line_start(size_t r) const
  {
    if(r >= lines())
      throw std::out_of_range("line_start(): argument out of range");
    size_t res = 0;
    while(r > 0 && res < str.size()){
      if(str[res] == '\n')
        r --;
      res ++;
    }
    return res;
  }
  virtual size_t line_length(size_t r) const
  {
    if(r >= lines())
      throw std::out_of_range("line_length(): argument out of range");
    return ((r + 1 == lines()) ? (str.size()) : line_start(r + 1)) - line_start(r);
  }
  virtual size_t char_to_line(size_t i) const
  {
    if(i >= str.size())
      throw std::out_of_range("char_to_line(): argument out of range");
    size_t res = 0;
    for(size_t j = 0; j < i; j ++){
      if(str[j] == '\n')
        res ++;
    }
    return res;
  }
};

class RandomGenerator{
private:
  int newLineChance;
public:
  RandomGenerator(int chance)
  : newLineChance(chance)
  {
    if(chance > 100 || chance < 0){
      throw std::invalid_argument("RandomGenerator(): invalid chance");
    }
    srand(time(0));
  }
  char newChar(void) const
  {
    if(rand() % 101 >= newLineChance){
      return rand() % 26 + 'a';
    }
    return '\n';
  }
  size_t newSize(size_t start, size_t end) const
  {
    if(end - start == 0)
      return start;
    size_t res = (rand() & 0xffffffff) | ((((size_t) rand()) << 32) & 0xffffffff00000000UL);
    res %= (end - start);
    return res + start;
  }

};

class Node{
  public:
  char value = '#';
  size_t depth = 0;
  size_t weight = 1;
  size_t newlines_before = 0;
  Node * left_son = nullptr;
  Node * right_son = nullptr;
  Node * predecesor = nullptr;

  Node(){};

  Node(char in){
    value = in;
  }

  void bounce(){
    if((left_son == nullptr)&&(right_son == nullptr)){
      this->bubble_up();
    }
    if(left_son != nullptr){
      left_son->bounce();
    }
    if(right_son != nullptr){
      right_son->bounce();
    }
  }

  void bubble_up(){
    if(predecesor != nullptr){
      this->update_state();
      predecesor->bubble_up();
    }
    //that means im in root
    else{
      this->update_state();
    }
  }

  void update_state(){
    if((left_son != nullptr) && (right_son != nullptr)){
      depth = std::max(left_son->depth, right_son->depth) + 1;
      weight = left_son->weight + right_son->weight + 1;
      return;
    }

    if(left_son != nullptr){
      depth = left_son->depth + 1;
      weight = left_son->weight + 1;
      return; 
    }

    if(right_son != nullptr){
      depth = right_son->depth + 1;
      weight = right_son->weight + 1;
      return; 
    }

    depth = 0;
    weight = 1;
  }

  void update_newline_count(int value){
    Node * src = this;

    while(src->predecesor){
      if(src->predecesor->left_son == src){
        src->predecesor->newlines_before = src->predecesor->newlines_before + value;
      }
      src = src->predecesor;
    }
  }

  void update_newline_count_until_meet(int value, Node * end_node){
    Node * src = this;

    while(src->predecesor){
      if(src == end_node){
        return;
      }
      if(src->predecesor->left_son == src){
        src->predecesor->newlines_before = src->predecesor->newlines_before + value;
      }
      src = src->predecesor;
    }
  }

  std::string print(void) const
  {
    return ((left_son) ? (left_son->print()) : (std::string())) + value + ((right_son) ? (right_son->print()) : (std::string()));
  }
};

class One_line{
  public:
  size_t number_of_chars = 0;
  size_t number_of_newlines = 1;

  Node * root = nullptr;

  std::vector<Node*> to_be_updated_newlines;

  One_line(){}

  void create_one_line(const std::string & input){
    size_t lenght = input.length();
    
    if(lenght == 0){
      number_of_chars = 0;
      root = nullptr;
      return;
    }

    number_of_chars = lenght;
    
    to_be_updated_newlines.clear();

    this->root = create_tree(input, 0, lenght - 1, nullptr);

    for(auto itr = to_be_updated_newlines.begin(); itr != to_be_updated_newlines.end();  itr++){
      (*itr)->update_newline_count(1);
      number_of_newlines++;
    }

    to_be_updated_newlines.clear();

    this->update_whole_tree();
  }

  ~One_line(){
    delete_Node(root);
  }

  void right_rotation(Node * src){

    Node * to_be_up = src->left_son;
    
    //if im root
    if(src->predecesor == nullptr){
      root = to_be_up; 
    }

    else if(src->predecesor->left_son == src){
      src->predecesor->left_son = to_be_up;
    }
    else{
      src->predecesor->right_son = to_be_up;
    }

    src->newlines_before = src->newlines_before - to_be_up->newlines_before;

    if(to_be_up->value == '\n' ){
      src->newlines_before--;
    }

    to_be_up->predecesor = src->predecesor;

    Node * middle_tree = to_be_up->right_son;

    to_be_up->right_son = src;
    src->predecesor = to_be_up;
    src->left_son = middle_tree;
    if(middle_tree != nullptr){
      middle_tree->predecesor = src;
    }
  }

  void left_rotation(Node * src){

    Node * to_be_up = src->right_son;
    
    //if im root
    if(src->predecesor == nullptr){
      root = to_be_up; 
    }

    else if(src->predecesor->left_son == src){
      src->predecesor->left_son = to_be_up;
    }
    else{
      src->predecesor->right_son = to_be_up;
    }

    to_be_up->newlines_before = to_be_up->newlines_before + src->newlines_before;
    if(src->value == '\n'){
      to_be_up->newlines_before++;
    }

    to_be_up->predecesor = src->predecesor;

    Node * middle_tree = to_be_up->left_son;

    to_be_up->left_son = src;
    src->predecesor = to_be_up;
    src->right_son = middle_tree;
    if(middle_tree != nullptr){
      middle_tree->predecesor = src;
    }
  }

  void left_right_rotation(Node * src){
    left_rotation(src->left_son);
    right_rotation(src);
  }

  void right_left_rotation(Node * src){
    right_rotation(src->right_son);
    left_rotation(src);
  }

  int balance_value(Node * src){

    if(src == nullptr){
      return 0;
    }

    int left_depth = -1;
    int right_depth = -1;

    if(src->left_son != nullptr){
      left_depth = int(src->left_son->depth);
    }

    if(src->right_son != nullptr){
      right_depth = int(src->right_son->depth);
    }

    int balance = right_depth - left_depth;

    return balance;
  }

  void rotation_check(Node * src){
    int balance = balance_value(src);

    if( balance >= 2){
      if(balance_value(src->right_son) <= -1){
        Node * buble = src->right_son; 
        right_left_rotation(src);
        bubble_up(src);
        bubble_up(buble);
      }
      else{
        left_rotation(src);
        bubble_up(src);
      }
    }
    else if(balance <= -2){
      if(balance_value(src->left_son) >= 1){
        Node * buble = src->left_son; 
        left_right_rotation(src);
        bubble_up(src);
        bubble_up(buble);
      }
      else{
        right_rotation(src);
        bubble_up(src);
      }
    }
  }

  void update_state(Node * src){
    if((src->left_son != nullptr) && (src->right_son != nullptr)){
      src->depth = std::max(src->left_son->depth, src->right_son->depth) + 1;
      src->weight = src->left_son->weight + src->right_son->weight + 1;
    }

    else if(src->left_son != nullptr){
      src->depth = src->left_son->depth + 1;
      src->weight = src->left_son->weight + 1;
    }

    else if(src->right_son != nullptr){
      src->depth = src->right_son->depth + 1;
      src->weight = src->right_son->weight + 1;
    }
    else{
      src->depth = 0;
      src->weight = 1;
    }

    rotation_check(src);
  }

  void bubble_up(Node * src){
    update_state(src);
    if(src->predecesor != nullptr){
      bubble_up(src->predecesor);
    }
  }

  Node * create_tree(const std::string & input ,int start, int end, Node * father){
    if (start > end) {
        return nullptr;
    }

    int middle = (start + end) / 2;
    Node * cur_node = new Node(input[middle]);
    cur_node->predecesor = father;
    
    cur_node->left_son = create_tree(input, start, middle - 1, cur_node);
    cur_node->right_son = create_tree(input, middle + 1, end, cur_node);

    if(input[middle] == '\n'){
      to_be_updated_newlines.push_back(cur_node);
    }

    return cur_node;

  }

  Node * search(Node * current, size_t index) const{
    size_t left_weight;

    if(!current){
      throw std::invalid_argument("this definitely shouldnt be null");
    }

    if(current->left_son == nullptr){
      left_weight = 0;
    }
    else{
      left_weight = current->left_son->weight;
    }

    if(left_weight > index){
      return search(current->left_son, index);
    }

    if(left_weight +1 <= index){
      if(current->right_son == nullptr){
        return current;
      }
      return search(current->right_son, (index - left_weight - 1));
    }
    return current;
  }

  Node * find_succesor(Node * src) const {

    if(src->right_son != nullptr){
      Node * succesor = src->right_son;
        while(succesor->left_son != nullptr){
          succesor = succesor->left_son;
      }
      return succesor;
    }

    while(src->predecesor != nullptr){
      if(src->predecesor->left_son == src){
        return src->predecesor;
      }
      else{
        src = src->predecesor;
      }
    }
    //if this while loop ended and i didnt find it it means im holding the last node
    return nullptr;

  }

  char at(size_t index) const{
    if(number_of_chars == 0){
      throw std::out_of_range("theres is 0 nodes in this tree");
    }
    if(index > number_of_chars -1){
      throw std::out_of_range("Index out of range");
    }
    Node * ret = this->search(root,index);

    return ret->value;
  }

  void edit(size_t index , char c){
    if(number_of_chars == 0){
      throw std::out_of_range("theres is 0 nodes in this tree");
    }
    if(index > number_of_chars -1){
      throw std::out_of_range("Index out of range");
    }
    Node * n = search(root, index);

    if(n->value == '\n'){
      if(c == '\n'){
        return;
      }
      n->update_newline_count(-1);
      number_of_newlines--;
    }
    else if(c == '\n'){
      n->update_newline_count(1);
      number_of_newlines++;
    }

    n->value = c;
  }

  void insert(size_t index , char c){

    if(index > number_of_chars){
      throw std::out_of_range("Index out of range");
      return;
    }

    if(c == '\n'){
      
      if(number_of_chars == 0){
      Node * tmp = new Node(c);
      root = tmp;
      number_of_chars++;
      number_of_newlines++;
      return;
      }

      insert(index, '#');
      edit(index, '\n');
      return;
    }

    if(number_of_chars == 0){
      Node * tmp = new Node(c);
      root = tmp;
      number_of_chars++;
      return;
    }

    if(number_of_chars == index){
      Node * current = root;
      while(current->right_son != nullptr){
        current = current->right_son;
      }
      Node * tmp = new Node(c);
      number_of_chars++;

      current->right_son = tmp;
      tmp->predecesor = current;
      bubble_up(tmp);
      return;
    }

    Node * n = search(root,index);

    //if there is no left son its simple
    if(n->left_son == nullptr){
      Node * tmp = new Node(c);
      tmp->predecesor = n;
      n->left_son = tmp;
      bubble_up(tmp);
    }

    //if he had left
    else{
      Node * tmp = new Node(c);
      tmp->left_son = n->left_son;
      tmp->right_son = n;
      tmp-> predecesor = n->predecesor;

      //need to fix the newlines before cuz it will change here
      tmp->newlines_before = n->newlines_before;
      n->newlines_before = 0;

      if(n->predecesor != nullptr){
        //if n was the right son or left son i should change it for the current one
        if(n->predecesor->right_son == n){
          n->predecesor->right_son = tmp;
        }
        else{
          n->predecesor->left_son = tmp;
        }
      }
      else{
        root = tmp;
      }

      tmp->left_son->predecesor = tmp;
      n->predecesor = tmp;
      n->left_son = nullptr;
      bubble_up(n);
    }

    number_of_chars++;
  }

  void erase(size_t index){
    if(number_of_chars == 0){
      throw std::out_of_range("theres is 0 nodes in this tree");
    }

    if(index > number_of_chars -1){
      throw std::out_of_range("Index out of range");
      return;
    }

    Node * n = search(root,index);

    if(n->value == '\n'){
      edit(index,'#');
      erase(index);
      return;
    }

    //if im leaf
    if((n->left_son == nullptr) && (n->right_son == nullptr)){
      //check whether im not root
      if(n->predecesor == nullptr){
        delete n;
        number_of_chars--;
        root = nullptr;
        return;
      }
      
      //change the value of the predecesor
      if(n->predecesor->left_son == n){
        n->predecesor->left_son = nullptr;
      }

      else if(n->predecesor->right_son == n){
        n->predecesor->right_son = nullptr;
      }
      bubble_up(n->predecesor);
      delete n;
    }

    //if i only have left son
    else if(n->right_son == nullptr){
      //if im root
      if(n->predecesor == nullptr){
        n->left_son->predecesor = nullptr;
        root = n->left_son;
        delete n;
      }
      
      else if(n->predecesor->left_son == n){
        n->predecesor->left_son = n->left_son;
        n->left_son->predecesor = n->predecesor;
        bubble_up(n->predecesor);
        delete n;
      }

      else if(n->predecesor->right_son == n){
        n->predecesor->right_son = n->left_son;
        n->left_son->predecesor = n->predecesor;
        bubble_up(n->predecesor);
        delete n;
      }
    }

    //if o only have right son
    else if(n->left_son == nullptr){
      //if im root
      if(n->predecesor == nullptr){
        n->right_son->predecesor = nullptr;
        root = n->right_son;
        delete n;
      }
      else if(n->predecesor->left_son == n){
        n->predecesor->left_son = n->right_son;
        n->right_son->predecesor = n->predecesor;
        bubble_up(n->predecesor);
        delete n;
      }

      else if(n->predecesor->right_son == n){
        n->predecesor->right_son = n->right_son;
        n->right_son->predecesor = n->predecesor;
        bubble_up(n->predecesor);
        delete n;
      }
    }

    //if i have two sons
    else{
      //find succesor
      Node * succesor = n->right_son;
      while(succesor->left_son != nullptr){
        succesor = succesor->left_son;
      }

      //this causes trouble
      if(succesor->value == '\n'){
        succesor->update_newline_count_until_meet(-1,n);
      }

      n->value = succesor->value;

      //if succesor was a leafe
      if(succesor->right_son == nullptr){
        if(succesor->predecesor->left_son == succesor){
          succesor->predecesor->left_son = nullptr;
        }
        else if(succesor->predecesor->right_son == succesor){
          succesor->predecesor->right_son = nullptr;
        }
        bubble_up(succesor->predecesor);
        delete succesor;
      }
      //if succesor had right son
      else{
        if(succesor->predecesor->left_son == succesor){
        succesor->predecesor->left_son = succesor->right_son;
        succesor->right_son->predecesor = succesor->predecesor;
        bubble_up(succesor->predecesor);
        delete succesor;
      }

      else if(succesor->predecesor->right_son == succesor){
        succesor->predecesor->right_son = succesor->right_son;
        succesor->right_son->predecesor = succesor->predecesor;
        bubble_up(succesor->predecesor);
        delete succesor;
      }
      }
      

    }
    number_of_chars--;
  }

  void update_whole_tree(){
    root->bounce();
    number_of_chars = root->weight;
  }

  void print_one_rec(Node * src){
    if(src == nullptr){
      return;
    }
    print_one_rec(src->left_son);
    std::cout << src->value << " ";
    print_one_rec(src->right_son);
  }

  void print_tree(){
    print_one_rec(root);
  }

  void delete_Node(Node * src){
    if(src == nullptr){
      return;
    }

    if(src->left_son != nullptr){
      delete_Node(src->left_son);
    }

    if(src->right_son != nullptr){
      delete_Node(src->right_son);
    }

    delete src;
  }

  std::pair<size_t, Node *> find_nth_newline_char(Node * start,size_t index, size_t node_index) const {
    if(start == nullptr){
      throw std::out_of_range("this rly shouldnt be nullptr");
    }
    if(index == 0){
      throw std::out_of_range("index shouldnt be 0 in this method");
    }

    Node * current_node = start;

    if(current_node->newlines_before >= index){
      return find_nth_newline_char (current_node->left_son, index,node_index);
    }

    int flag = 0;
    if(current_node->value == '\n'){
      flag = 1;
    }

    if(current_node->newlines_before + flag < index){
      return find_nth_newline_char(current_node->right_son, index - current_node->newlines_before - flag,node_index + current_node->weight - current_node->right_son->weight);
    }
    else{
      if(current_node->left_son != nullptr){
        node_index = node_index + current_node->left_son->weight;
      }
      return std::pair<size_t, Node *>(node_index, current_node);
    }

  }

  size_t size() const {
    return number_of_chars;
  }

  size_t lines() const {
    return number_of_newlines;
  }

  size_t line_start(size_t r) const{
    if(r == 0){
      return 0;
    }

    if(number_of_newlines <= r){
      throw std::out_of_range("Index out of range");
    }

    return (find_nth_newline_char(root,r,0).first + 1);
  }
  
  size_t line_length(size_t r) const{
    if(number_of_newlines <= r){
      throw std::out_of_range("Index out of range");
    }
    
    if(number_of_newlines == 1){
      return number_of_chars;
    }

    
    if(r == 0){
      return find_nth_newline_char(root,1,0).first+1;
    }

    if(r == number_of_newlines - 1){
      Node * tmp = find_nth_newline_char(root,r,0).second;
      size_t ret = 0;
      while(tmp!=nullptr){
        tmp = find_succesor(tmp);
        if(tmp){
          ret++;
        }
      }
      return ret;
    }

    return find_nth_newline_char(root,r+1,0).first - find_nth_newline_char(root,r,0).first;

  }

  size_t char_to_line(size_t i) const{
    if(i > number_of_chars -1){
      throw std::out_of_range("Index out of range");
    }
    if(number_of_chars == 0){
      throw std::out_of_range("Index out of range");
    }
    return search_line_index(root, i, 0);
  }

  size_t search_line_index(Node * current, size_t index, size_t line_num) const{
    size_t left_weight;

    int line_bool = 0;

    if(current->value == '\n'){
      line_bool = 1;
    }

    if(current->left_son == nullptr){
      left_weight = 0;
    }
    else{
      left_weight = current->left_son->weight;
    }

    if(left_weight > index){
      return search_line_index(current->left_son, index,line_num);
    }

    if(left_weight +1 <= index){
      if(current->right_son == nullptr){
        return line_num + current->newlines_before + line_bool;
      }
      return search_line_index(current->right_son, (index - left_weight - 1),line_num + current->newlines_before + line_bool);
    }
    return line_num + current->newlines_before;
  }

  std::string print(void) const
  {
    if(root == nullptr){
      return std::string();
    }
    return root->print();
  }
};

struct TextEditorBackend {

  One_line text;

  TextEditorBackend(const std::string& text){
    this->text.create_one_line(text);
  }
  size_t size() const{
    return text.size();
  }
  size_t lines() const{
    return text.lines();
  }

  char at(size_t i) const{
    return text.at(i);
  }
  void edit(size_t i, char c){
    text.edit(i,c);
  }
  void insert(size_t i, char c){
    text.insert(i,c);
  }
  void erase(size_t i){
    text.erase(i);
  }

  size_t line_start(size_t r) const{
    return text.line_start(r);
  }
  size_t line_length(size_t r) const{
    return text.line_length(r);
  }
  size_t char_to_line(size_t i) const{
    return text.char_to_line(i);
  }

  std::string show(void) const
  {
    return text.print();
  }
};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////
template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)
 
////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend& t) {
  std::string ret;
  for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
  return ret;
}

void test1(int& ok, int& fail) {
	TextEditorBackend s("123\n456\n789");
  CHECK(s.size(), 11);
	CHECK(text(s), "123\n456\n789");
  CHECK(s.lines(), 3);
  CHECK_ALL(s.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2);
  CHECK_ALL(s.line_start, 0, 4, 8);
  CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK(t.size(), 12);
	CHECK(text(t), "123\n456\n789\n");
  CHECK(t.lines(), 4);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2,2);
  CHECK_ALL(t.line_start, 0, 4, 8, 12);
  CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int& ok, int& fail) {
	TextEditorBackend t("asdfasdfasdf");

  CHECK(t.size(), 12);
  CHECK(text(t), "asdfasdfasdf");
	CHECK(t.lines(), 1);
  CHECK_ALL(t.char_to_line, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	CHECK(t.line_start(0), 0);
	CHECK(t.line_length(0), 12);
	
	t.insert(0, '\n');
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasdfasdf");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
	
  t.insert(4, '\n');
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasdfasdf");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);
	
  t.insert(t.size(), '\n');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdf\n");
	CHECK(t.lines(), 4);
  CHECK_ALL(t.line_start, 0, 1, 5, 15);
	
	t.edit(t.size() - 1, 'H');
  CHECK(t.size(), 15);
  CHECK(text(t), "\nasd\nfasdfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

	t.erase(8);
  CHECK(t.size(), 14);
  CHECK(text(t), "\nasd\nfasfasdfH");
	CHECK(t.lines(), 3);
  CHECK_ALL(t.line_start, 0, 1, 5);

  t.erase(4);
  CHECK(t.size(), 13);
  CHECK(text(t), "\nasdfasfasdfH");
	CHECK(t.lines(), 2);
  CHECK_ALL(t.line_start, 0, 1);
}

void test_ex(int& ok, int& fail) {
	TextEditorBackend t("123\n456\n789\n");
  CHECK_EX(t.at(12), std::out_of_range);

  CHECK_EX(t.insert(13, 'a'), std::out_of_range);
  CHECK_EX(t.edit(12, 'x'), std::out_of_range);
  CHECK_EX(t.erase(12), std::out_of_range);

  CHECK_EX(t.line_start(4), std::out_of_range);
  CHECK_EX(t.line_start(40), std::out_of_range);
  CHECK_EX(t.line_length(4), std::out_of_range);
  CHECK_EX(t.line_length(6), std::out_of_range);
  CHECK_EX(t.char_to_line(12), std::out_of_range);
  CHECK_EX(t.char_to_line(25), std::out_of_range);
}


void modifyAction(RandomGenerator & gen, StupidSolution & correct, TextEditorBackend & test, bool log, bool empty){
  char c = gen.newChar();
  int action = (empty) ? (2) : (rand() % 5);
  size_t pos;
  switch(action){
    case 0:
    case 1:
      pos = gen.newSize(0, correct.size());
      if(log)
        std::cout << "modifyAction - edit  " " char '" << c << "' position " << pos << std::endl;
      test.edit(pos, c);
      correct.edit(pos, c);
      break;
    case 2:
    case 3:
      pos = gen.newSize(0, correct.size() + 1);
      if(log)
        std::cout << "modifyAction - insert  " << " char '" << c << "' position " << pos << std::endl;
      test.insert(pos, c);
      correct.insert(pos, c);
      break;
    default:
      pos = gen.newSize(0, correct.size());
      if(log)
        std::cout << "modifyAction - erase  " << "' position " << pos << std::endl;
      test.erase(pos);
      correct.erase(pos);
      break;
  }
}

void checkAction(RandomGenerator & gen, StupidSolution & correct, TextEditorBackend & test, bool log){
  int action = rand() % 3;
  size_t pos;
  switch(action){
    case 0:
      pos = gen.newSize(0, correct.lines());
      if(log)
        std::cout << "checkAction " << "line_start  " << " row: " << pos << ", student: " << test.line_start(pos) << ", reference: " << correct.line_start(pos) << std::endl;
      assert(test.line_start(pos) == correct.line_start(pos));
      break;
    case 1:
      pos = gen.newSize(0, correct.lines());
      if(log)
        std::cout << "checkAction " << "line_lenght" << " row: " << pos << ", student: " << test.line_length(pos) << ", reference: " << correct.line_length(pos) << std::endl;
      assert(test.line_length(pos) == correct.line_length(pos));
      break;
    default:
      pos = gen.newSize(0, correct.size());
      if(log)
        std::cout << "checkAction " << "char_to_line" << " idx: " << pos << ", student: " << test.char_to_line(pos) << ", reference: " << correct.char_to_line(pos) << std::endl;
      assert(test.char_to_line(pos) == correct.char_to_line(pos));
      break;
  }
}



int main() {
  int ok = 0, fail = 0;
  if (!fail) test1(ok, fail);
  if (!fail) test2(ok, fail);
  if (!fail) test3(ok, fail);
  if (!fail) test_ex(ok, fail);
  
  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;

  StupidSolution correct = StupidSolution("");
  TextEditorBackend test("");
  RandomGenerator gen(6);

  for(int i = 0; i < 200; i ++){
    for(int j = 0; j < 20; j ++){
      modifyAction(gen, correct, test, 1, correct.size() == 0);
      assert(test.size() == correct.size());
      assert(test.lines() == correct.lines());
      assert(test.show() == correct.show());  
    }
    if(correct.size() != 0){
      for(int j = 0; j < 20; j ++){
        checkAction(gen, correct, test, 1);
      }
    }
  }
  std::cout << "Passed also all random tests." << std::endl;
  
  return 0;

}
#endif