#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <chrono>
using namespace std::chrono;

using namespace std;

///////// HELPER /////////

#define     EGG         1
#define     CRYSTAL     2

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
		int				beaconed;
		bool				ally_base;
		bool				oppo_base;

		int					nearest_base;
		int					value;

		int					strength;
    
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
	this->nearest_base = 0;
	this->value = 0;
	this->strength = 0;
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
	this->beaconed = 0;
	this->strength = 0;
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
		vector<int>		ally_base_indexes;
		vector<int>		oppo_base_indexes;
        int             n_cells;
        int             oppo_base_index;
        int             ally_ants;
        int             oppo_ants;
		int				init_cystals;
		int				init_eggs;
		int				curr_ants;
		int				potential_ants;
		int				curr_eggs;
		int				curr_crystal;
		int				my_crystals;
		int				oppo_crystals;
		int				map_size;
		bool			egg_phase;
    
    public:
        void            add_cell(Cell new_cell);
        Cell            &get_nearest_cell(Cell start);
        void            print_cells();

        void            set_ants(int _ally_ants, int _oppo_ants);
        void            set_distance(Cell &cell);
		void			set_all_distance();
		void			set_dist_nearest_base(Cell &cell);
		int				set_resources();

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
		std::sort(child_cells.begin(), child_cells.end());
		cell.cells_from_cell.push_back(child_cells);
    }
}

void	CellList::set_dist_nearest_base(Cell &cell)
{
	int min = 2147483647;
	for (int i = 0; i < this->ally_base_indexes.size(); i++)
	{
		int temp = cell.get_dist_index(ally_base_indexes[i]);
		if (temp < min)
		{
			cell.nearest_base = ally_base_indexes[i];
			min = temp;
		}
	}
	// cerr << "NEAREST BASE FROM " << cell.self_index << " IS " << cell.nearest_base << endl; 
}

void	CellList::set_all_distance()
{
	for (int i = 0; i < this->cells.size(); i++)
	{
		set_distance(this->cells[i]);
		set_dist_nearest_base(this->cells[i]);
	}
}

Cell	&CellList::get_cell(int index)
{
	return (this->cells[index]);
}

int		CellList::set_resources()
{
	int eggs_count = 0;
	int crystals_count = 0;
	int ants_count = 0;
	for (int i = 0; i < this->cells.size(); i++)
	{
		if (cells[i].type == EGG)
			eggs_count += cells[i].current_resource;
		else if (cells[i].type == CRYSTAL)
			crystals_count += cells[i].current_resource;
		ants_count += cells[i].ally_ants;
		ants_count += cells[i].oppo_ants;
	}
	this->curr_eggs = eggs_count;
	this->curr_crystal = crystals_count;
	this->curr_ants = ants_count;
	this->potential_ants = ants_count + eggs_count;
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
			|| this->cells[parent_cells[i]].current_resource > 0
			|| this->cells[parent_cells[i]].ally_ants > 0))
			return (parent_cells[i]);
	}

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

	int dist = this->cells[index1].get_dist_index(index2);

	vector<vector<int>> all_paths;
	all_paths.push_back(vector<int> (1, cell_2.self_index)); // {[11]}

	// cerr << "PATH " << index1 << " TO " << index2 << endl;
	while (dist--)
	{
		vector<vector<int>> new_paths;
		for (int i = 0; i < all_paths.size(); i++)
		{
			for (int j = 0; j < cell_1.cells_from_cell[dist].size(); j++)
			{
				if (this->is_neighboor(all_paths[i].back(), cell_1.cells_from_cell[dist][j]))
				{
					vector<int> temp;
					temp.assign(all_paths[i].begin(), all_paths[i].end());
					temp.push_back(cell_1.cells_from_cell[dist][j]);
					new_paths.push_back(temp);
				}
			}
		}
		all_paths = new_paths;
	}

	// cerr << "ALL PATH = " << endl;
	// for (int i = 0; i < all_paths.size(); i++)
	// {
	// 	cerr << "[" << i << "]: ";
	// 	for (int j = 0; j < all_paths[i].size(); j++)
	// 	{
	// 		cerr << " ["<< all_paths[i][j] << "]";
	// 	}
	// 	cerr << endl;
	// }

	int max = 0;
	vector<int> best_path = all_paths[0];
	for (int i = 0; i < all_paths.size(); i++)
	{
		int count = 0;
		for (int j = 0; j < all_paths[i].size(); j++)
		{
			if (this->cells[all_paths[i][j]].current_resource)
				count += this->cells[all_paths[i][j]].initial_resource;
			if (this->cells[all_paths[i][j]].beaconed)
				count += 1;
			if (this->cells[all_paths[i][j]].ally_ants)
				count += 1;
			// TAKE INTO ACCOUNT BEACONS
		}
		if (count > max)
		{
			max = count;
			best_path = all_paths[i];
		}
	}

	return (best_path);
}

