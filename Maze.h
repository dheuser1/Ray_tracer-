#ifndef _MAZE_H_
#define _MAZE_H_
#include<iostream>
#include<vector>
using namespace std;

class Maze
{
public:
	Maze();
	~Maze();
	void read_file();
	void read_path_file();
	int get_col();
	int get_row();
	vector< vector<int> >get_ans();
	string **get_vert_mtrx();
	string get_start_end();
	void solve();
	void flag_pos(int x, int y);
	int get_solved_steps(){return solved_steps;}
	int get_flag_x() {return flag_x;}
	int get_flag_y() {return flag_y;}
private:
	int col;
	int row;
	int flag_x;
	int flag_y;
	string start_end;
	string **vert_mtrx;
	vector< vector<int> > ans;
	int **path;
	string dec_to_bi(int base_10);
	int solved_steps;
};
#endif
