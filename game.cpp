#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

///////// HELPER /////////

#define     EGG         1
#define     CRYSTAL     2

void    LINE(int origin, int target, int strength)
{
    cout << "LINE " << origin << " " << target << " " << strength << ";";
}

void	BEACON(int index, int strength)
{
	cout << "BEACON " << index << " " << strength << ";";
}

///////// CELL CLASS /////////

class Cell
{
    public:
        vector<vector<int>> cells_from_cell;
        vector<int>         neigh_index;

        int                 type;
        int                 initial_resource;
        int                 current_resource;
        int                 ally_ants;
        int                 oppo_ants;

        int                 self_index;
		int					beaconed;
		bool				ally_base;
		bool				oppo_base;
    
    public:
        Cell(int _type, int _initial_resource, int self_index, int _neigh_0, int _neigh_1, int _neigh_2, int _neigh_3, int _neigh_4, int _neigh_5);

        void    update_cell(int _ressources, int _ally_ants, int _oppo_ants);
		int		get_dist_index(int index);
        void    print_cell(string msg);

};

Cell::Cell(int _type, int _initial_resource, int _self_index, int _neigh_0, int _neigh_1, int _neigh_2, int _neigh_3, int _neigh_4, int _neigh_5)
{
    this->self_index = _self_index;
    this->type = _type;
    this->initial_resource = _initial_resource;
    this->current_resource = _initial_resource;
    this->neigh_index.push_back(_neigh_0);
    this->neigh_index.push_back(_neigh_1);
    this->neigh_index.push_back(_neigh_2);
    this->neigh_index.push_back(_neigh_3);
    this->neigh_index.push_back(_neigh_4);
    this->neigh_index.push_back(_neigh_5);
    this->ally_ants = 0;
    this->oppo_ants = 0;
	this->ally_base = false;
	this->oppo_base = false;
	this->beaconed = 0;
}

int		Cell::get_dist_index(int index)
{
	for (int dist = 0; dist < this->cells_from_cell.size(); dist++)
	{
		for (int i = 0; i < this->cells_from_cell[dist].size(); i++)
		{
			if (this->cells_from_cell[dist][i] == index)
				return (dist);
		}
	}
	return (-1);
}

void    Cell::update_cell(int _ressources, int _ally_ants, int _oppo_ants)
{
    this->current_resource = _ressources;
    this->ally_ants = _ally_ants;
    this->oppo_ants = _oppo_ants;
}

void    Cell::print_cell(string msg)
{
    cerr << "=====  " << msg << "  =====" << endl;
    cerr << "index: " << this->self_index << endl;
    cerr << "type: " << this->type << endl;
    cerr << "init_ress: " << this->initial_resource << endl;
    cerr << "curr_ress: " << this->current_resource << endl;
    for (int j = 0; j < this->neigh_index.size(); j++)
    {
        cerr << "neigh_" << j << ": " << this->neigh_index[j] << endl;
    }
	cerr << "cells from cell:" << endl;
	for (int i = 0; i < this->cells_from_cell.size(); i++)
	{
		cerr << "i[" << i << "] = "; 
		for (int j = 0; j < this->cells_from_cell[i].size(); j++)
		{
			cerr << "[" << this->cells_from_cell[i][j] << "] ";
		}
		cerr << endl;
	}
}

///////// END OF CELL CLASS /////////



///////// CELL LIST CLASS /////////
class CellList
{
    public:
        vector<Cell>    cells;
        vector<Cell>    ressource_cells;
		vector<int>		ally_base_indexes;
		vector<int>		oppo_base_indexes;
        int             n_cells;
        int             oppo_base_index;
        int             ally_ants;
        int             oppo_ants;
		int				init_cystals;
		int				init_eggs;
    
    public:
        void            add_cell(Cell new_cell);
        Cell            &get_nearest_cell(Cell start);
        void            print_cells();

        void            set_ants(int _ally_ants, int _oppo_ants);
        void            set_distance(Cell &cell);
		void			set_all_distance();

		Cell			&get_cell(int index);