void    CellList::add_cell(Cell new_cell)
{
    this->cells.push_back(new_cell);
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


///////// PATH VALUE CLASS /////////

class ValuePath
{
	public:
		ValuePath(vector<int> _path, int _path_value);

	public:
		int				path_value;

		vector<int> 	indexes;
		vector<int> 	strength;

		vector<Cell> 	cells;

	public:
	
};

ValuePath::ValuePath(vector<int> _path, int _path_value)
{
	for (int i = 0; i < _path.size(); i++)
	{
		this->cells.push_back(cellList.cells[_path[i]]);
	}
	this->path_value = _path_value;
	this->indexes = _path;
}

///////// END OF PATH VALUE CLASS /////////

// ValuePath	all_path;


///////// STRATEGY FUNCTION /////////

void    LINE(int origin, int target, int strength)
{
    cout << "LINE " << origin << " " << target << " " << strength << ";";
}

void	BEACON(int index, int strength)
{
	if (strength > 0 && strength > cellList.cells[index].beaconed)
	{
		cellList.cells[index].beaconed = strength;
		cout << "BEACON " << index << " " << strength << ";";
	}
}

int		get_nearest_in_indexes(Cell cell, vector<int> indexes)
{
	int min = 2147483647;
	for (int i = 0; i < indexes.size(); i++)
	{
		int temp = cell.get_dist_index(indexes[i]);
		if (temp < min)
			min = temp;
	}
	return (min);
}

void	set_cells_value()
{
	vector<Cell>	&cells = cellList.cells;
	
	// MAP SIZE REF
	cellList.map_size = (cellList.cells.size() / 5) + 1;
	// cerr << "MAP SIZE = " << cellList.map_size << endl;

	// SPREADING COST
	// EXPENSIVES TO GO FAR IF BIG MAP AND NOT MANY ANTS
	int spread_cost = cellList.map_size / cellList.ally_ants;

	int egg_value = 10;
	int	crystal_value = 10;
	// COMPARE EGGS TO CRYTALS

	if (cellList.potential_ants % 2 == 0 && cellList.ally_ants >= cellList.potential_ants / 2)
		egg_value = 0;
	else if (cellList.potential_ants % 2 == 1 && cellList.ally_ants >= (cellList.potential_ants + 1) / 2)
		egg_value = 0;
	else
		egg_value = 30;

	(cellList.ally_ants <= cellList.potential_ants / 4 ? cellList.egg_phase = true : cellList.egg_phase = false);

	if (cellList.curr_crystal < cellList.init_cystals / 4)
		crystal_value = 30;

	// FOR EACH CELL
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells[i].current_resource == 0)
		{
			cells[i].value = 0;
			continue;
		}

		// GET QUANTITY OF RESSOURCE COMPARED TO ALL AVAILABLE
		int weight = 1;
		// if (cells[i].type == EGG)
		// 	weight = cells[i].initial_resource * 100 / cellList.init_eggs;
		// else if (cells[i].type == CRYSTAL)
		// 	weight = cells[i].initial_resource * 100 / cellList.init_cystals;
		// cerr << "WEIGHT OF CELL " << cells[i].self_index << " IS " << weight << endl;

		// DISTANCE VALUES
		int dist_value = 0;

		// GET DISTANCE FROM NEAREST ALLY BASE
		int dist_min_ally = get_nearest_in_indexes(cells[i], cellList.ally_base_indexes);

		// GET DISTANCE FROM NEAREST ENNEMY BASE
		int dist_min_oppo = get_nearest_in_indexes(cells[i], cellList.oppo_base_indexes);

		// NULLIFY VALUE IF TOO FAR FROM A BASE

		if (cellList.egg_phase && cells[i].type == EGG)
		{
			if (dist_min_ally > dist_min_oppo)
				dist_value = 0 - dist_min_ally;
			else if (dist_min_ally == dist_min_oppo)
				dist_value = cellList.map_size - dist_min_ally;
			else
				dist_value = 2 * cellList.map_size - dist_min_ally;
		}
		else
		{
			if (dist_min_ally > cellList.ally_ants)
				dist_value = 0;
			else if (dist_min_ally > dist_min_oppo)
				dist_value = 0 - dist_min_ally;
			else if (dist_min_ally == dist_min_oppo)
				dist_value = 2 * cellList.map_size - dist_min_ally;
			else
				dist_value = cellList.map_size - dist_min_ally;
		}

		// SET VALUE FOR THIS CELL
		if (cells[i].type == EGG)
			cells[i].value = egg_value * weight * dist_value;
		else if (cells[i].type == CRYSTAL)
			cells[i].value = crystal_value * weight * dist_value;
		// cerr << "VALUE" << cells[i].self_index << "=" << cells[i].value << " DIST=" << dist_value << " ALLY=" << dist_min_ally << " OPPO=" << dist_min_oppo << endl;
	}
}

