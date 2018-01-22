#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
using namespace std;

struct xyPair{
	int x;  //point[x][y]
	int y;
};

struct MatrixNode {
	int x;
	int y;
	int cost;
	xyPair parent;
	bool inqueue;
};

struct nodeV_compare {
	bool operator()(const MatrixNode & a, const MatrixNode & b)
	{
		return a.cost > b.cost;
	}
};

void Read_info(string fileName, vector <vector<int>> &map_vec, vector <vector<MatrixNode>> &cost_vec, xyPair &Source, xyPair &Target);
void PrintPath(vector <vector<MatrixNode>> &cost_vec, xyPair &Source, xyPair &Target);


int main(int argc, char* argv[]){
	//load info from file and record it to adj_matrix
	string fileName = argv[1];
	vector <vector<int>> map_matrix;  // 0: zero Area, 1: normal Rd, 2: cant pass Area
	vector <vector<MatrixNode>> cost_matrix; // cost/parent for each point
	xyPair Source;
	xyPair Target;
	Read_info(fileName, map_matrix, cost_matrix, Source, Target);
	int NodeCount = map_matrix.size() - 1;
	if (NodeCount <= 0) {
		cout << "read file fail..." << endl;
		cout << "enter anything to terminate";
		cin.get();
		return -1;
	}
	int rowUpperBound = map_matrix.size() - 1;
	int colUpperBound = map_matrix[0].size() - 1;
	std::priority_queue <MatrixNode, std::vector<MatrixNode>, nodeV_compare> Q;
	Q.push(cost_matrix[Source.x][Source.y]);
	bool goal_flag = false;
	//bool improve_flag = false;

	//early termination (Source == Target):
	if ((Source.x == Target.x) && (Source.y == Target.y)) {
		cout << "Cost is 0, becasue source point is also the target point" << endl;
		cout << "enter anything to terminate";
		cin.get();
		return 0;
	}

	//early termination (Because Source or Target in the blockage):
	if ((map_matrix[Source.x][Source.y] == 2) || (map_matrix[Target.x][Target.y] == 2)) {
		cout << "There is no path from source to target" << endl;
		cout << "enter anything to terminate";
		cin.get();
		return 0;
	}

	while (Q.size() != 0) {
		MatrixNode u = Q.top();
		Q.pop();
		cost_matrix[u.x][u.y].inqueue = false;
		u.inqueue = false;
		//improve_flag = false;
		//move: up/left/right/down
		//left move
		if ((u.x - 1) >= 1) {
			MatrixNode v = cost_matrix[u.x - 1][u.y];
			if (map_matrix[(u.x - 1)][u.y] != 2) {  //left possible
				int move_cost = 0;
				if (map_matrix[u.x][u.y] == 0 && map_matrix[(u.x - 1)][u.y] == 0)
					move_cost = 0;
				else
					move_cost = 1;

				if (v.cost > (u.cost + move_cost))
				{
					v.cost = (u.cost + move_cost);
					v.parent.x = u.x;
					v.parent.y = u.y;
					cost_matrix[u.x - 1][u.y].cost = (u.cost + move_cost);
					cost_matrix[u.x - 1][u.y].parent.x = u.x;
					cost_matrix[u.x - 1][u.y].parent.y = u.y;
					if (cost_matrix[u.x - 1][u.y].inqueue == false) {
						Q.push(v);
						v.inqueue = true;
						cost_matrix[u.x - 1][u.y].inqueue = true;
					}
					else { //update MatrixNode already in queue
						vector<MatrixNode> backup_vec;
						while (Q.size() != 0) {
							MatrixNode n = Q.top();
							Q.pop();
							if (n.x == v.x && n.y == v.y)
							{
								Q.push(v);
								break;
							}
							backup_vec.push_back(n);
						}
						int backCount  = backup_vec.size();
						for (int i = 0; i < backCount; i++) {
							MatrixNode n = backup_vec[i];
							Q.push(n);
						}
					}
				}
				if (v.x == Target.x && v.y == Target.y)
					goal_flag = true;
			}
		}
		if (goal_flag == true)
			break;  //to the goal

		//right move
		if ((u.x + 1) <= rowUpperBound) { 
			MatrixNode v = cost_matrix[u.x + 1][u.y];
			if (map_matrix[(u.x + 1)][u.y] != 2) {  //right possible
				int move_cost = 0;
				if (map_matrix[u.x][u.y] == 0 && map_matrix[(u.x + 1)][u.y] == 0)
					move_cost = 0;
				else
					move_cost = 1;

				if (v.cost > (u.cost + move_cost)) //move_cost = 0
				{
					v.cost = u.cost + move_cost;
					v.parent.x = u.x;
					v.parent.y = u.y;
					cost_matrix[u.x + 1][u.y].cost = u.cost + move_cost;
					cost_matrix[u.x + 1][u.y].parent.x = u.x;
					cost_matrix[u.x + 1][u.y].parent.y = u.y;
					if (cost_matrix[u.x + 1][u.y].inqueue == false) {
						Q.push(v);
						v.inqueue = true;
						cost_matrix[u.x + 1][u.y].inqueue = true;
					}
					else { //update MatrixNode already in queue
						vector<MatrixNode> backup_vec;
						while (Q.size() != 0) {
							MatrixNode n = Q.top();
							Q.pop();
							if (n.x == v.x && n.y == v.y)
							{
								Q.push(v);
								break;
							}
							backup_vec.push_back(n);
						}
						int backCount = backup_vec.size();
						for (int i = 0; i < backCount; i++) {
							MatrixNode n = backup_vec[i];
							Q.push(n);
						}
					}
				}
				if (v.x == Target.x && v.y == Target.y)
					goal_flag = true;
			}
		}
		if (goal_flag == true)
			break;  //to the goal

		//down move
		if ((u.y - 1) >= 1) {
			MatrixNode v = cost_matrix[u.x][u.y-1];
			if (map_matrix[u.x][u.y - 1] != 2) {  //down possible
				int move_cost = 0;
				if (map_matrix[u.x][u.y] == 0 && map_matrix[u.x][u.y-1] == 0)
					move_cost = 0;
				else
					move_cost = 1;

				if (v.cost > (u.cost + move_cost)) //move_cost = 0
				{
					v.cost = u.cost + move_cost;
					v.parent.x = u.x;
					v.parent.y = u.y;
					cost_matrix[u.x][u.y - 1].cost = u.cost + move_cost;
					cost_matrix[u.x][u.y - 1].parent.x = u.x;
					cost_matrix[u.x][u.y - 1].parent.y = u.y;
					if (cost_matrix[u.x][u.y - 1].inqueue == false) {
						Q.push(v);
						v.inqueue = true;
						cost_matrix[u.x][u.y - 1].inqueue = true;
					}
					else { //update MatrixNode already in queue
						vector<MatrixNode> backup_vec;
						while (Q.size() != 0) {
							MatrixNode n = Q.top();
							Q.pop();
							if (n.x == v.x && n.y == v.y)
							{
								Q.push(v);
								break;
							}
							backup_vec.push_back(n);
						}
						int backCount = backup_vec.size();
						for (int i = 0; i < backCount; i++) {
							MatrixNode n = backup_vec[i];
							Q.push(n);
						}
					}
				}
				if (v.x == Target.x && v.y == Target.y)
					goal_flag = true;
			}
		}
		if (goal_flag == true)
			break;  //to the goal

		//up move
		if ((u.y + 1) <= colUpperBound) {
			MatrixNode v = cost_matrix[u.x][u.y + 1];
			if (map_matrix[u.x][u.y + 1] != 2) {  //left possible
				int move_cost = 0;
				if (map_matrix[u.x][u.y] == 0 && map_matrix[u.x][u.y + 1] == 0)
					move_cost = 0;
				else
					move_cost = 1;

				if (v.cost > (u.cost + move_cost)) //move_cost = 0
				{
					v.cost = u.cost + move_cost;
					v.parent.x = u.x;
					v.parent.y = u.y;
					cost_matrix[u.x][u.y + 1].cost = u.cost + move_cost;
					cost_matrix[u.x][u.y + 1].parent.x = u.x;
					cost_matrix[u.x][u.y + 1].parent.y = u.y;
					if (cost_matrix[u.x][u.y + 1].inqueue == false) {
						Q.push(v);
						v.inqueue = true;
						cost_matrix[u.x][u.y + 1].inqueue = true;
					}
					else { //update MatrixNode already in queue
						vector<MatrixNode> backup_vec;
						while (Q.size() != 0) {
							MatrixNode n = Q.top();
							Q.pop();
							if (n.x == v.x && n.y == v.y)
							{
								Q.push(v);
								break;
							}
							backup_vec.push_back(n);
						}
						int backCount = backup_vec.size();
						for (int i = 0; i < backCount; i++) {
							MatrixNode n = backup_vec[i];
							Q.push(n);
						}
					}
				}
				if (v.x == Target.x && v.y == Target.y)
					goal_flag = true;
			}
		}
		if (goal_flag == true)
			break;  //to the goal
		/*if (!improve_flag)  //becasue there is loop in graph, need terminal setting
			break;*/
	}
	if (goal_flag == true)
		PrintPath(cost_matrix, Source, Target);
	else
		cout << "There is no path from source to target" << endl;

	cout << endl << "enter anything to terminate";
	cin.get();
	return 0;
}

