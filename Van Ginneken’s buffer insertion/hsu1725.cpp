#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define BUF_C 1
#define BUF_R 1
#define BUF_D 1
#define WIRE_UC 1
#define WIRE_UR 1
#define DRIVER_R 1
#define DRIVER_C 1
// buffer cap, resistance, instrinc delay
// wire unit cap and unit resistance

struct edgeId {
	int source;
	int dest;
};

struct solNode {
	double rat; //q
	double cap; //c
	vector<edgeId> buf_pos; //to record buffer position (2n) for possible positions
	solNode* next;
};

//node unit for every node index in the "Circuit" input
struct CNode {
	int childL_Id;
	int childR_Id;
	int parent_Id;
	int edge;  // the length of the edge to its parent 
	int rat;  //only shows in sink nodes
	int cap;  //only shows in sink nodes
	solNode* solList;
};

int Read_info(string fileName, vector <CNode> &CNodes, vector<bool> &finishFlag_CNodes);
void Access_sink(CNode& n, int sink_num, int self_id);
void Insert_solNode(solNode* &List, solNode* &n);
void Prune_inferior(solNode* &List);
void Access_nonSink_oneChild(CNode& n, int sink_num, CNode& n_c, int self_id);
void Access_nonSink_twoChildren(CNode& n, int sink_num, CNode& n_c1, CNode& n_c2, int self_id);
void Access_driver_oneChild(CNode& n, int sink_num, CNode& n_c, int self_id);
void Access_driver_twoChildren(CNode& n, int sink_num, CNode& n_c1, CNode& n_c2, int self_id);
void PrintMsg(CNode& n, int sink_num);

/*int main(int argc, char* argv[]) {
	//test internal link functions
	solNode* s1 = new solNode; s1->cap = 1; s1->rat = 5;
	solNode* s2 = new solNode; s2->cap = 1; s2->rat = 6;
	solNode* s3 = new solNode; s3->cap = 2; s3->rat = 3;
	solNode* s4 = new solNode; s4->cap = 2; s4->rat = 5;
	solNode* s5 = new solNode; s5->cap = 100; s5->rat = 7;
	solNode* s6 = new solNode; s6->cap = 3; s6->rat = 5;
	solNode* s7 = new solNode; s7->cap = 3; s7->rat = 7;
	solNode* solList = NULL;
	Insert_solNode(solList, s3);
	Insert_solNode(solList, s4);
	Insert_solNode(solList, s5);
	Insert_solNode(solList, s1);
	Insert_solNode(solList, s7);
	Insert_solNode(solList, s2);
	Insert_solNode(solList, s6);
	Prune_inferior(solList);
	return 1;
}*/