void	sort_cells_by_value(vector<Cell> &cells)
{
	std::sort(cells.begin(), cells.end(), [](Cell a, Cell b) {return (a.value > b.value);});
}

bool	am_I_too_rich(vector<Cell>	&target_cells, Cell curr_cell)
{
	int		crystal_count = cellList.my_crystals;

	for (int i = 0; i < target_cells.size(); i++)
	{
		if (target_cells[i].type == CRYSTAL)
			crystal_count += target_cells[i].current_resource;
	}
	// crystal_count += curr_cell.current_resource;
	// cerr << "HAVE " << cellList.my_crystals  << " MAP " << cellList.init_cystals << " CRYST COUNT " << crystal_count << endl;

	if (cellList.init_cystals % 2 == 1 && crystal_count >= (cellList.init_cystals + 1) / 2)
		return (true);
	if (cellList.init_cystals % 2 == 0 && crystal_count >= cellList.init_cystals / 2)
		return (true);
	return (false);
}

vector<vector<Cell>>	generate_priority_cells()
{
	vector<vector<Cell>>	cells_to_link;

	set_cells_value();

	int max_target = (cellList.ally_ants / 10) + 1;

	vector<Cell>	cells_for_base;
	for (int i = 0 ; i < cellList.cells.size(); i++)
		if (cellList.cells[i].current_resource)
			cells_for_base.push_back(cellList.cells[i]);
	sort_cells_by_value(cells_for_base);

	vector<Cell>	target_cell;
	for (int i = 0; i < max_target && i < cells_for_base.size(); i++)
	{
		// cerr << "TARGET " << cells_for_base[i].self_index << " VALUE " << cells_for_base[i].value << endl;
		target_cell.push_back(cells_for_base[i]);
		if (am_I_too_rich(target_cell, cells_for_base[i]) && target_cell.size() >= 1)
			break;
		// TO AVOID GOING TOO MUCH TOWARD LOW VALUE CELL
		if (cells_for_base[i].value <= 0)
			i++;
	}
	cells_to_link.push_back(target_cell);

	return (cells_to_link);
}

