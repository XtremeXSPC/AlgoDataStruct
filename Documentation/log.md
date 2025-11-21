[ 53%] Building CXX object CMakeFiles/test_lists.dir/src/main_Linked_Lists.cc.o
[ 56%] Building CXX object CMakeFiles/test_heaps.dir/src/main_Heaps.cc.o
[ 59%] Building CXX object CMakeFiles/test_priority_queue.dir/src/main_Priority_Queue.cc.o
[ 62%] Building CXX object CMakeFiles/test_singly_list.dir/src/main_Singly_Linked_List.cc.o
[ 65%] Building CXX object CMakeFiles/test_rbt.dir/src/main_Red_Black_Tree.cc.o
[ 68%] Building CXX object CMakeFiles/test_stacks_queues.dir/src/main_Stacks_Queues.cc.o
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:111:39: error: use of
      undeclared identifier 'test_val'
  111 |   const int* succ     = bst.successor(test_val);
      |                                       ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:112:41: error: use of
      undeclared identifier 'test_val'
  112 |   const int* pred     = bst.predecessor(test_val);
      |                                         ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:114:32: error: use of
      undeclared identifier 'test_val'
  114 |   cout << "  Successor of " << test_val << ": " << (succ ? to_string(*succ) : "none") << '\n';
      |                                ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:115:34: error: use of
      undeclared identifier 'test_val'
  115 |   cout << "  Predecessor of " << test_val << ": " << (pred ? to_string(*pred) : "none") << '\n';
      |                                  ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:117:3: error: use of
      undeclared identifier 'test_val'
  117 |   test_val = 10;
      |   ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:118:28: error: use of
      undeclared identifier 'test_val'
  118 |   succ     = bst.successor(test_val);
      |                            ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:119:30: error: use of
      undeclared identifier 'test_val'
  119 |   pred     = bst.predecessor(test_val);
      |                              ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:120:32: error: use of
      undeclared identifier 'test_val'
  120 |   cout << "  Successor of " << test_val << ": " << (succ ? to_string(*succ) : "none") << '\n';
      |                                ^~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Binary_Search_Tree.cc:121:34: error: use of
      undeclared identifier 'test_val'
  121 |   cout << "  Predecessor of " << test_val << ": " << (pred ? to_string(*pred) : "none") << '\n';
      |                                  ^~~~~~~~