int main(int argc, char* argv[]) {
	string fileName = argv[1];
	vector <CNode> CNodes;  // CNodes[2n]
	vector <bool> finishFlag_CNodes; // Flags[2n]
	int sink_num = Read_info(fileName, CNodes, finishFlag_CNodes);  //read data from input file
	int NodeCount = finishFlag_CNodes.size();
	if (NodeCount <= 0) {
		cout << "read file fail..." << endl;
		cout << "enter anything to terminate";
		cin.get();
		return -1;
	}
	//access every sink node first
	for (int i = 1; i <= sink_num; i++) {
		Access_sink(CNodes[i], sink_num, i);
		finishFlag_CNodes[i] = true;
	}

	//access middle node until CNode[0]'s child is ready
	while (finishFlag_CNodes[0] != true) {
		//check Steiner points 
		for (int i = (sink_num + 1); i < (2 * sink_num); i++) {
			int child_num = 0;
			if (finishFlag_CNodes[i] != true) {
				if (CNodes[i].childL_Id == -1)
					continue; //this node is not interminate point, next index
				else {
					if (CNodes[i].childR_Id == -1) {
						child_num = 1;
						if (finishFlag_CNodes[CNodes[i].childL_Id] != true)
							continue;  //not ready, next index
					}
					else { //check two child
						child_num = 2;
						if (finishFlag_CNodes[CNodes[i].childL_Id] != true)
							continue;  //not ready, next index
						if (finishFlag_CNodes[CNodes[i].childR_Id] != true)
							continue;  //not ready, next index
					}
				}
				//access this index becasue all children nodes are ready
				if (child_num == 1) {  //Line_Access
					int child_id = CNodes[i].childL_Id;
					Access_nonSink_oneChild(CNodes[i], sink_num, CNodes[child_id], i);
					finishFlag_CNodes[i] = true;
				}
				else {  //merge case
					int child_id1 = CNodes[i].childL_Id;
					int child_id2 = CNodes[i].childR_Id;
					Access_nonSink_twoChildren(CNodes[i], sink_num, CNodes[child_id1], CNodes[child_id2], i);
					finishFlag_CNodes[i] = true;
				}
			}
		}

		//check source point
		if (finishFlag_CNodes[0] != true) {
			int child_num;
			if (CNodes[0].childL_Id == -1)
				continue; //this node is not interminate point, next index
			else {
				if (CNodes[0].childR_Id == -1) {
					child_num = 1;
					if (finishFlag_CNodes[CNodes[0].childL_Id] != true)
						continue;  //not ready, next loop
				}
				else { //check two child
					child_num = 2;
					if (finishFlag_CNodes[CNodes[0].childL_Id] != true)
						continue;  //not ready, next loop
					if (finishFlag_CNodes[CNodes[0].childR_Id] != true)
						continue;  //not ready, next loop
				}
			}
			//access this index becasue all children nodes are ready
			if (child_num == 1) {  //Line_Access
				int child_id = CNodes[0].childL_Id;
				Access_driver_oneChild(CNodes[0], sink_num, CNodes[child_id], 0);
				finishFlag_CNodes[0] = true;
			}
			else {  //merge case
				int child_id1 = CNodes[0].childL_Id;
				int child_id2 = CNodes[0].childR_Id;
				Access_driver_twoChildren(CNodes[0], sink_num, CNodes[child_id1], CNodes[child_id2], 0);
				finishFlag_CNodes[0] = true;
			}
		}
	}

	//output
	PrintMsg(CNodes[0], sink_num);
	cout << "[Finish] enter anything to terminate";
	cin.get();
	return 0;
}


void PrintMsg(CNode& n,int sink_num) {
	solNode* sn = n.solList;
	double rat_source = 0.0;
	rat_source = sn->rat;
	int buf_cnt = 0;
	buf_cnt = n.solList->buf_pos.size();
	cout << "Total number of buffers inserted:" << buf_cnt << endl;
	cout << "Each buffer is at the upstream node(i) of each following edge(i,j)" << endl;
	for (int i = 0; i < buf_cnt; i++) {
		cout << "edge( " << n.solList->buf_pos[i].source << ", " << n.solList->buf_pos[i].dest <<" )" << endl;
	}
	cout << "The Required Arrived Time at the driver node: " << rat_source << endl;
}

// n is n_c's parent
void Access_driver_oneChild(CNode& n, int sink_num, CNode& n_c, int self_id) {
	solNode* curr = n_c.solList;
	//got new solutions from every solution in child node 
	while (curr != NULL) {
		//sol 1 with wire without buf
		solNode* sn0 = new solNode();
		int buf_count = curr->buf_pos.size();
		for (int i = 0; i <buf_count; i++) {
			sn0->buf_pos.push_back(curr->buf_pos[i]);
		}
		sn0->cap = DRIVER_C; //assume 1
		sn0->rat = (curr->rat) - DRIVER_R*(curr->cap);

		Insert_solNode(n.solList, sn0);
		curr = curr->next;  //next solution node in child
	}
	Prune_inferior(n.solList); //remove inferior nodes
	return;
}