int 	best_start_link(vector<int> &linked_cell_indexes, Cell target)
{
	vector<int>	nearest_points;
	int nearest_cell = target.self_index;
	int min = 10000;
	// SEARCH NEAREST POINT
	for (int i = 0; i < linked_cell_indexes.size(); i++)
	{
		int dist = target.get_dist_index(linked_cell_indexes[i]);
		if (dist < min)
		{
			nearest_points.clear();
			nearest_points.push_back(linked_cell_indexes[i]);
			nearest_cell = linked_cell_indexes[i];
			min = dist;
		}
		else if (dist == min)
			nearest_points.push_back(linked_cell_indexes[i]);
	}

	// SEARCH IF MULTIPLE NEAREST POINT
	if (nearest_points.size() > 1)
	{
		int max = 0;
		for (int i = 0; i < nearest_points.size(); i++)
		{
			int count = 0;
			vector<int>	path = cellList.path_index_to_index(target.self_index, nearest_points[i]);
			for (int j = 0; j < path.size(); j++)
			{
				if (cellList.cells[path[i]].current_resource)
					count++;
				if (cellList.cells[path[i]].ally_ants)
					count++;
			}
			if (count > max)
			{
				max = count;
				// cerr << "GOING FROM " << nearest_points[i] << " TO " << target.self_index << " IS BETTER THAN " << nearest_cell << " TO " << target.self_index << endl; 
				nearest_cell = nearest_points[i];
			}
		}
	}

	// cerr << "LINKING " << target.self_index << " TO " << nearest_cell << endl;
	return (nearest_cell);
}

void	sort_cells_by_distance(vector<Cell> &cells)
{
	std::sort(cells.begin(), cells.end(), [](Cell a, Cell b) 
	{
		return (a.get_dist_index(a.nearest_base) < b.get_dist_index(b.nearest_base));
	});
}