		bool			is_neighboor(int index1, int index2);
		int				most_popular_neigh(vector<int> neigh_list);
		int				best_neighboor(vector<int> parent_cells, Cell child_cell);

		vector<int>		path_index_to_index(int index1, int index2);
};

void    CellList::set_ants(int _ally_ants, int _oppo_ants)
{
    this->ally_ants = _ally_ants;
    this->oppo_ants = _oppo_ants;
}

// current_floor = max_range in vector to search, value = value to search for
bool	search_parent_and_bros(vector<vector<int>> &cell_from_cells, vector<int> current_vec,int current_floor, int value)
{
	//check parents
	for (int i = 0; i < current_floor; i++)
	{
		for (int j = 0; j < cell_from_cells[i].size(); j++)
		{
			if (cell_from_cells[i][j] == value)
				return (true);
		}
	}
	//check bros
	for (int i = 0; i < current_vec.size(); i++)
	{
		if (current_vec[i] == value)
			return (true);
	}
	return (false);
}

void    CellList::set_distance(Cell &cell)
{
    for (int dist = 0; dist < this->n_cells; dist++)
    {
        vector<int> child_cells;
        if (dist == 0)
        {
            child_cells.push_back(cell.self_index);
        }
        else
        {
            for (int i = 0; i < cell.cells_from_cell[dist - 1].size(); i++)
            {
				// kleki 01
                for (int neigh = 0; neigh < this->cells[cell.cells_from_cell[dist - 1][i]].neigh_index.size(); neigh++)
				{
					vector<int> neigh_vec = this->cells[cell.cells_from_cell[dist - 1][i]].neigh_index;
					if (neigh_vec[neigh] != -1 && !search_parent_and_bros(cell.cells_from_cell, child_cells, dist, neigh_vec[neigh]))
						child_cells.push_back(neigh_vec[neigh]);
				}
            }
        }
		if (child_cells.empty())
			break;
		cell.cells_from_cell.push_back(child_cells);
    }
}

void	CellList::set_all_distance()
{
	for (int i = 0; i < this->cells.size(); i++)
	{
		set_distance(this->cells[i]);
	}
}

Cell	&CellList::get_cell(int index)
{
	return (this->cells[index]);
}


bool	CellList::is_neighboor(int index1, int index2)
{
	Cell cell_1 = this->cells[index1];
	
	for (int i = 0; i < cell_1.neigh_index.size(); i++)
		if (cell_1.neigh_index[i] == index2)
			return (true);
	return (false);
}

int		neigh_count(vector<int> neigh_indexes)
{
	int count = 0;
	for (int i = 0; i < neigh_indexes.size(); i++)
	{
		if (neigh_indexes[i])
			count++;
	}
	return (count);
}

int		CellList::most_popular_neigh(vector<int> neigh_list)
{
	int max = 0;
	int most_popular = neigh_list[0];

	for (int i = 0; i < neigh_list.size(); i++)
	{
		int count = 0;
		vector<int> curr_neigh = this->cells[neigh_list[i]].neigh_index;
		for (int j = 0; j < curr_neigh.size(); j++)
		{
			if (curr_neigh[j])
				count++;
			if (this->cells[curr_neigh[j]].type)
				count++;
		}
		if (count > max)
		{
			max = count;
			most_popular = neigh_list[i];
		}
	}
	return (most_popular);
}

int		CellList::best_neighboor(vector<int> parent_cells, Cell child_cell)
{
	vector<int>	interesting_neigh;
	for (int i = 0; i < parent_cells.size(); i++)
	{
		if (is_neighboor(child_cell.self_index, parent_cells[i]) 
		&& (this->cells[parent_cells[i]].beaconed 
			|| this->cells[parent_cells[i]].current_resource > 0))
			return (parent_cells[i]);
			// interesting_neigh.push_back(parent_cells[i]);
	}
	// if (!interesting_neigh.empty())
	// 	return (most_popular_neigh(interesting_neigh));

	vector<int> normal_neigh;
	for (int i = 0; i < parent_cells.size(); i++)
	{
		if (is_neighboor(child_cell.self_index, parent_cells[i]))
			normal_neigh.push_back(parent_cells[i]);
	}
	if (!normal_neigh.empty())
		return (most_popular_neigh(normal_neigh));

	cerr << "BEST NEIGHBOOR NOT FOUND :(" << endl;
	return (0);
}