void Access_driver_twoChildren(CNode& n, int sink_num, CNode& n_c1, CNode& n_c2, int self_id) {
	//merge 2 solutions from 2 children into one solution list, 
	//and the following thing is the same as one child case
	//merge part:
	solNode* tmpSolList = NULL; //save merged result first
	solNode* curr_L = n_c1.solList;
	solNode* curr_R = n_c2.solList;
	bool flag_L_end = false;
	bool flag_R_end = false;
	while (flag_L_end == false || flag_R_end == false) {
		solNode* sn_m = new solNode();
		if (curr_L->rat <= curr_R->rat) {
			sn_m->rat = curr_L->rat;
			sn_m->cap = curr_L->cap + curr_R->cap;
			int buf_count = curr_L->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_L->buf_pos[i]);
			}
			buf_count = curr_R->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_R->buf_pos[i]);
			}
			if (curr_L->next != NULL)
				curr_L = curr_L->next;
			else {
				flag_L_end = true; //curr_L is at the last element of L_list
				if (curr_R->next != NULL)
					curr_R = curr_R->next;
				else
					flag_R_end = true;
			}
		}
		else {
			sn_m->rat = curr_R->rat;
			sn_m->cap = curr_L->cap + curr_R->cap;
			int buf_count = curr_L->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_L->buf_pos[i]);
			}
			buf_count = curr_R->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_R->buf_pos[i]);
			}
			if (curr_R->next != NULL)
				curr_R = curr_R->next;
			else {
				flag_R_end = true; //curr_R is at the last element of R_list
				if (curr_L->next != NULL)
					curr_L = curr_L->next;
				else
					flag_L_end = true;
			}
		}
		Insert_solNode(tmpSolList, sn_m);
	}
	//end merge part

	//access solutions in tmpSolList
	solNode* curr = tmpSolList;
	//got new solutions from every solution in tmpSolList 
	while (curr != NULL) {
		//sol 1 with wire without buf
		solNode* sn0 = new solNode();
		int buf_count = curr->buf_pos.size();
		for (int i = 0; i < buf_count; i++) {
			sn0->buf_pos.push_back(curr->buf_pos[i]);
		}
		sn0->cap = DRIVER_C; //assume 1
		sn0->rat = (curr->rat) - DRIVER_R*(curr->cap);
		Insert_solNode(n.solList, sn0);
		curr = curr->next;  //next solution node in child
	}
	Prune_inferior(n.solList); //remove inferior nodes
	return;
}


// n is n_c's parent
void Access_nonSink_oneChild(CNode& n, int sink_num, CNode& n_c, int self_id) {
	solNode* curr = n_c.solList;
	//got new solutions from every solution in child node 
	while (curr != NULL) {
		//sol 1 with wire without buf
		solNode* sn0 = new solNode();
		int buf_count = curr->buf_pos.size();
		for (int i = 0; i < buf_count; i++) {
			sn0->buf_pos.push_back(curr->buf_pos[i]);
		}
		sn0->cap = curr->cap + WIRE_UC*(n.edge);
		sn0->rat = (curr->rat) - (WIRE_UC*WIRE_UR*(n.edge)*(n.edge) / 2.0) - WIRE_UR*(n.edge)*(curr->cap);

		//sol 2 with wire with buf
		solNode* sn1 = new solNode();
		edgeId e;
		e.source = n.parent_Id;
		e.dest = self_id;
		sn1->buf_pos.push_back(e);
		int buf_count2 = curr->buf_pos.size();
		for (int i = 0; i < buf_count2; i++) {
			sn1->buf_pos.push_back(curr->buf_pos[i]);
		}

		sn1->cap = BUF_C;
		sn1->rat = sn0->rat - BUF_R*(sn0->cap) - BUF_D;
		Insert_solNode(n.solList, sn0);
		Insert_solNode(n.solList, sn1);
		curr = curr->next;  //next solution node in child
	}
	Prune_inferior(n.solList); //remove inferior nodes
	return;
}

