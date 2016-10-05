#include "Maze.h"
#include <string>
#include<iostream>
#include <fstream>
#include<cstdlib>
#include<iostream>
#include<vector>
using namespace std;


Maze::Maze()
{		
	solved_steps=0;
	read_file();
	read_path_file();
	solve();
	flag_x=-3;
	flag_y=25;
}

Maze::~Maze()
{
	for(int i=0; i<row; i++)
	{
		delete[] vert_mtrx[i];
	}
	delete[] vert_mtrx;
	vert_mtrx=NULL;
}

void Maze::read_file()
{
	string file_name="maze-51x51output.txt";
	string temp;
	ifstream instream;

	instream.open(file_name);
	if(instream.fail())
	{
		cout<<"opening "<<file_name<<" failed"<<endl;
		exit(1);
	}

	getline(instream, temp);
	row=atoi(temp.c_str());
	getline(instream, temp);
	col=atoi(temp.c_str());
	
	//initialize 2d array
	vert_mtrx = new string*[row];
	for(int i=0; i<row; i++)
	{
		vert_mtrx[i]=new string[col];
	}
	//hold this for file at the end
	getline(instream, start_end);


	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			instream>>temp;
			temp=dec_to_bi(atoi(temp.c_str()));
			vert_mtrx[i][j]=temp;
		}
	}
	instream.close();

}

string Maze::dec_to_bi(int base_10)
{
	//int base_10=dec;
	string binary;
	if(base_10>=8)
	{
		binary="1";
		base_10-=8;
	}
	else
	{
		binary="0";
	}

	if(base_10>=4)
	{
		binary+="1";
		base_10-=4;
	}
	else
	{
		binary+="0";
	}

	if(base_10>=2)
	{
		binary+="1";
		base_10-=2;
	}
	else
	{
		binary+="0";
	}

	if(base_10==1)
	{
		binary+="1";
	}
	else
	{
		binary+="0";
	}

	return binary;
} 

//will want to make this take two ints as the starting point
void Maze::solve()
{
	//this will be the start of the maze
	int x=0;
	int y=23;
	ans.clear();

	vector<int> inner;
	inner.push_back(path[x*col+y][0]);
	inner.push_back(path[x*col+y][1]);
	inner.push_back(path[x*col+y][2]);
	inner.push_back(path[x*col+y][3]);
	ans.push_back(inner);
	inner.clear();
	//this will be true when you have hit the end
	while(path[x*col+y][3]!=-1)
	{
		solved_steps++;
		int temp_x=path[x*col+y][2];
		int temp_y=path[x*col+y][3];
		x=temp_x;
		y=temp_y;
		inner.push_back(path[x*col+y][0]);
		inner.push_back(path[x*col+y][1]);
		inner.push_back(path[x*col+y][2]);
		inner.push_back(path[x*col+y][3]);
		ans.push_back(inner);
		inner.clear();
	}
}

void Maze::flag_pos(int x, int y)
{
	int i=0;
	while(path[x*col+y][3]!=-1 && i<3)
	{
		i++;
		int new_temp_x=path[x*col+y][2];
		int new_temp_y=path[x*col+y][3];
		int old_temp_x=path[x*col+y][0];
		int old_temp_y=path[x*col+y][1];
		x=new_temp_x;
		y=new_temp_y;
		if(old_temp_x<new_temp_x)
		{
			flag_y--;
		}
		if(old_temp_x>new_temp_x)
		{
			flag_y++;
		}
		if(old_temp_y<new_temp_y)
		{
			flag_x++;
		}
		if(old_temp_y>new_temp_y)
		{
			flag_x--;
		}
	}
}

void Maze::read_path_file()
{
	string file_name="maze-51x51output-paths.txt";
	string temp;
	ifstream instream;

	path = new int*[row*col];
	for(int i=0; i<row*col; i++)
	{
		path[i]=new int[4];
	}

	instream.open(file_name);
	if(instream.fail())
	{
		cout<<"opening "<<file_name<<" failed"<<endl;
		exit(1);
	}

	for(int i=0; i<51*51; i++)
	{
		for(int j=0; j<4; j++)
		{
			instream>>temp;	
			path[i][j]= atoi(temp.c_str());
		}
	}
	instream.close();
}

int Maze::get_col()
{
	return col;
}

int Maze::get_row()
{
	return row;
}

string** Maze::get_vert_mtrx()
{
	return vert_mtrx;
}

string Maze::get_start_end()
{
	return start_end;
}

vector< vector<int> > Maze::get_ans()
{
	return ans;
}