void	go_little_ants()
{
	// INIT INITIAL LIST OF RESSOURCE CELLS + ALLY BASES
	vector<vector<Cell>>	cells_to_link = generate_priority_cells();
	// ADD BASES AS STARTING POINT FOR LINKING
	vector<int>		linked_cell_indexes;
	for (int i = 0; i < cellList.ally_base_indexes.size(); i++)
		linked_cell_indexes.push_back(cellList.ally_base_indexes[i]);

	vector<Cell>	cells_to_link_base_i = cells_to_link[0];
	vector<int> 	full_path;
	vector<ValuePath>		all_paths;
	
	// DEFINE PATH
	sort_cells_by_distance(cells_to_link_base_i);
	for (vector<Cell>::iterator it = cells_to_link_base_i.begin(); it != cells_to_link_base_i.end();)
	{
		int best_start = best_start_link(linked_cell_indexes, *it);
		linked_cell_indexes.push_back((*it).self_index);
		vector<int> path = cellList.path_index_to_index((*it).self_index, best_start);
		all_paths.push_back(ValuePath(path, (*it).value));
		full_path.insert(full_path.end(), path.begin(), path.end());
		cells_to_link_base_i.erase(it);
	}

	// CLEAN DOUBLE BEACON AND COUNT TOTAL CELLS
	std::sort(full_path.begin(), full_path.end());
	auto last = std::unique(full_path.begin(), full_path.end());
	full_path.erase(last, full_path.end());
	int full_path_size = full_path.size();

	if (full_path.empty())
	{
		cout << "WAIT;";
		return;
	}

	// // GO GO GO GO
	if (cellList.egg_phase)
	{
		for (int i = 0; i < full_path.size(); i++)
		{
			int strength = 1;
			BEACON(full_path[i], strength);
		}
	}
	else
	{
		// EXPERIMENTAL
		for (int i = 0; i < all_paths.size(); i++)
		{
			cerr << "PATH " << i << " : ";
			for (int j = 0; j < all_paths[i].indexes.size(); j++)
			{
				cerr << all_paths[i].indexes[j] << " "; 
			}
			cerr << endl;
		}

		for (int i = 0; i < all_paths.size(); i++)
		{
			ValuePath curr_path = all_paths[i];
			int curr_ants = 0;
			int path_size = 0;
			int total_path_ants = 0;
			int	ants_per_cell = 0;

			for (int j = 0; j < curr_path.cells.size(); j++)
				curr_ants += curr_path.cells[j].ally_ants;

			if (i == 0)
				path_size = curr_path.cells.size();
			else
				path_size = curr_path.cells.size() - 1;

			total_path_ants = (path_size * cellList.ally_ants) / full_path_size;

			ants_per_cell = total_path_ants / path_size;

			int	salt = 0;
			if (curr_path.cells.back().oppo_ants >= curr_path.cells.back().ally_ants)
				salt = 1;
			else if (curr_path.cells.back().current_resource < ants_per_cell)
				salt = ants_per_cell - curr_path.cells.back().current_resource;
			
			
			cerr << "CURRENT ANTS " << curr_ants << " TOTAL PATH ANTS " << total_path_ants << endl;
			cerr << "ANTS PER CELL " << ants_per_cell << endl;

			if (curr_ants == 0)
			{
				curr_path.cells.back().strength = total_path_ants;
			}
			else if (curr_ants < total_path_ants)
			{
				int ants_left = total_path_ants;
				curr_path.cells.front().strength = ants_per_cell;
				for (int j = curr_path.cells.size() - 1; j > 0; j--)
				{
					if (curr_path.cells[j].ally_ants)
					{
						curr_path.cells[j].strength = curr_ants / path_size;
						ants_left -= curr_ants / path_size;
					}
					else
					{
						
						break;
					}
				}
				for (int j = 1; j < curr_path.cells.size(); j++)
				{
					if (curr_path.cells[j].ally_ants)
					{
						curr_path.cells[j].strength = curr_ants / path_size;
						ants_left -= curr_ants / path_size;
					}
					else
					{
						curr_path.cells[j].strength = ants_left;
						break;
					}
				}
			}
			else
			{
				int min = (*min_element(curr_path.cells.begin(), curr_path.cells.end(), 
							[](Cell a, Cell b) {return(a.ally_ants < b.ally_ants);})).ally_ants;
				vector<int> min_ants;
				int max_str = total_path_ants;
				for (int i = 0; i < curr_path.cells.size(); i++)
				{
					if (curr_path.cells[i].ally_ants <= min)
					{
						min_ants.push_back(curr_path.cells[i].self_index);
						continue;
					}
					int self_ants = curr_path.cells[i].ally_ants;
					int right_theorical = ants_per_cell * curr_path.cells.size() - 1 - i;
					int left_theorical = i;
					int right_ants = 0;
					for (int j = i + 1; j < curr_path.cells.size(); j++)
						right_ants += curr_path.cells[j].ally_ants;
					int left_ants = total_path_ants - right_ants - self_ants;
					
					// x < 0 = NEED ANTS ON LEFT, x > 0 = TOO MUCH ANTS ON LEFT
					int left_stream = left_ants - left_theorical;
					
					// x < 0 = NEED ANTS ON RIGHT, x > 0 = TOO MUCH ANTS ON RIGHT
					int right_stream = right_ants - right_theorical;

					if (left_stream <= 0 && right_stream <= 0)
					{
						curr_path.cells[i].strength = ants_per_cell;
					}
					else if (left_stream > 0 && right_stream > 0)
					{
						
					}
				}
			}

			for (int m = 0; m < curr_path.cells.size(); m++)
			{
				cerr << "BEACON " << curr_path.cells[m].self_index << " STR " << curr_path.cells[m].strength << endl; 
				BEACON(curr_path.cells[m].self_index, curr_path.cells[m].strength);
			}
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

	cellList.set_all_distance();
    //// END OF GAME START ENTRY ////

    //// INIT BEFORE GAME LOOP ////

    //// GAME LOOP  ////
    while (1) {
		// auto start = high_resolution_clock::now();
        //// MAP UPDATE ////
		int my_score;
        int opp_score;
        cin >> my_score >> opp_score; cin.ignore();
		cellList.my_crystals = my_score;
		cellList.oppo_crystals = opp_score;
        cellList.set_ants(0, 0); // reset ants before recounting them
        for (int i = 0; i < number_of_cells; i++) {
            int resources; // the current amount of eggs/crystals on this cell
            int my_ants; // the amount of your ants on this cell
            int opp_ants; // the amount of opponent ants on this cell
            cin >> resources >> my_ants >> opp_ants; cin.ignore();
            cellList.cells[i].update_cell(resources, my_ants, opp_ants);
            cellList.set_ants(cellList.ally_ants + my_ants, cellList.oppo_ants + opp_ants);
        }
		cellList.set_resources();

        //// MY INSTRUCTIONS ////
        // WAIT | LINE <sourceIdx> <targetIdx> <strength> | BEACON <cellIdx> <strength> | MESSAGE <text>

		go_little_ants();

        cout << endl;
		// auto stop = high_resolution_clock::now();
		// auto duration = duration_cast<microseconds>(stop - start);
		// cerr << "TIME: " << duration.count() << endl;
    }
    //// END OF GAME LOOP ////
}