void Access_nonSink_twoChildren(CNode& n, int sink_num, CNode& n_c1, CNode& n_c2, int self_id) {
	//merge 2 solutions from 2 children into one solution list, 
	//and the following thing is the same as one child case
	//merge part:
	solNode* tmpSolList = NULL; //save merged result first
	solNode* curr_L = n_c1.solList;
	solNode* curr_R = n_c2.solList;
	bool flag_L_end = false;
	bool flag_R_end = false;
	while (flag_L_end == false || flag_R_end == false) {
		solNode* sn_m = new solNode();
		if (curr_L->rat <= curr_R->rat) {
			sn_m->rat = curr_L->rat;
			sn_m->cap = curr_L->cap + curr_R->cap;
			int buf_count = curr_L->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_L->buf_pos[i]);
			}
			buf_count = curr_R->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_R->buf_pos[i]);
			}
			if (curr_L->next != NULL)
				curr_L = curr_L->next;
			else {
				flag_L_end = true; //curr_L is at the last element of L_list
				if (curr_R->next != NULL)
					curr_R = curr_R->next;
				else
					flag_R_end = true;
			}
		}
		else {
			sn_m->rat = curr_R->rat;
			sn_m->cap = curr_L->cap + curr_R->cap;
			int buf_count = curr_L->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_L->buf_pos[i]);
			}
			buf_count = curr_R->buf_pos.size();
			for (int i = 0; i < buf_count; i++) {
				sn_m->buf_pos.push_back(curr_R->buf_pos[i]);
			}
			if (curr_R->next != NULL)
				curr_R = curr_R->next;
			else {
				flag_R_end = true; //curr_R is at the last element of R_list
				if (curr_L->next != NULL)
					curr_L = curr_L->next;
				else
					flag_L_end = true;
			}
		}
		Insert_solNode(tmpSolList, sn_m);
	}
	//end merge part

	//access solutions in tmpSolList
	solNode* curr = tmpSolList;
	//got new solutions from every solution in tmpSolList 
	while (curr != NULL) {
		//sol 1 with wire without buf
		solNode* sn0 = new solNode();
		int buf_count = curr->buf_pos.size();
		for (int i = 0; i < buf_count; i++) {
			sn0->buf_pos.push_back(curr->buf_pos[i]);
		}
		sn0->cap = curr->cap + WIRE_UC*(n.edge);
		sn0->rat = (curr->rat) - (WIRE_UC*WIRE_UR*(n.edge)*(n.edge) / 2.0) - WIRE_UR*(n.edge)*(curr->cap);

		//sol 2 with wire with buf
		solNode* sn1 = new solNode();
		edgeId e;
		e.source = n.parent_Id;
		e.dest = self_id;
		sn1->buf_pos.push_back(e);
		int buf_count2 = curr->buf_pos.size();
		for (int i = 0; i < buf_count2; i++) {
			sn1->buf_pos.push_back(curr->buf_pos[i]);
		}
		sn1->cap = BUF_C;
		sn1->rat = sn0->rat - BUF_R*(sn0->cap) - BUF_D;
		Insert_solNode(n.solList, sn0);
		Insert_solNode(n.solList, sn1);
		curr = curr->next;  //next solution node in child
	}
	Prune_inferior(n.solList); //remove inferior nodes
	return;
}

void Access_sink(CNode& n , int sink_num, int self_id) {
	//sol 1 with wire without buf
	solNode* sn0 = new solNode();
	sn0->cap = n.cap + WIRE_UC*n.edge;
	sn0->rat = n.rat - (WIRE_UC*WIRE_UR*(n.edge)*(n.edge) / 2.0) - WIRE_UR*(n.edge)*(n.cap);
	
	//sol 2 with wire with buf
	solNode* sn1 = new solNode();
	edgeId e;
	e.source = n.parent_Id;
	e.dest = self_id;
	sn1->buf_pos.push_back(e);
	sn1->cap = BUF_C;
	sn1->rat = sn0->rat - BUF_R*(sn0->cap) - BUF_D;
	Insert_solNode(n.solList, sn0);
	Insert_solNode(n.solList, sn1);
	Prune_inferior(n.solList);
	return;
}

