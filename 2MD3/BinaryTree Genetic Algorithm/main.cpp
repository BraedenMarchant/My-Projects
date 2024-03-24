#include <math.h>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <random>
#include <stack>
#include <vector>

#include "cartCentering.h"

using namespace std;

// return a double unifomrly sampled in (0,1)
double randDouble(mt19937& rng) {
  return std::uniform_real_distribution<>{0, 1}(rng);
}
// return uniformly sampled 0 or 1
bool randChoice(mt19937& rng) {
  return std::uniform_int_distribution<>{0, 1}(rng);
}
// return a random integer uniformly sampled in (min, max)
int randInt(mt19937& rng, const int& min, const int& max) {
  return std::uniform_int_distribution<>{min, max}(rng);
}

// return true if op is a suported operation, otherwise return false
bool isOp(string op) {
  if (op == "+")
    return true;
  else if (op == "-")
    return true;
  else if (op == "*")
    return true;
  else if (op == "/")
    return true;
  else if (op == ">")
    return true;
  else if (op == "abs")
    return true;
  else
    return false;
}

int arity(string op) {
  if (op == "abs")
    return 1;
  else
    return 2;
}

typedef string Elem;

class LinkedBinaryTree {
 public:
  struct Node {
    Elem elt;
    string name;
    Node* par;
    Node* left;
    Node* right;
    Node() : elt(), par(NULL), name(""), left(NULL), right(NULL) {}
    int depth() {
      if (par == NULL) return 0;
      return par->depth() + 1;
    }
  };

  class Position {
   private:
    Node* v;

   public:
    Node* getNode() const { return v; }
    Position(Node* _v = NULL) : v(_v) {}
    Elem& operator*() { return v->elt; }
    Position left() const { return Position(v->left); }
    void setLeft(Node* n) { v->left = n; }
    Position right() const { return Position(v->right); }
    void setRight(Node* n) { v->right = n; }
    Position parent() const  // get parent
    {
      return Position(v->par);
    }
    bool isRoot() const  // root of the tree?
    {
      return v->par == NULL;
    }
    bool isExternal() const  // an external node?
    {
      return v->left == NULL && v->right == NULL;
    }
    friend class LinkedBinaryTree;  // give tree access
  };
  typedef vector<Position> PositionList;

 public:
  LinkedBinaryTree() : _root(NULL), score(0), steps(0), generation(0) {}

  Node* getRoot() const { return _root; }

  // copy constructor
  LinkedBinaryTree(const LinkedBinaryTree& t) {
    _root = copyPreOrder(t.root());
    score = t.getScore();
    steps = t.getSteps();
    generation = t.getGeneration();
  }

  // copy assignment operator
  LinkedBinaryTree& operator=(const LinkedBinaryTree& t) {
    if (this != &t) {
      // if tree already contains data, delete it
      if (_root != NULL) {
        PositionList pl = positions();
        for (auto& p : pl) delete p.v;
      }
      _root = copyPreOrder(t.root());
      score = t.getScore();
      steps = t.getSteps();
      generation = t.getGeneration();
    }
    return *this;
  }

  // destructor
  ~LinkedBinaryTree() {
    if (_root != NULL) {
      PositionList pl = positions();
      for (auto& p : pl) delete p.v;
    }
  }

  int size() const { return size(_root); }
  int size(Node* root) const;
  int depth() const;
  bool empty() const { return size() == 0; };
  Node* root() const { return _root; }
  PositionList positions() const;
  void addRoot() { _root = new Node; }
  void addRoot(Elem e) {
    _root = new Node;
    _root->elt = e;
  }
  void nameRoot(string name) { _root->name = name; }
  void addLeftChild(const Position& p, const Node* n);
  void addLeftChild(const Position& p);
  void addRightChild(const Position& p, const Node* n);
  void addRightChild(const Position& p);
  void printExpression() { printExpression(_root); }
  void printExpression(Node* v);
  double evaluateExpression(double a, double b) {
    return evaluateExpression(Position(_root), a, b);
  };
  double evaluateExpression(const Position& p, double a, double b);
  long getGeneration() const { return generation; }
  void setGeneration(int g) { generation = g; }
  double getScore() const { return score; }
  void setScore(double s) { score = s; }
  double getSteps() const { return steps; }
  void setSteps(double s) { steps = s; }
  void randomExpressionTree(Node* p, const int& maxDepth, mt19937& rng);
  void randomExpressionTree(const int& maxDepth, mt19937& rng) {
    randomExpressionTree(_root, maxDepth, rng);
  }
  void deleteSubtreeMutator(mt19937& rng);
  void addSubtreeMutator(mt19937& rng, const int maxDepth);
  void delete_subtree_recursive(Node* node);

