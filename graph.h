#pragma once


#include "definitions.h"

//carefull: vector_of_dist isnot a virtual method! Be carefull with its call for different Graph-types

struct Step {

	Step(char _step_name, int v1, int v2) :step_name(_step_name), from(v1), to(v2) {};
	Step() : step_name(' '), from(-1), to(-1) {};

	char step_name = ' ';
	int from = -1;
	int to = -1;
	//-1 means no connection

	friend bool operator==(const Step& s1, const Step& s2) {
		if (s1.step_name == s2.step_name &&
			s1.from == s2.from &&
			s1.to == s2.to)
			return true;
		return false;
	}

	friend bool operator!=(const Step& s1, const Step& s2) {
		return !(s1 == s2);
	}

	friend std::ostream& operator<<(std::ostream& os, const Step& step) {
		os << "Step(" << step.step_name << ", " << step.from << ", " << step.to << ")\n";
		return os;
	}

	
	
};



class Graph
{
	//here graph is undirected. Specific behavior:
	//1. If uv edge exists,then when trying to add vu, adj matrix does not change, but u added to the strategy of v.
	//2. If you try to delete vu, nothing happens to adj matrix, only strategy of v changes
	//3. node degrees calcukated according to adj_matrix, thus, double edges counted once
	
	
public:
	explicit Graph(const unsigned int _num_of_nodes,  const bool _storage_distances, const int _infty);
	Graph(const Graph& _graph); //for copy/-list initialization
	explicit Graph(const unsigned int _num_of_nodes, const std::string input_file_name, const bool pairwise, const bool _storage_distances, const int _infty); //for initialization from a file
	

	void path_graph(const bool pairwise); 
	void cycle_graph(const bool pairwise); 
	void tree_graph(const bool pairwise, const int init_diam); 
	void triangulated_tree_graph(const bool pairwise); 
	void complete_graph(const bool pairwise);
	void triangulated_path_graph(const bool pairwise);
	void triangulated_star(const bool pairwise, const int length_of_vane, const int num_of_vanes);

	void add_edge(const int v1, const int v2, const bool pairwise, const bool recompute_distance_matrix);
	void remove_edge(const int v1, const int v2, const bool pairwise, const bool recompute_distance_matrix);
	void swap_edge(const int agent, const int v_from, const int v_to, const bool recompute_distance_matrix);
	void set_strategy(const int agent, const std::set<int> &strategy, const bool pairwise, const bool recompute_distance_matrix);//in test
	virtual void recompute_distances();

	void perform_step(const int agent, const Step &step, const bool pairwise, const bool recompute_distance_matrix);
	std::vector<int> distances_from_node_after_removed_edges(const int node, const std::set<std::set<int>>& set_of_removed_edges, const int destination = -1) const;
	std::vector<int> distances_from_node_after_edge_swap(const int v_source, const int v_from, const int v_to, const int destination = -1) const;
	std::vector<int> distances_from_node_after_1_step(const Step& step, const int source, const int destinateion = -1) const;
	bool check_if_step_is_allowed(const Step& step, const int source) const; //check if the edge for swap or del exists or does not exists and so on

	//class properties
	int infty_value() const;

	//graph properties
	 int graph_num_of_nodes() const;
	 int num_of_edges() const;
	 int hop_distance(const int n1, const int n2) const;
	double avg_clustering() const;
	bool storage_dist() const;
	int diameter() const;

	//node properties
	const std::set<int> strategy_of_player(const int player) const;
	unsigned int node_degree(const int node) const; 
	std::vector<int> vector_of_hop_dist(const int source,  const int destination = -1) const;
	std::vector<std::vector<int>> get_dist_matrix() const;
	std::vector<int> get_vector_of_dist(const int agent) const;
	std::vector<int> vector_of_dist_weighted(const int source, const int destination = -1) const;
	int dist_betw_nodes(const int n1, const int n2) const;
	

	bool nodes_are_connected(int v1, int v2) const;
	std::set<int> neighborhood(const int node) const;
	std::set<int> k_neighborhood(const int node, const int k) const;
	double local_clustering(const int node) const;


	//output
	virtual void output_graph_to_dot_file(const std::string &file_name) const;
	void output_graph_to_gexf_file(const std::string& file_name) const;

	//operators
	friend bool operator==(const Graph& g1, const Graph& g2) {
		if (g1.adjacency_matrix_int == g2.adjacency_matrix_int &&
			g1.num_of_nodes == g2.num_of_nodes &&
			g1.players_strategy == g2.players_strategy) {
			return true;
		}
		return false;
	}

protected:
	std::vector<std::set<int>> adjacency_matrix_int; 
	int num_of_nodes;
	std::vector<std::set<int>> players_strategy; //set of nodes to which player owns edges
	bool storage_distances; //if true, storage distances in the distance matrix
	std::vector<std::vector<int>> dist_matrix;
	int infty;


};





class Dynamic_W_graph : public Graph {
	//edge weight(u,v) = dist(u,v) in G without this edge
	//dist(u,v) = hop-dist between u, v
	//always recomputes distances
private:
	std::vector<std::vector<int>> edge_weights;
	std::vector<int> dist_from_node_after_edge_modifications(const int node, const std::set<std::pair<int, int>>& added_edges, const std::set<std::pair<int, int>>& deleted_edges, const int destination = -1) const;


public:
	explicit Dynamic_W_graph(const int _num_of_nodes, const int _infty);
	Dynamic_W_graph(const Dynamic_W_graph& _graph) : Graph(_graph), edge_weights(_graph.edge_weights) {};
	explicit Dynamic_W_graph(const int _num_of_nodes, const std::string& _input_file_name, const bool _pairwise, const int _infty);

	
	void recompute_all_edge_weights(const bool pairwise);

	void path_graph(const bool pairwise); 
	void cycle_graph(const bool pairwise); 
	void tree_graph(const bool pairwise, const int init_diam); //generate a path of random hight and connect other nodes sequentially //proper random tree generation assumes init_diam==2
	void triangulated_tree_graph(const bool pairwise); 
	void complete_graph(const bool pairwise);
	void triangulated_path_graph(const bool pairwise);
	void triangulated_star(const bool pairwise, const int length_of_vane, const int num_of_vanes);
	void triangulated_cycle(const bool pairwise);

	void add_edge(const int v1, const int v2, const bool pairwise, const bool recompute_dist_and_edge_matrix, const int new_edge_cost=-1); //if the edge weight should be computed, then use the default value -1
	void remove_edge(const int v1, const int v2, const bool pairwise, const bool recompute_dist_and_edge_matrix);
	void swap_edge(const int agent, const int v_from, const int v_to, const bool recompute_distance_matrix, const int new_edge_cost=-1);
	void set_strategy(const int agent, const std::set<int>& strategy, const bool pairwise, const bool recompute_distance_matrix);
	void perform_step(const int agent, const Step& step, const bool pairwise, const bool _recompute_distance_matrix);
	
	std::vector<int> get_edge_weights(const int node) const;
	
	std::vector<int> edge_weights_after_1_step(const Step& step, const int source) const;
};