//order by increasing cap value
void Insert_solNode(solNode* &List, solNode* &n) {
	if (List == NULL) {
		List = n;
		n->next = NULL;
	}
	else {
		solNode* curr = List;
		//smaller to anyone in the list, insert as the head
		if (n->cap < curr->cap) {
			List = n;
			n->next = curr;
		}
		else {
			bool inFlag = false;
			while (curr->next != NULL) {
				if (n->cap < curr->next->cap) {
					n->next = curr->next; //insert in the middle
					curr->next = n;
					inFlag = true;
					break;
				}
				curr = curr->next;
			}
			
			if (curr->next == NULL && !inFlag) {
				curr->next = n; //insert in the end
				inFlag = true;
				n->next = NULL;
			}
		}
	}
	return;
}

void Prune_inferior(solNode* &List) {
	if (List == NULL)
		return; //no element... return
	solNode* pre = List;
	solNode* curr = pre->next;
	if (curr == NULL)
		return; //only one element... return

	while (curr != NULL) {
		if (pre->cap == curr->cap) { //equal cap case
			if (pre->rat < curr->rat) {
				//prune not first
				if (pre != List) {
					solNode* probe = List;
					while (List->next != pre) {
						probe = probe->next;
					}
					probe->next = curr;
					pre = curr;
					curr = curr->next;
				}
				else {
					List = curr;
					pre = curr;
					curr = curr->next;
				}
			}
			else {  //pre->rat >= curr->rat delete later term
				    //if equal, delate later term...
				pre->next = curr->next;
				curr = curr->next;
			}
		}
		else {
			if (pre->rat >= curr->rat) { //prune 'curr' case
				pre->next = curr->next;
				curr = curr->next;
			}
			else {  //no-prune, compare next
				pre = curr;
				curr = curr->next;
			}
		}
	}
	return;
}

int Read_info(string fileName, vector <CNode> &CNodes, vector<bool> &finishFlag_CNodes) {
	fstream fp(fileName, ios::in);
	if (fp.fail())
		return 0;  //read file fail...

	//read how many sink and init CNodes and finishFlag_CNodes
	string tmpLine;
	int sink_Num;
	getline(fp, tmpLine); //#Line 1: row  col in grid
	stringstream ssin(tmpLine);
	ssin >> sink_Num; //n
	CNodes.resize(2 * sink_Num); // 0 + n + n-1: index from 0~2n-1
	finishFlag_CNodes.resize(2 * sink_Num);
	//init
	for (int i = 0; i < (2 * sink_Num); i++) {
		CNodes[i].childL_Id = -1;
		CNodes[i].childR_Id = -1;
		CNodes[i].edge = -1;
		CNodes[i].parent_Id = -1;
		CNodes[i].solList = NULL;
		CNodes[i].rat = -1;
		CNodes[i].cap = -1;
		finishFlag_CNodes[i] = false;
	}

	//sink info
	//read n lines for sink: line 2 to line n+1
	for (int i = 0; i < sink_Num; i++) {
		int sink_cap, sink_rat;
		string tmpLine2;
		getline(fp, tmpLine2);
		stringstream ssin2(tmpLine2);
		ssin2 >> sink_cap >> sink_rat;
		CNodes[i + 1].cap = sink_cap;
		CNodes[i + 1].rat = sink_rat;
	}

	string tmpLine3;
	int parent_id, child_id, edge;
	while (getline(fp, tmpLine3)) {
		stringstream ssin3(tmpLine3);
		ssin3 >> parent_id >> child_id >> edge;

		if (CNodes[parent_id].childL_Id >= 0) {
			CNodes[parent_id].childR_Id = child_id;
		}
		else {
			CNodes[parent_id].childL_Id = child_id;
		}
		CNodes[child_id].parent_Id = parent_id;
		CNodes[child_id].edge = edge;
	}
	fp.close();
	return sink_Num;
}