  LinkedBinaryTree crossover(const LinkedBinaryTree& parent1,
                             const LinkedBinaryTree& parent2, mt19937& rng);

 protected:                                        // local utilities
  void preorder(Node* v, PositionList& pl) const;  // preorder utility
  Node* copyPreOrder(const Node* root);
  double score;     // mean reward over 20 episodes
  double steps;     // mean steps-per-episode over 20 episodes
  long generation;  // which generation was tree "born"
 private:
  Node* _root;  // pointer to the root
};

// add the tree rooted at node child as this tree's left child
void LinkedBinaryTree::addLeftChild(const Position& p, const Node* child) {
  Node* v = p.v;
  v->left = copyPreOrder(child);  // deep copy child
  v->left->par = v;
}

// add the tree rooted at node child as this tree's right child
void LinkedBinaryTree::addRightChild(const Position& p, const Node* child) {
  Node* v = p.v;
  v->right = copyPreOrder(child);  // deep copy child
  v->right->par = v;
}

void LinkedBinaryTree::addLeftChild(const Position& p) {
  Node* v = p.v;
  v->left = new Node;
  v->left->par = v;
}

void LinkedBinaryTree::addRightChild(const Position& p) {
  Node* v = p.v;
  v->right = new Node;
  v->right->par = v;
}

// return a list of all nodes
LinkedBinaryTree::PositionList LinkedBinaryTree::positions() const {
  PositionList pl;
  preorder(_root, pl);
  return PositionList(pl);
}

void LinkedBinaryTree::preorder(Node* v, PositionList& pl) const {
  pl.push_back(Position(v));
  if (v->left != NULL) preorder(v->left, pl);
  if (v->right != NULL) preorder(v->right, pl);
}

int LinkedBinaryTree::size(Node* v) const {
  int lsize = 0;
  int rsize = 0;
  if (v->left != NULL) lsize = size(v->left);
  if (v->right != NULL) rsize = size(v->right);
  return 1 + lsize + rsize;
}

int LinkedBinaryTree::depth() const {
  PositionList pl = positions();
  int depth = 0;
  for (auto& p : pl) depth = std::max(depth, p.v->depth());
  return depth;
}

LinkedBinaryTree::Node* LinkedBinaryTree::copyPreOrder(const Node* root) {
  if (root == NULL) return NULL;
  Node* nn = new Node;
  nn->elt = root->elt;
  nn->left = copyPreOrder(root->left);
  if (nn->left != NULL) nn->left->par = nn;
  nn->right = copyPreOrder(root->right);
  if (nn->right != NULL) nn->right->par = nn;
  return nn;
}

void LinkedBinaryTree::printExpression(Node* v) {
  if (v == nullptr) return;

  if (v->left != nullptr) {
    cout << "(";
  }
  //go through left subtree the node then right
  printExpression(v->left);
  cout << v->elt;
  printExpression(v->right);

  if (v->left != nullptr) {
    cout << ")";
  }
}

double evalOp(string op, double x, double y = 0) {
  double result;
  if (op == "+")
    result = x + y;
  else if (op == "-")
    result = x - y;
  else if (op == "*")
    result = x * y;
  else if (op == "/") {
    result = x / y;
  } else if (op == ">") {
    result = x > y ? 1 : -1;
  } else if (op == "abs") {
    result = abs(x);
  } else
    result = 0;
  return isnan(result) || !isfinite(result) ? 0 : result;
}

double LinkedBinaryTree::evaluateExpression(const Position& p, double a,
                                            double b) {
  if (!p.isExternal()) {
    auto x = evaluateExpression(p.left(), a, b);
    if (arity(p.v->elt) > 1) {
      auto y = evaluateExpression(p.right(), a, b);
      return evalOp(p.v->elt, x, y);
    } else {
      return evalOp(p.v->elt, x);
    }
  } else {
    if (p.v->elt == "a")
      return a;
    else if (p.v->elt == "b")
      return b;
    else
      return stod(p.v->elt);
  }
}

void LinkedBinaryTree::delete_subtree_recursive(Node* node) {
  if (!node) return;

  // delete left and right subtrees
  delete_subtree_recursive(node->left);
  node->left = nullptr;  // Set the pointer to null
  delete_subtree_recursive(node->right);
  node->right = nullptr;  // Set the pointer to null

  // Delete the node
  delete node;
}

void LinkedBinaryTree::deleteSubtreeMutator(mt19937& rng) {
  PositionList all_positions = positions();
  PositionList internal_nodes;

  int rand_indx =
      randInt(rng, 1, all_positions.size() - 1);  // Avoid selecting root
  Position pos_to_delete = all_positions[rand_indx];
  Node* node_to_delete = pos_to_delete.v;

  if (!pos_to_delete.isRoot()) {
    // Delete all of the children of the randomly selected node
    delete_subtree_recursive(node_to_delete->left);
    node_to_delete->left = nullptr;
    delete_subtree_recursive(node_to_delete->right);
    node_to_delete->right = nullptr;

    // Replace node with "a" or "b" randomly
    node_to_delete->elt = randChoice(rng) ? "a" : "b";
  }
}