[ 71%] Linking CXX executable bin/test_singly_list
[ 75%] Linking CXX executable bin/test_heaps
[ 78%] Linking CXX executable bin/test_lists
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: error: no member named
      'RedBlackTree' in namespace 'ads::trees'; did you mean 'Red_Black_Tree'?
   31 | using ads::trees::RedBlackTree;
      |       ~~~~~~~~~~~~^~~~~~~~~~~~
      |                   Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/../include/ads/trees/Red_Black_Tree.hpp:62:7: note:
      'Red_Black_Tree' declared here
   62 | class Red_Black_Tree {
      |       ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:35:22: error: no template
      named 'RedBlackTree'; did you mean 'Red_Black_Tree'?
   35 | void print_rbt(const RedBlackTree<T>& tree, const string& name) {
      |                      ^~~~~~~~~~~~
      |                      Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:70:3: error: no matching function for
      call to 'print_btree'
   70 |   print_btree(btree, "btree");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:104:3: error: no matching function for
      call to 'print_btree'
  104 |   print_btree(btree, "btree");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:52:3: error: no template named
      'RedBlackTree'; did you mean 'Red_Black_Tree'?
   52 |   RedBlackTree<int> rbt;
      |   ^~~~~~~~~~~~
      |   Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:73:7: error: no member named
      'pre_order_traversal' in 'ads::trees::Red_Black_Tree<int>'
   73 |   rbt.pre_order_traversal([](const int& value) -> void { cout << value << " "; });
      |   ~~~ ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:77:7: error: no member named
      'post_order_traversal' in 'ads::trees::Red_Black_Tree<int>'
   77 |   rbt.post_order_traversal([](const int& value) -> void { cout << value << " "; });
      |   ~~~ ^
9 errors generated.
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:81:7: error: no member named
      'level_order_traversal' in 'ads::trees::Red_Black_Tree<int>'
   81 |   rbt.level_order_traversal([](const int& value) -> void { cout << value << " "; });
      |   ~~~ ^
gmake[2]: *** [CMakeFiles/test_bst.dir/build.make:79: CMakeFiles/test_bst.dir/src/main_Binary_Search_Tree.cc.o] Error 1
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:170:24: error: invalid range
      expression of type 'B_Tree<int, 3>'; no viable 'begin' function available
  170 |   for (const auto& val : btree) {
      |                        ^ ~~~~~
gmake[1]: *** [CMakeFiles/Makefile2:198: CMakeFiles/test_bst.dir/all] Error 2
gmake[1]: *** Waiting for unfinished jobs....
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:97:3: error: no template named
      'RedBlackTree'; did you mean 'Red_Black_Tree'?
   97 |   RedBlackTree<int> rbt_asc;
      |   ^~~~~~~~~~~~
      |   Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:225:3: error: no matching function for
      call to 'print_btree'
  225 |   print_btree(btree1, "btree1");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:231:3: error: no matching function for
      call to 'print_btree'
  231 |   print_btree(btree1, "btree1 (should be empty)");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:232:3: error: no matching function for
      call to 'print_btree'
  232 |   print_btree(btree2, "btree2 (should have the data)");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:240:3: error: no matching function for
      call to 'print_btree'
  240 |   print_btree(btree3, "btree3");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:245:3: error: no matching function for
      call to 'print_btree'
  245 |   print_btree(btree2, "btree2 (should be empty)");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:
106:21:    35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:246:3: error: no matching function for
      call to 'print_btree'
  246 |   print_btree(btree3, "btree3 (should have berror: treusee 2of'
s       undeclaredd aidentifiert a'rbt_desc';) "did )you; mean
       'rbt_asc'?|
  ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument   106does |   not  Rhavee dtheB lsamea ctypek Tasr ethee
<      correspondingi ntemplatet parameter> ('int' vs 'size_t' (aka 'unsigned long'))
   35 |  rbt_desc;
      |                     ^~~~~~~~
      |                     rbt_asc
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:97:21: note: 'rbt_asc'
      declared here
   97 |   RedBlackTree<int> rbt_asc;
      |                     ^
void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:106:3: error: use of
      undeclared identifier 'RedBlackTree'; did you mean 'Red_Black_Tree'?
  106 |   RedBlackTree<int> rbt_desc;
      |   ^~~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:109:5: error: use of
      undeclared identifier 'rbt_desc'; did you mean 'rbt_asc'?
  109 |     rbt_desc.insert(i);
      |     ^~~~~~~~
      |     rbt_asc
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:97:21: note: 'rbt_asc'
      declared here
   97 |   RedBlackTree<int> rbt_asc;
      |                     ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:111:25: error: use of
      undeclared identifier 'rbt_desc'; did you mean 'rbt_asc'?
  111 |   cout << "  Size: " << rbt_desc.size() << ", Height: " << rbt_desc.height() << "\n";
      |                         ^~~~~~~~
      |                         rbt_asc
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:97:21: note: 'rbt_asc'
      declared here
   97 |   RedBlackTree<int> rbt_asc;
      |                     ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:111:60: error: use of
      undeclared identifier 'rbt_desc'; did you mean 'rbt_asc'?
  111 |   cout << "  Size: " << rbt_desc.size() << ", Height: " << rbt_desc.height() << "\n";
      |                                                            ^~~~~~~~
      |                                                            rbt_asc
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:97:21: note: 'rbt_asc'
      declared here
   97 |   RedBlackTree<int> rbt_asc;
      |                     ^
[ 81%] Linking CXX executable bin/test_stacks_queues
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:327:3: error: no matching function for
      call to 'print_btree'
  327 |   print_btree(btree, "btree");
      |   ^~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_B_Tree.cc:35:6: note: candidate template
      ignored: substitution failure: deduced non-type template argument does not have the same type as the
      corresponding template parameter ('int' vs 'size_t' (aka 'unsigned long'))
   35 | void print_btree(const B_Tree<int, T>& tree, const string& name) {
      |      ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:120:3: error: no template
      named 'RedBlackTree'; did you mean 'Red_Black_Tree'?
  120 |   RedBlackTree<int> rbt;
      |   ^~~~~~~~~~~~
      |   Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:136:38: error: 'find_min' is a
      private member of 'ads::trees::Red_Black_Tree<int>'
  136 |   cout << "\nMinimum value: " << rbt.find_min() << '\n';
      |                                      ^
/Volumes/LCS.Data/AlgoDataStruct/src/../include/ads/trees/Red_Black_Tree.hpp:277:15: note:
      declared private here
  277 |   static auto find_min(Node* node) -> Node* {
      |               ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:136:47: error: too few
      arguments to function call, single argument 'node' was not specified
  136 |   cout << "\nMinimum value: " << rbt.find_min() << '\n';
      |                                  ~~~~~~~~~~~~ ^
/Volumes/LCS.Data/AlgoDataStruct/src/../include/ads/trees/Red_Black_Tree.hpp:277:15: note:
      'find_min' declared here
  277 |   static auto find_min(Node* node) -> Node* {
      |               ^        ~~~~~~~~~~
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:137:36: error: no member named
      'find_max' in 'ads::trees::Red_Black_Tree<int>'
  137 |   cout << "Maximum value: " << rbt.find_max() << '\n';
      |                                ~~~ ^
[ 81%] Built target test_singly_list
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:144:3: error: no template
      named 'RedBlackTree'; did you mean 'Red_Black_Tree'?
  144 |   RedBlackTree<int> rbt;
      |   ^~~~~~~~~~~~
      |   Red_Black_Tree
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:31:19: note: 'Red_Black_Tree'
      declared here
   31 | using ads::trees::RedBlackTree;
      |                   ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:155:7: error: no member named
      'remove' in 'ads::trees::Red_Black_Tree<int>'
  155 |   rbt.remove(10);
      |   ~~~ ^
/Volumes/LCS.Data/AlgoDataStruct/src/main_Red_Black_Tree.cc:160:7: error: no member named
      'remove' in 'ads::trees::Red_Black_Tree<int>'
  160 |   rbt.remove(20);
      |   ~~~ ^
fatal error: too many errors emitted, stopping now [-ferror-limit=]
20 errors generated.
gmake[2]: *** [CMakeFiles/test_rbt.dir/build.make:79: CMakeFiles/test_rbt.dir/src/main_Red_Black_Tree.cc.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:518: CMakeFiles/test_rbt.dir/all] Error 2
[ 81%] Built target test_heaps
[ 81%] Built target test_lists
[ 84%] Linking CXX executable bin/test_priority_queue
[ 84%] Built target test_stacks_queues
10 errors generated.
gmake[2]: *** [CMakeFiles/test_bbt.dir/build.make:79: CMakeFiles/test_bbt.dir/src/main_B_Tree.cc.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:166: CMakeFiles/test_bbt.dir/all] Error 2
[ 84%] Built target test_priority_queue
gmake: *** [Makefile:91: all] Error 2
