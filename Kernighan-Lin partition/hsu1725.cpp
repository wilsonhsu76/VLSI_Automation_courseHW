#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

struct gain_swap{
	int i;
	int j;
	int gain_value;  //the gain by swapping (i,j)
};

void Read_info(string fileName, vector <vector<bool>> &vec);
void Init_groups(vector<int> &group_a, vector<int> &group_b, int NodeCount);
void Copy_groups(vector<int> &ref_group, vector<int> &group);
void Init_swap(vector<bool> &swapdone_vec, int NodeCount);

void calc_g_per_loop(vector <int> &tmp_groupA, vector <int> &tmp_groupB, vector <vector<bool>> &vec, vector <gain_swap> &tmp_gain_vec, vector <bool> &swapdone_vec);
int compare_g(vector <gain_swap> &tmp_gain_vec);  //return max gain index
void swap_elements_in_groups(vector<int> &group_a, vector<int> &group_b, int nodeA, int nodeB, vector <bool> &swapdone_vec);
int calc_G_global(vector <gain_swap> &swapRecord_vec);
void print_group(vector<int> &group);
void print_edges_on_the_cut(vector<int> &groupA, vector<int> &groupB, vector <vector<bool>> &vec);


int main(int argc, char* argv[]){
	//load info from file and record it to adj_matrix
	string fileName = argv[1];
	vector <vector<bool>> adj_matrix;
	Read_info(fileName, adj_matrix);
	int NodeCount = adj_matrix.size() - 1;
	if (NodeCount <= 0) {
		cout << "read file fail..." << endl;
		cout << "enter anything to terminate";
		cin.get();
		return -1;
	}

	vector <int> groupA; //groupA commit version
	vector <int> groupB; //groupB commit version
	Init_groups(groupA, groupB, NodeCount);

	vector <int> tmp_groupA; //groupA testing version
	vector <int> tmp_groupB; //groupB testing version

	vector <gain_swap> tmp_gain_vec; //record gain/swap pair to compare in each step
	vector <bool> swapdone_vec; //record which nodes swap done
	vector <gain_swap> swapRecord_vec; //record swap record with gain in each step

	while (1)
	{
		swapRecord_vec.clear();
		swapdone_vec.clear();
		Copy_groups(groupA, tmp_groupA); //groupA==>tmp_groupA
		Copy_groups(groupB, tmp_groupB); //groupB==>tmp_groupB
		Init_swap(swapdone_vec, NodeCount); //set all elements are false (not swap yet)

		//total run NodeCount / 2 times
		for (int i = 1; i <= NodeCount / 2; i++) {
			tmp_gain_vec.clear();
			calc_g_per_loop(tmp_groupA, tmp_groupB, adj_matrix, tmp_gain_vec, swapdone_vec);
			int gs_index = compare_g(tmp_gain_vec);
			int swap_A = tmp_gain_vec[gs_index].i;
			int swap_B = tmp_gain_vec[gs_index].j;
			int gain_value = tmp_gain_vec[gs_index].gain_value;
			gain_swap gsObj = { swap_A, swap_B, gain_value };
			swapRecord_vec.push_back(gsObj); //global
			swap_elements_in_groups(tmp_groupA, tmp_groupB, swap_A, swap_B, swapdone_vec);
		}
		int m = calc_G_global(swapRecord_vec); //return how many steps do the best profit
		//do index from 0 to m can bring maximum gain
		if (m >= 0) {
			for (int j = 0; j <= m; j++) {
				int nodeA = swapRecord_vec[j].i;
				int nodeB = swapRecord_vec[j].j;
				swap_elements_in_groups(groupA, groupB, nodeA, nodeB, swapdone_vec);
			}
		}
		else {
			break;  //termination condition for while(1)
		}
	}
	cout << "group1:" << endl;
	print_group(groupA);
	cout << "group2:" << endl;
	print_group(groupB);
	print_edges_on_the_cut(groupA, groupB, adj_matrix);
	cout << "enter anything to terminate";
	cin.get();
	return 0;
}

void Read_info(string fileName, vector <vector<bool>> &vec) {
	fstream fp(fileName, ios::in);
	if (fp.fail())
		return;  //read file fail...
	string firstLine;
	int numNode, edgeNode = 0;  //
	getline(fp, firstLine);
	stringstream ssin(firstLine);
	ssin >> numNode >> edgeNode;
	vec.resize(numNode + 1); //0,1~numNode
	//+1 for mappinf vec index(start from 1) to input index(start from 1)
	for (int i = 0; i < (numNode + 1); i++) {
		vec[i].resize(numNode + 1);
		for (int j = 0; j < (numNode + 1); j++)
			vec[i][j] = false;
	}

	string tmpLine;
	int a, b = 0;
	while (getline(fp, tmpLine)) {
		stringstream ssin2(tmpLine);
		ssin2 >> a >> b;
		if ((a < (numNode+1)) && (b < (numNode + 1))) {
			if (vec[a][b] == false)
				vec[a][b] = true;
			if (vec[b][a] == false)
				vec[b][a] = true;
		}
	}
	fp.close();
}

void Init_groups(vector<int> &group_a, vector<int> &group_b, int NodeCount) {
	int TH = NodeCount / 2;
	for (int i = 1; i <= NodeCount; i++) {
		if (i <= TH)
			group_a.push_back(i);
		else
			group_b.push_back(i);
	}
}

void Copy_groups(vector<int> &ref_group, vector<int> &group) {
	group.clear();
	int sz = ref_group.size();
	group.resize(sz);
	for (int i = 0; i < sz; i++) {
		group[i] = ref_group[i];
	}
}