bool operator<(const LinkedBinaryTree& x, const LinkedBinaryTree& y) {
  return x.getScore() < y.getScore();
}

LinkedBinaryTree createExpressionTree(string expression) {
  stack<LinkedBinaryTree> tree_stack;
  stringstream ss(expression);
  // Split each line into words
  string token;
  while (getline(ss, token, ' ')) {
    LinkedBinaryTree t;
    if (!isOp(token)) {
      t.addRoot(token);
      tree_stack.push(t);
    } else {
      t.addRoot(token);
      if (arity(token) > 1) {
        LinkedBinaryTree r = tree_stack.top();
        tree_stack.pop();
        t.addRightChild(t.root(), r.root());
      }
      LinkedBinaryTree l = tree_stack.top();
      tree_stack.pop();
      t.addLeftChild(t.root(), l.root());
      tree_stack.push(t);
    }
  }
  return tree_stack.top();
}

LinkedBinaryTree createRandExpressionTree(int max_depth, mt19937& rng) {
  // modify this function to create and return a random expression tree
  int rand_depth = randInt(rng, 0, max_depth);
  string expression = "";
  for (int i = 0; i < max_depth; i++) {
    string operation;
    switch (randInt(rng, 0, 5)) {
      case 0:
        operation = "+";
        break;
      case 1:
        operation = "-";
        break;
      case 2:
        operation = "*";
        break;
      case 3:
        operation = "/";
        break;
      case 4:
        operation = ">";
        break;
      case 5:
        operation = "abs";
        break;
    }

    if (arity(operation) == 1) {
      string variable;
      switch (randChoice(rng)) {
        case 0:
          variable = "a ";
          break;
        case 1:
          variable = "b ";
          break;
      }
      expression += variable;
    }

    else {
      string variable1, variable2;
      switch (randChoice(rng)) {
        case 0:
          variable1 = "a ";
          break;
        case 1:
          variable1 = "b ";
          break;
      }
      expression += variable1;

      switch (randChoice(rng)) {
        case 0:
          variable2 = "a ";
          break;
        case 1:
          variable2 = "b ";
          break;
      }

      expression += variable2;
    }
    expression += (operation + " ");
  }

  LinkedBinaryTree t = createExpressionTree(expression);
  return t;
}

void LinkedBinaryTree::addSubtreeMutator(mt19937& rng, const int maxDepth) {
  PositionList all_positions = positions();
  PositionList external_nodes;

  // only take the external nodes
  for (int i = 0; i < all_positions.size(); i++) {
    if (all_positions[i].isExternal()) {
      external_nodes.push_back(all_positions[i]);
    }
  }

  // Select a random external node
  int index_to_add = randInt(rng, 0, external_nodes.size() - 1);
  Position node_to_add_to = external_nodes[index_to_add];

  // make a new subtree using the createRand
  LinkedBinaryTree subtree = createRandExpressionTree(maxDepth, rng);

  // Transform the selected node into the new subtree's root
  Node* node_to_transform = node_to_add_to.v;
  node_to_transform->elt = subtree.root()->elt;

  if (subtree.root()->left != nullptr) {
    node_to_transform->left = subtree.root()->left;
  }

  if (subtree.root()->right != nullptr) {
    node_to_transform->right = subtree.root()->right;
  }

  // avoid deleting nodes when subtree goes out of scope
  subtree._root = nullptr;
}

// evaluate tree t in the cart centering task
void evaluate(mt19937& rng, LinkedBinaryTree& t, const int& num_episode,
              bool animate) {
  cartCentering env;
  double mean_score = 0.0;
  double mean_steps = 0.0;
  for (int i = 0; i < num_episode; i++) {
    double episode_score = 0.0;
    int episode_steps = 0;
    env.reset(rng);
    while (!env.terminal()) {
      int action = t.evaluateExpression(env.getCartXPos(), env.getCartXVel());
      episode_score += env.update(action, animate);
      episode_steps++;
    }
    mean_score += episode_score;
    mean_steps += episode_steps;
  }
  t.setScore(mean_score / num_episode);
  t.setSteps(mean_steps / num_episode);
}

class LexLessThan {
 public:
  bool operator()(const LinkedBinaryTree& TA,
                  const LinkedBinaryTree& TB) const {
    // Calculate the difference in scores
    double scoreDifference = TA.getScore() - TB.getScore();

    // If the scores differ by less than 0.01, favor the tree with more nodes
    if (abs(scoreDifference) < 0.01) {
      // Compare by the number of nodes
      return TA.size() > TB.size();
    } else {
      // Otherwise, compare by scores
      return TA.getScore() < TB.getScore();
    }
  }
};