vector<int>		CellList::path_index_to_index(int index1, int index2)
{
	Cell cell_1 = this->cells[index1]; // 0
	Cell cell_2 = this->cells[index2]; // 13
	int dist = 0;

	for (int i = 0; i < cell_1.cells_from_cell.size(); i++)
	{
		for (int j = 0; j < cell_1.cells_from_cell[i].size(); j++)
		{
			if (cell_1.cells_from_cell[i][j] == cell_2.self_index)
			{
				dist = i; // dist = 3
				goto stop;
			}
		}
	}

	stop:
	vector<int> path;
	int point = cell_2.self_index; // = 13
	path.push_back(cell_2.self_index);
	while (dist--)
	{
		int best_neigh = best_neighboor(cell_1.cells_from_cell[dist], this->cells[point]);
		path.push_back(best_neigh);
		point = best_neigh;
	}
	return (path);
}

void    CellList::add_cell(Cell new_cell)
{
    this->cells.push_back(new_cell);
    if (new_cell.type == EGG || new_cell.type == CRYSTAL)
        this->ressource_cells.push_back(new_cell);
	if (new_cell.type == EGG)
		this->init_eggs += new_cell.current_resource;
	if (new_cell.type == CRYSTAL)
		this->init_cystals += new_cell.current_resource;
}

void    CellList::print_cells()
{
    for (int i = 0; i < this->cells.size(); i++)
    {
		if (cells[i].type != 0)
        	cells[i].print_cell("INIT");
    }
}

///////// END OF CELL LIST CLASS /////////

CellList    cellList;

///////// STRATEGY FUNCTION /////////

int		nearest_cell_in_vector(Cell base, vector<Cell> cell_vec)
{
	for (int i = 1; i < base.cells_from_cell.size(); i++)
	{
		for (int j = 0; j < base.cells_from_cell[i].size(); j++)
		{
			for (int k = 0; k < cell_vec.size(); k++)
			{
				if (cell_vec[k].self_index == base.cells_from_cell[i][j])
					return (base.cells_from_cell[i][j]);
			}
		}
	}
	return (cellList.ally_base_indexes[0]);
}

// DEFINE RANGE TO SEARCH FOR PRIORITY CELLS
int						max_range = 5;
vector<vector<Cell>>	generate_priority_cells()
{
	vector<vector<Cell>>	cells_to_link;

	for (int i = 0; i <  cellList.ally_base_indexes.size(); i++)
	{
		vector<Cell>	cells_for_base;
		Cell			curr_base = cellList.cells[cellList.ally_base_indexes[i]];

		for (int dist = 1; dist < max_range && dist < curr_base.cells_from_cell.size(); dist++)
		{
			for (int i = 0; i < curr_base.cells_from_cell[dist].size(); i++)
			{
				Cell cell = cellList.get_cell(curr_base.cells_from_cell[dist][i]);
				if (cell.current_resource > 0)
					cells_for_base.push_back(cell);
			}
		}
		if (cells_for_base.size() < 5)
			max_range += 2;
		cells_to_link.push_back(cells_for_base);
	}

	return (cells_to_link);
}

int 	best_start_link(vector<int> &linked_cell_indexes, Cell target)
{
	int nearest_cell = target.self_index;
	int min = 10000;
	for (int i = 0; i < linked_cell_indexes.size(); i++)
	{
		int dist = target.get_dist_index(linked_cell_indexes[i]);
		if (dist < min)
		{
			nearest_cell = linked_cell_indexes[i];
			min = dist;
		}
	}
	// cerr << "LINKING " << target.self_index << " TO " << nearest_cell << endl;  
	return (nearest_cell);
}