void Init_swap(vector<bool> &swapdone_vec, int NodeCount) {
	swapdone_vec.resize(NodeCount + 1);
	for (int i = 0; i < NodeCount + 1; i++) {
		swapdone_vec[i] = false;
	}
}

void calc_g_per_loop(vector <int> &tmp_groupA, vector <int> &tmp_groupB, vector <vector<bool>> &vec, vector <gain_swap> &tmp_gain_vec, vector <bool> &swapdone_vec){
	int group_sz = tmp_groupA.size();
	int adj_matrix_sz = vec.size();
	vector <int> D_table; //for fast algo
	vector <bool> D_table_b;
	D_table.resize(adj_matrix_sz);
	D_table_b.resize(adj_matrix_sz);
	for (int z = 0; z < adj_matrix_sz; z++)
		D_table_b[z] = false;

	for (int i = 0; i < group_sz; i++) {
		int nodeNum_A = tmp_groupA[i];
		if (swapdone_vec[nodeNum_A] == true)
			continue; //this node is fixed
		//match group B
		for (int j = 0; j < group_sz; j++) {
			int nodeNum_B = tmp_groupB[j];
			if (swapdone_vec[nodeNum_B] == true)
				continue; //this node is fixed
			////////////////////////////////////////
			//calc g(nodeNum_A, nodeNum_B):
			int D_A = 0; //calc D_A
			if (D_table_b[nodeNum_A] == false) {
				for (int x = 0; x < group_sz; x++) {
					int nodeB = tmp_groupB[x];
					if (vec[nodeNum_A][nodeB] == true) {
						D_A++; //other group++
					}
				}
				for (int x = 0; x < group_sz; x++) {
					int nodeA = tmp_groupA[x];
					if (nodeA == nodeNum_A)
						continue;
					if (vec[nodeNum_A][nodeA] == true) {
						D_A--; //same group
					}
				}
				D_table_b[nodeNum_A] = true;
				D_table[nodeNum_A] = D_A;
			}
			else
				D_A = D_table[nodeNum_A];

			////////////////////////////////////////
			int D_B = 0; //calc D_B
			if (D_table_b[nodeNum_B] == false) {
				for (int x = 0; x < group_sz; x++) {
					int nodeA = tmp_groupA[x];
					if (vec[nodeNum_B][nodeA] == true) {
						D_B++; //other group++
					}
				}
				for (int x = 0; x < group_sz; x++) {
					int nodeB = tmp_groupB[x];
					if (nodeB == nodeNum_B)
						continue;
					if (vec[nodeNum_B][nodeB] == true) {
						D_B--; //same group
					}
				}
				D_table_b[nodeNum_B] = true;
				D_table[nodeNum_B] = D_B;
			}
			else
				D_B = D_table[nodeNum_B];

			////////////////////////////////////////
			gain_swap gs_obj;
			gs_obj.i = nodeNum_A;
			gs_obj.j = nodeNum_B;
			if (vec[nodeNum_A][nodeNum_B] == true)
				gs_obj.gain_value = D_A + D_B - 2;
			else
				gs_obj.gain_value = D_A + D_B;
			////////////////////////////////////////
			tmp_gain_vec.push_back(gs_obj);  //record this gain_swap
		}
	}
}

int compare_g(vector <gain_swap> &tmp_gain_vec) {
	int sz = tmp_gain_vec.size();
	int max_index = 0;
	int max_gain = INT32_MIN;
	for (int i = 0; i < sz; i++) {
		if (tmp_gain_vec[i].gain_value > max_gain) {
			max_gain = tmp_gain_vec[i].gain_value;
			max_index = i;
		}
	}
	return max_index;
}

void swap_elements_in_groups(vector<int> &group_a, vector<int> &group_b, int nodeA, int nodeB, vector <bool> &swapdone_vec) {
	int group_sz = group_a.size();
	for (int i = 0; i < group_sz; i++) {
		if (group_a[i] == nodeA)
			group_a[i] = nodeB;
	}
	for (int i = 0; i < group_sz; i++) {
		if (group_b[i] == nodeB)
			group_b[i] = nodeA;
	}
	swapdone_vec[nodeA] = true; //fix this node
	swapdone_vec[nodeB] = true; //fix this node
}

int calc_G_global(vector <gain_swap> &swapRecord_vec) {
	int steps = -1; //only report positive Gm
	int total_gain = INT32_MIN;  //taking return index's gain
	int acc_gain = 0; //from 1st to last step
	int sz = swapRecord_vec.size();
	
	for (int i = 0; i < sz; i++) {
		acc_gain += swapRecord_vec[i].gain_value;
		if (acc_gain > total_gain) {
			steps = i;
			total_gain = acc_gain;
		}
	}
	if (total_gain > 0) {
		return steps;
	}
	else
		return -1;
}

void print_group(vector<int> &group) {
	int sz = group.size();
	cout << "{ ";
	for (int i = 0; i < sz; i++)
		cout << group[i] << " ";
	cout << "}" << endl;
}

void print_edges_on_the_cut(vector<int> &groupA, vector<int> &groupB, vector <vector<bool>> &vec) {
	int sz = groupA.size();
	cout << endl<< "edges on the cut:" << endl;
	for (int i = 0; i < sz; i++) {
		int NodeA = groupA[i];
		for (int j = 0; j < sz; j++) {
			int NodeB = groupB[j];
			if (vec[NodeA][NodeB])
				cout << "(" << NodeA << " , " << NodeB << ") ";
		}
	}
	cout << endl;
}