LinkedBinaryTree LinkedBinaryTree::crossover(const LinkedBinaryTree& parent1,
                                             const LinkedBinaryTree& parent2,
                                             mt19937& rng) {
  LinkedBinaryTree child(parent1);

  // Randomly select crossover points in each parent
  PositionList positions_parent1 = parent1.positions();
  PositionList positions_parent2 = parent2.positions();

  // Randomly select crossover points
  int crossover_point_parent1 = randInt(rng, 1, positions_parent1.size() - 1);
  int crossover_point_parent2 = randInt(rng, 1, positions_parent2.size() - 1);

  Position crossover_position_parent1 =
      positions_parent1[crossover_point_parent1];
  Position crossover_position_parent2 =
      positions_parent2[crossover_point_parent2];

  Node* node1_to_cross = crossover_position_parent1.v;
  Node* node1_par = node1_to_cross->par;
  Node* node2_to_cross = crossover_position_parent2.v;

  // Check if node is left or right then perform crossover
  if (node1_par->left == node1_to_cross) {
    if (node2_to_cross != nullptr) {
      Node* new_subtree = copyPreOrder(node2_to_cross);
      node1_par->left = new_subtree;
      // Update parent pointers of the new subtree
      if (new_subtree != nullptr) {
        new_subtree->par = node1_par;
      }
    }
  } else {
    if (node2_to_cross != nullptr) {
      Node* new_subtree = copyPreOrder(node2_to_cross);
      node1_par->right = new_subtree;
      // Update parent pointers of the new subtree
      if (new_subtree != nullptr) {
        new_subtree->par = node1_par;
      }
    }
  }
  return child;
}

int main() {
  mt19937 rng(42);
  // Experiment parameters
  const int NUM_TREE = 50;
  const int MAX_DEPTH_INITIAL = 1;
  const int MAX_DEPTH = 20;
  const int NUM_EPISODE = 20;
  const int MAX_GENERATIONS = 100;

  // Create an initial "population" of expression trees
  vector<LinkedBinaryTree> trees;
  for (int i = 0; i < NUM_TREE; i++) {
    LinkedBinaryTree t = createRandExpressionTree(MAX_DEPTH_INITIAL, rng);
    trees.push_back(t);
  }

  // Genetic Algorithm loop
  LinkedBinaryTree best_tree;
  std::cout << "generation,fitness,steps,size,depth" << std::endl;
  for (int g = 1; g <= MAX_GENERATIONS; g++) {
    // Fitness evaluation
    for (auto& t : trees) {
      if (t.getGeneration() < g - 1) continue;  // skip if not new
      evaluate(rng, t, NUM_EPISODE, false);
    }

    // sort trees using overloaded "<" op (worst->best)
    std::sort(trees.begin(), trees.end());

    // // sort trees using comparaor class (worst->best)
    std::sort(trees.begin(), trees.end(), LexLessThan());

    // erase worst 50% of trees (first half of vector)
    trees.erase(trees.begin(), trees.begin() + NUM_TREE / 2);

    // Print stats for best tree
    best_tree = trees[trees.size() - 1];
    std::cout << g << ",";
    std::cout << best_tree.getScore() << ",";
    std::cout << best_tree.getSteps() << ",";
    std::cout << best_tree.size() << ",";
    std::cout << best_tree.depth() << std::endl;

    // Selection and mutation
    while (trees.size() < NUM_TREE) {
      LinkedBinaryTree parent1 = trees[randInt(rng, 0, (NUM_TREE / 2) - 1)];
      LinkedBinaryTree parent2 = trees[randInt(rng, 0, (NUM_TREE / 2) - 1)];

      LinkedBinaryTree child(parent1);
      child.setGeneration(g);

      // call crossover that returns child
      child.crossover(parent1, parent2, rng);

      // mutators
      child.deleteSubtreeMutator(rng);
      child.addSubtreeMutator(rng, MAX_DEPTH);

      trees.push_back(child);
    }
  }

  // // Evaluate best tree with animation
  // const int num_episode = 3;
  // evaluate(rng, best_tree, num_episode, true);

  // Print best tree info
  std::cout << std::endl << "Best tree:" << std::endl;
  best_tree.printExpression();
  std::cout << endl;
  std::cout << "Generation: " << best_tree.getGeneration() << endl;
  std::cout << "Size: " << best_tree.size() << std::endl;
  std::cout << "Depth: " << best_tree.depth() << std::endl;
  std::cout << "Fitness: " << best_tree.getScore() << std::endl << std::endl;
}
