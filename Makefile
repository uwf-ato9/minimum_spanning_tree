graph.dot.svg: graph.dot
	dot -Tsvg -O graph.dot

graph.dot: minimum_spanning_tree
	./minimum_spanning_tree > graph.dot

minimum_spanning_tree: minimum_spanning_tree.cpp
	c++ -g -o minimum_spanning_tree minimum_spanning_tree.cpp
