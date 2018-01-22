#include <vector>
#include <queue>
#include <iostream>
#include <limits.h>
using namespace std;
#define V_COUNT 9
#define START_INDEX 6
//0 is dummy, V={1,2,...,8}
struct nodeV {
	int lable; //path weight
	int index; //this is which node (index)
	int predecessor;
	bool inQueue;
};

struct nodeV_compare {
	bool operator()(const nodeV & a, const nodeV & b)
	{
		return a.lable > b.lable;
	}
};

void fill_graph_into_adj_matrix(vector < vector<int> > &matrix) {
	matrix[6][1] = 7; matrix[1][6] = 7;
	matrix[6][3] = 3; matrix[3][6] = 3;
	matrix[1][3] = 4; matrix[3][1] = 4;
	matrix[1][2] = 3; matrix[2][1] = 3;
	matrix[1][4] = 7; matrix[4][1] = 7;
	matrix[1][7] = 8; matrix[7][1] = 8;
	matrix[2][3] = 4; matrix[3][2] = 4;
	matrix[2][4] = 1; matrix[4][2] = 1;
	matrix[2][5] = 8; matrix[5][2] = 8;
	matrix[2][7] = 3; matrix[7][2] = 3;
	matrix[3][4] = 6; matrix[4][3] = 6;
	matrix[4][5] = 2; matrix[5][4] = 2;
	matrix[5][7] = 4; matrix[7][5] = 4;
	matrix[5][8] = 6; matrix[8][5] = 6;
	matrix[4][7] = 5; matrix[7][4] = 5;
	matrix[7][8] = 9; matrix[8][7] = 9;
	return;
}

void init_node_set(vector<nodeV> &node_set) {
	for (int i = 0; i < V_COUNT; i++) {
		node_set[i].lable = INT_MAX;
		node_set[i].predecessor = -1; //-1: NIL
		node_set[i].index = i;
	}
	return;
}

//path from 6 to 8
void print_result(vector <nodeV> & node_set, vector <vector <int> > & adj_matrix) {
	vector<int> s;
	int predecessor_id = node_set[8].predecessor; //-1 means to end
	s.push_back(8);
	while (predecessor_id > 0) {
		s.push_back(predecessor_id);
		predecessor_id = node_set[predecessor_id].predecessor;
	}
	int total_length = 0;
	int s_path_size = s.size();
	while (s_path_size != 1) {
		int u = s[s_path_size-1];
		s.pop_back();
		int v = s[s_path_size - 2];
		int length = adj_matrix[u][v];
		total_length += length;
		cout << "from node " << u << " to node " << v << ",  edge lenght is:" << length << endl;
		s_path_size--;
	}
	cout << "one shortest path from node 6 to node 8, the length of this path: " << total_length << endl;
	return;
}

void Dijkstra_Algorithm() {
	//take input to adj_matrix 
	vector <vector <int> > adj_matrix;
	adj_matrix.resize( V_COUNT, vector<int>(V_COUNT, 0) );
	fill_graph_into_adj_matrix(adj_matrix);

	//init node Vs
	vector <nodeV> node_set(V_COUNT);
	init_node_set(node_set);
	
	std::priority_queue <nodeV, std::vector<nodeV>, nodeV_compare> Q;
	node_set[6].lable = 0;  //start from node 6
	Q.push(node_set[6]);
	node_set[6].inQueue = true;
	bool improve_flag = false;

	while (Q.size() != 0) {
		int u = Q.top().index;
		Q.pop();
		node_set[u].inQueue = false;
		improve_flag = false;
		for (int v = 0; v < V_COUNT; v++) {
			if (adj_matrix[u][v] != 0) { //(u,v) has edge
				if (node_set[v].lable > (node_set[u].lable + adj_matrix[u][v])) {
					node_set[v].lable = (node_set[u].lable + adj_matrix[u][v]);
					node_set[v].predecessor = u;
					improve_flag = true;

					if (!node_set[v].inQueue) {
						Q.push(node_set[v]);
						node_set[v].inQueue = true;
					}
				}
			}
		}

		if (!improve_flag)  //becasue there is loop in graph, need terminal setting
			break;

	}
	print_result(node_set, adj_matrix);
	return;
}

int main() {
	Dijkstra_Algorithm();  //find shortest path from node 6 to node 8
	cin.get();
	return 0;
}