void	path_resource_to_resouce()
{
	// INIT INITIAL LIST OF RESSOURCE CELLS + ALLY BASES
	vector<int>				best_path;
	vector<vector<Cell>>	cells_to_link = generate_priority_cells();

	// ADD BASES AS STARTING POINT FOR LINKING
	vector<int>		linked_cell_indexes;
	for (int i = 0; i < cellList.ally_base_indexes.size(); i++)
		linked_cell_indexes.push_back(cellList.ally_base_indexes[i]);

	// BEACON ONE OR MANY MAPS DEPENDING ON CELLS TO LINK
	for (int base = 0; base < cells_to_link.size(); base++)
	{
		vector<Cell>	cells_to_link_base_i = cells_to_link[base];
		vector<int> 	full_path;
		
		// DEFINE PATH
		for (vector<Cell>::iterator it = cells_to_link_base_i.begin(); it != cells_to_link_base_i.end();)
		{
			int best_start = best_start_link(linked_cell_indexes, *it);
			linked_cell_indexes.push_back((*it).self_index);
			vector<int> path = cellList.path_index_to_index((*it).self_index, best_start);
			full_path.insert(full_path.end(), path.begin(), path.end());
			cells_to_link_base_i.erase(it);
		}

		// CLEAN DOUBLE BEACON
		std::sort(full_path.begin(), full_path.end());
		auto last = std::unique(full_path.begin(), full_path.end());
		full_path.erase(last, full_path.end());

		// GO GO GO GO
		for (int i = 0; i < full_path.size(); i++)
		{
			int strength = 1;
			if (cellList.cells[full_path[i]].oppo_base)
				strength = 3;
			BEACON(full_path[i], strength);
		}
	}
}

///////// STRATEGY FUNCTION /////////

int main()
{

    //// GAME START ENTRY ////
    int number_of_cells; // amount of hexagonal cells in this map
    cin >> number_of_cells; cin.ignore();
    cellList.n_cells = number_of_cells;
    for (int i = 0; i < number_of_cells; i++) {
        int type; // 0 for empty, 1 for eggs, 2 for crystal
        int initial_resources; // the initial amount of eggs/crystals on this cell
        int neigh_0; // the index of the neighbouring cell for each direction
        int neigh_1;
        int neigh_2;
        int neigh_3;
        int neigh_4;
        int neigh_5;
        cin >> type >> initial_resources >> neigh_0 >> neigh_1 >> neigh_2 >> neigh_3 >> neigh_4 >> neigh_5; cin.ignore();
        cellList.add_cell(Cell(type, initial_resources, i, neigh_0, neigh_1, neigh_2, neigh_3, neigh_4, neigh_5));
    }
	cellList.set_all_distance();
	// cellList.print_cells();

    int number_of_bases;
    cin >> number_of_bases; cin.ignore();
    for (int i = 0; i < number_of_bases; i++) {
        int my_base_index;
        cin >> my_base_index; cin.ignore();
        cellList.ally_base_indexes.push_back(my_base_index);
		cellList.cells[my_base_index].ally_base = true;
    }
    for (int i = 0; i < number_of_bases; i++) {
        int opp_base_index;
        cin >> opp_base_index; cin.ignore();
		cellList.oppo_base_indexes.push_back(opp_base_index);
        cellList.cells[opp_base_index].oppo_base = true;
    }
    //// END OF GAME START ENTRY ////

    //// INIT BEFORE GAME LOOP ////

    //// GAME LOOP  ////
    while (1) {
        //// MAP UPDATE ////

        cellList.set_ants(0, 0); // reset ants before recounting them
        for (int i = 0; i < number_of_cells; i++) {
            int resources; // the current amount of eggs/crystals on this cell
            int my_ants; // the amount of your ants on this cell
            int opp_ants; // the amount of opponent ants on this cell
            cin >> resources >> my_ants >> opp_ants; cin.ignore();
            cellList.cells[i].update_cell(resources, my_ants, opp_ants);
            cellList.set_ants(cellList.ally_ants + my_ants, cellList.oppo_ants + opp_ants);
        }

        //// MY INSTRUCTIONS ////
        // WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>

		path_resource_to_resouce();

        cout << endl;
    }
    //// END OF GAME LOOP ////
}