void Read_info(string fileName, vector <vector<int>> &map_vec, vector <vector<MatrixNode>> &cost_vec, xyPair &Source, xyPair &Target) {
	fstream fp(fileName, ios::in);
	if (fp.fail())
		return;  //read file fail...
	string tmpLine; 
	int rowNum, colNum = 0; 
	getline(fp, tmpLine); //#Line 1: row  col in grid
	stringstream ssin(tmpLine);
	ssin >> rowNum >> colNum;

	map_vec.resize(rowNum + 1); //0,1~numNode
	cost_vec.resize(rowNum + 1);
	//+1 for mappinf vec index(start from 1) to input index(start from 1)
	for (int i = 0; i < (rowNum + 1); i++) {
		map_vec[i].resize(colNum + 1);
		cost_vec[i].resize(colNum + 1);
		for (int j = 0; j < (colNum + 1); j++) {
			map_vec[i][j] = 1; // 0: zero Area, 1: normal Rd, 2: cant pass Area
			cost_vec[i][j].cost = INT32_MAX;
			cost_vec[i][j].x = i;
			cost_vec[i][j].y = j;
			cost_vec[i][j].inqueue = false;
			cost_vec[i][j].parent.x = -1;  //(-1,-1) means NULL
			cost_vec[i][j].parent.y = -1;
		}
	}

	int index_X, index_Y = 0;
	getline(fp, tmpLine); //#Line 2: Source.x   Source.xy
	stringstream ssin2(tmpLine);
	ssin2 >> index_X >> index_Y;
	Source.x = index_X;
	Source.y = index_Y;
	//assume index_X and index_Y are legal location
	cost_vec[index_X][index_Y].cost = 0; //set source cost is 0

	getline(fp, tmpLine); //#Line 3: Target.x   Target.xy
	stringstream ssin3(tmpLine);
	ssin3 >> index_X >> index_Y;
	Target.x = index_X;
	Target.y = index_Y;

	getline(fp, tmpLine); //#Line 4: blockage: left bottom right up
	int index_X_LB, index_Y_LB = 0;  //left bottom
	int index_X_RU, index_Y_RU = 0;  //right up
	stringstream ssin4(tmpLine);
	ssin4 >> index_X_LB >> index_Y_LB >> index_X_RU >> index_Y_RU;
	for (int i = index_X_LB; i <= index_X_RU; i++)
		for (int j = index_Y_LB; j <= index_Y_RU; j++)
			map_vec[i][j] = 2; // 0: zero Area, 1: normal Rd, 2: cant pass Area

	getline(fp, tmpLine); //#Line 4: free island: left bottom right up
	stringstream ssin5(tmpLine);
	ssin5 >> index_X_LB >> index_Y_LB >> index_X_RU >> index_Y_RU;
	for (int i = index_X_LB; i <= index_X_RU; i++)
		for (int j = index_Y_LB; j <= index_Y_RU; j++)
			map_vec[i][j] = 0; // 0: zero Area, 1: normal Rd, 2: cant pass Area

	fp.close();
}

void PrintPath(vector <vector<MatrixNode>> &cost_vec, xyPair &Source, xyPair &Target) {
	int cost = cost_vec[Target.x][Target.y].cost;
	cout << "Path cost from (" << Source.x << "," << Source.y << ") to (" << Target.x << "," << Target.y << "): "<< cost << endl;
	vector<xyPair> output_vec;
	xyPair tmp_id = { Target.x , Target.y };
	while (tmp_id.x != Source.x || tmp_id.y != Source.y) {
		output_vec.push_back(tmp_id);
		xyPair parent_id = cost_vec[tmp_id.x][tmp_id.y].parent;
		tmp_id.x = parent_id.x;
		tmp_id.y = parent_id.y;
	}
	int path_count = output_vec.size();
	cout << "PATH: ";
	for (int i = (path_count - 1); i >= 0; i--) {
		cout << "(" << output_vec[i].x << "," << output_vec[i].y << ") ";
	}

}
