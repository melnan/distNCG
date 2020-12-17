
#include "graph.h"
#include"definitions.h"

Graph::Graph(const unsigned int  _num_of_nodes, const bool _storage_distances, const int _infty) : adjacency_matrix_int(_num_of_nodes), num_of_nodes(_num_of_nodes), players_strategy(_num_of_nodes), storage_distances(_storage_distances), dist_matrix(_num_of_nodes), infty(_infty){
	if (storage_distances) {
		for (int i = 0; i < num_of_nodes; i++) {
			dist_matrix[i].resize(num_of_nodes, _infty);
			dist_matrix[i][i] = 0;
		}

	}
};

Graph::Graph(const Graph& _graph) : adjacency_matrix_int(_graph.adjacency_matrix_int), num_of_nodes(_graph.num_of_nodes), players_strategy(std::move(_graph.players_strategy)), storage_distances(_graph.storage_distances), dist_matrix(std::move(_graph.dist_matrix)), infty(_graph.infty) {};


Graph::Graph(const unsigned int number_of_nodes, const std::string input_file_name, const bool pairwise, const bool _storage_distances, const int _infty) : adjacency_matrix_int(number_of_nodes), num_of_nodes(number_of_nodes), players_strategy(number_of_nodes), storage_distances(_storage_distances), dist_matrix(number_of_nodes), infty(_infty){
	std::ifstream in_file(input_file_name);
	std::string _line;
	std::getline(in_file, _line); //skip the first line
	//std::getline(in_file, _line); //skip the second line

	std::smatch sm;
	std::regex regex_numbers("[0-9]+");

	if (in_file.is_open()){
		//read list of adjacents
		//for (auto i = 0; i < number_of_nodes; i++){
		std::getline(in_file, _line);
			while (!in_file.eof()){
				
				//read parent node first
				std::regex_search(_line, sm, regex_numbers);
				int parent_node = std::stoi(sm.str());
				_line = sm.suffix();

				while (std::regex_search(_line, sm, regex_numbers)){
					int node = std::stoi(sm.str());
					add_edge(parent_node, node, pairwise, 0);
					_line = sm.suffix();
				}
				std::getline(in_file, _line);
			}
		//}
	}
	else
	{
		std::cout << "Input file cann't be open \n";
		exit(0);
	}

	if (storage_distances) {
		for (int i = 0; i < num_of_nodes; i++) {
			dist_matrix[i].resize(num_of_nodes, _infty);
			dist_matrix[i] = vector_of_hop_dist(i);
			dist_matrix[i][i] = 0;
		}
	}
};

//Graph::~Graph(){};

void Graph::path_graph(const bool pairwise) {
	for (size_t i = 0; i < num_of_nodes-1; i++)
		add_edge(i, i + 1, pairwise, 0);
	
	if (storage_distances) {
		recompute_distances();
	}
};

void Graph::cycle_graph(const bool pairwise) {
	path_graph(pairwise);
	add_edge(num_of_nodes - 1, 0, pairwise, 0);

	if (storage_distances) {
		recompute_distances();
	}
};

void Graph::tree_graph(const bool pairwise, const int init_diam){
	//generate a path of random right and connect other nodes sequentially
	int hight = init_diam;//rand()%num_of_nodes+1;

	for (int i = 0; i < hight; i++)
		add_edge(i, i + 1, pairwise, 0);

	for (int i = hight+1; i < num_of_nodes; i++){
		int connect_to = rand() % i;
		add_edge(i, connect_to, pairwise, 0);
	}

	if (storage_distances) {
		recompute_distances();
	}
}


void Graph::triangulated_tree_graph(const bool pairwise){
	int hight = rand() % (num_of_nodes/2) + 1;
	int i;
	for (i = 0; i < hight; i+=2){
		add_edge(i, i + 1, pairwise, 0);
		add_edge(i + 1, i + 2, pairwise, 0);
		add_edge(i + 2, i, pairwise, 0);
	}

	for (int j = i+1; j < num_of_nodes-1; j+=2){
		int connect_to = rand() % j;
		add_edge(j, connect_to, pairwise, 0);
		add_edge(j+1, connect_to, pairwise, 0);
		add_edge(j, j + 1, pairwise, 0);
	}

	if (storage_distances) {
		recompute_distances();
	}
}

void Graph::complete_graph(const bool pairwise){
	for (int i = 0; i < num_of_nodes; i++)
		for (int j = i+1; j < num_of_nodes; j++){
			if (pairwise) add_edge(i, j, pairwise, 0);
			else{
				int rand_seed = rand() % 2;
				if (rand_seed == 0) add_edge(i, j, pairwise, 0);
				else add_edge(j, i, pairwise, 0);
			}
			dist_matrix[i][j] = 1;
		}


}

void Graph::triangulated_path_graph(const bool pairwise)
{
	for (int i = 0; i < num_of_nodes-1; i += 2)
	{
		add_edge(i, i + 1, pairwise, 0);
		if (i + 2 < num_of_nodes) {
			add_edge(i, i + 2, pairwise, 0);
			add_edge(i + 1, i + 2, pairwise, 0);
		}
	}

	if (storage_distances) {
		recompute_distances();
	}
}

void Graph::triangulated_star(const bool pairwise, const int length_of_vane, const int num_of_vanes) {
	for (int j = 0; j < num_of_vanes; j++)
	{
		for (int i = 0; i < 2 * length_of_vane-2; i += 2)
		{
			add_edge(2*length_of_vane*j + i, 2 * length_of_vane*j + i + 1, pairwise, 0);
			add_edge(j * length_of_vane * 2+ i + 1, j * length_of_vane * 2+ i + 2, pairwise, 0);
			add_edge(j * length_of_vane *2 + i, j * length_of_vane * 2+ i + 2, pairwise, 0);
		}
		add_edge(2 * length_of_vane * (j ) + 2*length_of_vane-2, 2 * length_of_vane * (j) +2 * length_of_vane - 1, pairwise, 0);
		add_edge(2 * length_of_vane * (j )+2 * length_of_vane - 2, num_of_nodes - 1, pairwise, 0);
		add_edge(2 * length_of_vane * (j )+2 * length_of_vane - 1, num_of_nodes - 1, pairwise, 0);
	}

	if (storage_distances) {
		recompute_distances();
	}
}

int Graph::graph_num_of_nodes() const{
	return num_of_nodes;
};


 int Graph::num_of_edges() const{
	 int result = 0;
	for (auto i = 0; i < num_of_nodes; i++)
		result += adjacency_matrix_int[i].size();
	return result/2;
}

 int Graph::hop_distance(const int n1, const int n2) const
 {
	 if(this->storage_distances)
		return dist_matrix[n1][n2];
	 else
		return  vector_of_hop_dist(n1, n2)[n2];
 }

 
 double Graph::avg_clustering() const{
	 double result = 0.0;

	 for (int i = 0; i < num_of_nodes; i++)
		 result += local_clustering(i);

	 result /= num_of_nodes;

	 return result;
 }

 bool Graph::storage_dist() const
 {
	 if (this->storage_distances) return true;
	 else return false;
 }

 int Graph::diameter() const
 {
	 int max_dist = 0;
	 if (storage_distances)
	 {
		 for (int i = 0; i < num_of_nodes; i++)
			 for (int j = i + 1; j < num_of_nodes; j++)
				 if (dist_matrix[i][j] > max_dist)
					 max_dist = dist_matrix[i][j];
	 }
	 else //then have to compute all dist
	 {
		 for (int i = 0; i < num_of_nodes; i++)
		 {
			 std::vector<int> dist = get_vector_of_dist(i);
			 for (int j = i + 1; j < num_of_nodes; j++)
			 {
				 if (dist[j] > max_dist)
					 max_dist = dist[j];
			 }
		 }
	 }
	 return max_dist;
 }

const std::set<int> Graph::strategy_of_player(const int player) const{
	return players_strategy[player];
}

unsigned int Graph::node_degree(const int node) const{		//undirected
	return adjacency_matrix_int[node].size();
}

void Graph::add_edge(const int v1, const int v2, const bool pairwise, const bool _recompute_distance_matrix){
	//if (v1<num_of_nodes && players_strategy[v1].count(v2) == 0){ // we do not insert the same edge again, only the reverse edge cannot be added
		adjacency_matrix_int[v1].insert(v2);
		adjacency_matrix_int[v2].insert(v1);

		players_strategy[v1].insert(v2);
		if (pairwise) 
			players_strategy[v2].insert(v1);

		if (_recompute_distance_matrix){
			//for(int node =0 ;node<num_of_nodes;node++)
			//{
			//	if(node==v1 || distance(v2, node)+1<distance)
			//}
			recompute_distances();
	}
}


void Graph::remove_edge(const int v1, const int v2, const bool pairwise, const bool _recompute_distance_matrix){
	if (v1<num_of_nodes && players_strategy[v1].count(v2) > 0)//if the edge exists
	{
		players_strategy[v1].erase(v2);

		if (players_strategy[v2].count(v1) == 0 || pairwise) {
			adjacency_matrix_int[v1].erase(v2);
			adjacency_matrix_int[v2].erase(v1);
		}
			if (pairwise) players_strategy[v2].erase(v1);
	}

	if (_recompute_distance_matrix)
		recompute_distances();
}



void Graph::swap_edge(const int agent, const int v_from, const int v_to, const bool _recompute_distance_matrix){
	if (agent<num_of_nodes && 
		v_from<num_of_nodes &&
		v_to < num_of_nodes &&
		players_strategy[agent].count(v_from)> 0 && //if the edge exists
		adjacency_matrix_int[agent].find(v_to) ==adjacency_matrix_int[agent].end()) //new edge does not exist
	{
		players_strategy[agent].erase(v_from);
		players_strategy[agent].insert(v_to);

		adjacency_matrix_int[agent].erase(v_from);
		adjacency_matrix_int[v_from].erase(agent);

		adjacency_matrix_int[agent].insert(v_to);
		adjacency_matrix_int[v_to].insert(agent);
	}

	if (_recompute_distance_matrix)
		recompute_distances();
}

void Graph::perform_step(const int agent, const Step &step, const bool pairwise, const bool _recompute_distance_matrix) {
	switch (step.step_name)
	{
	case 'a':
	{
		add_edge(agent, step.to, pairwise, _recompute_distance_matrix);
		break;
	}
	case 'd':
	{
		remove_edge(agent, step.from, pairwise, _recompute_distance_matrix);
		break;
	}
	case 's':
	{
		if(!pairwise)	swap_edge(agent, step.from, step.to, _recompute_distance_matrix);
		break;
	}
	default:
		break;
	}
}

//std::vector<int> Graph::dist_after_edge_v1v2_removal(const int v1, const int v2, const int destination) const {
//	std::vector<int> dist_vector(num_of_nodes, infty);
//	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 
//
//	bfs_queue.push_back(v1);
//	dist_vector[v1] = 0;
//
//	size_t iter_to_curr_node = 0;
//
//	while (iter_to_curr_node != bfs_queue.size()) //while do not reach end of the queue
//	{
//		int curr_node = bfs_queue[iter_to_curr_node];
//
//		if (curr_node == destination) //if we need dist to the destination only, then stop and return the dest_vector
//			return dist_vector;
//
//		for (const auto& neighb : adjacency_matrix_int[curr_node]) {
//			if (dist_vector[neighb] == infty && !(neighb == v2 && curr_node == v1) && !(neighb == v1 && curr_node == v2)) {
//				dist_vector[neighb] = dist_vector[curr_node] + 1;
//				//if (dist_vector[neighb] > max_radius)	//if max_radius is reached, return only list of nodes within the distance <= max_radius
//				//	return bfs_queue;
//				bfs_queue.push_back(neighb);
//			}
//		}
//		iter_to_curr_node++;
//	}
//
//	return dist_vector;
//}

std::vector<int> Graph::distances_from_node_after_removed_edges(const int node, const std::set<std::set<int>>& set_of_removed_edges, const int destination) const {
	std::vector<int> dist_vector(num_of_nodes, infty);
	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 

	bfs_queue.push_back(node);
	dist_vector[node] = 0;

	size_t iter_to_curr_node = 0;

	while (iter_to_curr_node != bfs_queue.size()) //while do not reach end of the queue
	{
		int curr_node = bfs_queue[iter_to_curr_node];

		if (curr_node == destination) //if we need dist to the destination only, then stop and return the dest_vector
			return dist_vector;

		for (const auto& neighb : adjacency_matrix_int[curr_node]) {
			std::set<int> edge = { neighb, curr_node };
			if (dist_vector[neighb] == infty && (set_of_removed_edges.find(edge)==set_of_removed_edges.end())) {
				dist_vector[neighb] = dist_vector[curr_node] + 1;
				//if (dist_vector[neighb] > max_radius)	//if max_radius is reached, return only list of nodes within the distance <= max_radius
				//	return bfs_queue;
				bfs_queue.push_back(neighb);
			}
		}
		iter_to_curr_node++;
	}

	return dist_vector;
}

std::vector<int> Graph::distances_from_node_after_edge_swap(const int v_source, const int v_from, const int v_to, const int destination) const {
	std::vector<int> dist_vector(num_of_nodes, infty);
	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 
	std::vector<std::set<int>> copy_of_adj_matrix = adjacency_matrix_int;

	//do smth better than this
	copy_of_adj_matrix[v_source].erase(v_from);
	copy_of_adj_matrix[v_from].erase(v_source);
	
	copy_of_adj_matrix[v_source].insert(v_to);
	copy_of_adj_matrix[v_to].insert(v_source);

	bfs_queue.push_back(v_source);
	dist_vector[v_source] = 0;

	size_t iter_to_curr_node = 0;

	while (iter_to_curr_node != bfs_queue.size()) //while do not reach end of the queue
	{
		int curr_node = bfs_queue[iter_to_curr_node];

		if (curr_node == destination) //if we need dist to the destination only, then stop and return the dest_vector
			return dist_vector;

		for (const auto& neighb : copy_of_adj_matrix[curr_node]) {
			if (dist_vector[neighb] == infty &&
				(!(neighb == v_from && curr_node == v_source) && !(neighb == v_source && curr_node == v_from) 
					|| (curr_node == v_source && neighb == v_to) || (curr_node == v_to && neighb == v_source))) {
				dist_vector[neighb] = dist_vector[curr_node] + 1;
				//if (dist_vector[neighb] > max_radius)	//if max_radius is reached, return only list of nodes within the distance <= max_radius
				//	return bfs_queue;
				bfs_queue.push_back(neighb);
			}
		}
		iter_to_curr_node++;
	}

	return dist_vector;
}

bool Graph::check_if_step_is_allowed(const Step& step, const int source) const {
	switch (step.step_name)
	{
	case 'a':
	{
		if (players_strategy[source].find(step.to) == players_strategy[source].end())
			return true;
		break;
	}
	case 'd':
	{
		if (players_strategy[source].find(step.from) != players_strategy[source].end())
			return true;
		break;
	}
	case 's':
	{
		if (players_strategy[source].find(step.to) == players_strategy[source].end() 
			&& players_strategy[source].find(step.from) != players_strategy[source].end())
			return true; 
		break;
	}
	default:
		std::cout << "Incorrect step name. Error in graph.check_if_step_is_allowed() method";
		exit(0);
	}

	return false;
}
std::vector<int> Graph::distances_from_node_after_1_step(const Step &step, const int source, const int destination ) const
//returns vestor of dist to all nodes from source
{
	if (!check_if_step_is_allowed(step, source))
		return get_vector_of_dist(source);

	switch (step.step_name)
	{
	case 'a':
	{
		std::vector<int> curr_dist = get_vector_of_dist(source);
		std::vector<int> result = get_vector_of_dist(source);

		for (int i = 0; i < num_of_nodes; i++)
		{
			int new_dist = 1 + std::min(dist_betw_nodes(i, step.to), dist_betw_nodes(i, source));
			if (curr_dist[i] > new_dist)
				result[i] = new_dist;
		}
		return result;
		break;
	}
	case 'd':
	{
		//return dist_after_edge_v1v2_removal(source, step.from, destination);
			return distances_from_node_after_removed_edges(source, { {source, step.from} });
		break;
	}
	case 's':
	{
		return distances_from_node_after_edge_swap(source, step.from, step.to);
		break;
	}
	default: {
		std::cout << "Error in graph.distances_from_node_after_1_step. Incorrect step name.";
		exit(0);
		break; 
	}
	}	
}

int Graph::infty_value() const
{
	return this->infty;
}

void Graph::set_strategy(const int agent, const std::set<int> &strategy, const bool pairwise, const bool _recompute_distance_matrix) {
	//better: find intersection of two strategies, then remove/add strategy-intersection/strategy+intersection
	std::vector<int> edges_to_add;
	std::vector<int> edges_to_del;

	std::set_difference(strategy.begin(), strategy.end(), players_strategy[agent].begin(), players_strategy[agent].end(), std::back_inserter(edges_to_add));
	std::set_difference(players_strategy[agent].begin(), players_strategy[agent].end(), strategy.begin(), strategy.end(), std::back_inserter(edges_to_del));

	for (const auto &i : edges_to_add){
		add_edge(agent, i, pairwise, 0);
	}

	for (const auto &i : edges_to_del)
		remove_edge(agent, i, pairwise, 0);

	if (_recompute_distance_matrix)
		recompute_distances();
}

void Graph::recompute_distances() {
	for (int i = 0; i < this->num_of_nodes; i++)
	{
		this->dist_matrix[i] = vector_of_hop_dist(i);
	}
}

std::vector<int> Graph::vector_of_hop_dist(const int source, const int destination) const{
	std::vector<int> dist_vector(num_of_nodes, infty);
	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 

	bfs_queue.push_back(source);	
	dist_vector[source] = 0;

	size_t iter_to_curr_node = 0;

	while (iter_to_curr_node!=bfs_queue.size()) //while do not reach end of the queue
	{
		int curr_node = bfs_queue[iter_to_curr_node];

		if (curr_node == destination) //if we need dist to the destination only, then stop and return the dest_vector
			return dist_vector;

		for (const auto &neighb : adjacency_matrix_int[curr_node]){
			if (dist_vector[neighb] == infty){
				dist_vector[neighb] = dist_vector[curr_node] + 1;
				//if (dist_vector[neighb] > max_radius)	//if max_radius is reached, return only list of nodes within the distance <= max_radius
				//	return bfs_queue;
				bfs_queue.push_back(neighb);
			}
		}		
		iter_to_curr_node++;
	}

	return dist_vector;
}

std::vector<std::vector<int>> Graph::get_dist_matrix() const
{
	if(storage_distances)
	return this->dist_matrix;
	else {
		std::vector<std::vector<int>> result (num_of_nodes, std::vector<int>(num_of_nodes));
		for (int i = 0; i < num_of_nodes; i++)
			result[i] = vector_of_dist_weighted(i);
		return result;
	}
}

std::vector<int> Graph::get_vector_of_dist(const int agent) const
{
	if (storage_distances)
		return dist_matrix[agent];
	else
		return vector_of_dist_weighted(agent);
}

std::vector<int> Graph::vector_of_dist_weighted(const int source, const int destination) const
{
	return vector_of_hop_dist(source, destination);
}

int Graph::dist_betw_nodes(const int n1, const int n2) const
{
	return hop_distance(n1, n2);
}

std::set<int> Graph::k_neighborhood(const int node, const int k)const {
	std::vector<int> dist_vector(num_of_nodes, -1);
	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 

	bfs_queue.push_back(node);
	dist_vector[node] = 0;

	size_t iter_to_curr_node = 0;
	int curr_dist = 0;
	while (iter_to_curr_node != bfs_queue.size()) //while do not reach end of the queue
	{
		int curr_node = bfs_queue[iter_to_curr_node];
		
	
		for (const auto &neighb : adjacency_matrix_int[curr_node]){
			if (dist_vector[neighb] == -1){
				dist_vector[neighb] = dist_vector[curr_node] + 1;
				if (dist_vector[neighb] > k)	//if max_radius is reached, return only list of nodes within the distance <= max_radius
					return std::set<int>(bfs_queue.begin(), bfs_queue.end());
				bfs_queue.push_back(neighb);
			}
		}
		iter_to_curr_node++;
	}

	return std::set<int>(bfs_queue.begin(), bfs_queue.end());
}

bool Graph::nodes_are_connected(int v1, int v2) const{
	return (adjacency_matrix_int[v1].find(v2) != adjacency_matrix_int[v1].end());
}

std::set<int> Graph::neighborhood(const int node) const{
	return adjacency_matrix_int[node];
}

double Graph::local_clustering(const int node) const{
	if (node_degree(node) == 1)
		return 0.0;

	double result = 0.0;
	auto neighb = neighborhood(node);
	
	for (const auto i : neighb){
		std::set<int> neighb_of_neighb = neighborhood(i);
		std::vector<int> intersection(num_of_nodes);
		auto it = std::set_intersection(neighb.begin(), neighb.end(), neighb_of_neighb.begin(), neighb_of_neighb.end(), intersection.begin());
		intersection.resize(it - intersection.begin());
		result += intersection.size();
	}

	result = result / (static_cast <double>(neighb.size()) * (static_cast<double>(neighb.size() - 1)));
	return result;
}

//output
void Graph::output_graph_to_dot_file(const std::string &file_name) const{
	std::ofstream out_file(file_name);
	std::string endpoints="";

	if (out_file.is_open()){
		out_file << "strict digraph {\n";

		for (auto i = 0; i < num_of_nodes; i++){
			if (players_strategy[i].size() > 0){
				out_file << i << " -> ";

				for (const auto &iter : players_strategy[i]){
					endpoints += (std::to_string(iter) + "; ");
				}
				endpoints.resize(endpoints.size() - 2);
				if (players_strategy[i].size() > 1)
					endpoints = "{" + endpoints + "}";

				out_file << endpoints << ";\n";
				endpoints.clear();
			}
		}
		out_file << "}";
		out_file.close();
	}
	else{
		std::cout << "Output file cann't be open \n";
		exit(0);
	}
}

void Graph::output_graph_to_gexf_file(const std::string& file_name) const {
	std::ofstream out_file(file_name);
	std::string endpoints = "";

	if (out_file.is_open()) {
		out_file << "<gexf xmlns=\"http://www.gexf.net/my_draft\" version=\"1.2\">\n";
		out_file << "	<graph mode=\"static\" defaultedgetype=\"undirected\">\"\n";
		out_file << "		<nodes>\n";
		//output nodes
		for (auto i = 0; i < num_of_nodes; i++)
		{
			out_file << "			<node id=\"" << i << "\" label=\"" << i << "\" />\n";
		}
		out_file << "		</nodes>\n";
		out_file << "		<edges>\n";
		int edge_counter=0;
		for (auto i = 0; i < num_of_nodes; i++) {
			if (players_strategy[i].size() > 0) {
				for (const auto& iter : players_strategy[i]) {
					out_file << "			<edge id=\"" << edge_counter << "\" source=\"" << i << "\" target=\"" << std::to_string(iter) << "\" />\n";
					edge_counter++;
				}
			}
		}
		out_file << "		</edges>\n";
		out_file << "	</graph>\n";
		out_file << "</gexf>\n";
		out_file.close();
	}
	else {
		std::cout << "Output file cann't be open \n";
		exit(0);
	}
}


//******************************* Dynamic_W_graph  ********************************************

Dynamic_W_graph::Dynamic_W_graph(const int _num_of_nodes, const int _infty) : Graph(_num_of_nodes, 1, _infty), edge_weights(_num_of_nodes, std::vector<int>(_num_of_nodes, 0)) {
	//infty = _num_of_nodes;
};

Dynamic_W_graph::Dynamic_W_graph(const int _num_of_nodes, const std::string& _input_file_name, const bool _pairwise, const int _infty) : Graph(_num_of_nodes, _input_file_name, _pairwise, 1, _infty), edge_weights(_num_of_nodes, std::vector<int>(_num_of_nodes, 0)) {
	//infty = _num_of_nodes;
	recompute_all_edge_weights(_pairwise);

};

void Dynamic_W_graph::recompute_all_edge_weights(const bool _pairwise) {
	for (int i = 0; i < num_of_nodes; i++)
	{
		edge_weights[i].assign(this->num_of_nodes, 0);
		
		for (const auto endpoint : players_strategy[i]) {
			if (!_pairwise)	edge_weights[i][endpoint] = distances_from_node_after_removed_edges(i, { {i, endpoint} })[endpoint];
			else if (endpoint < i) edge_weights[endpoint][i] = edge_weights[i][endpoint] = distances_from_node_after_removed_edges(i, { {i, endpoint} })[endpoint];
		}
	}
}



void Dynamic_W_graph::path_graph(const bool pairwise) {
	for (size_t i = 0; i < num_of_nodes - 1; i++) {
		add_edge(i, i + 1, pairwise, 0, num_of_nodes);
	}
	for(int i=0;i<num_of_nodes;i++)
		for (int j = i+1; j < num_of_nodes; j++) {
			dist_matrix[i][j] = dist_matrix[j][i]= j-i;
		}
};

void Dynamic_W_graph::cycle_graph(const bool pairwise) {
	for (size_t i = 0; i < num_of_nodes - 1; i++) 
		add_edge(i, i + 1, pairwise, 0, num_of_nodes - 1);
	
	add_edge(num_of_nodes - 1, 0, pairwise, 1, num_of_nodes - 1);
	
	recompute_distances();
};

void Dynamic_W_graph::tree_graph(const bool pairwise, const int init_diam) {
	//generate a path of random hight and connect other nodes sequentially
	int hight = init_diam; // rand() % num_of_nodes + 1;

	for (int i = 0; i < hight; i++) {
		if(i<num_of_nodes-1)
		add_edge(i, i + 1, pairwise, 0, num_of_nodes);
	}

	for (int i = hight + 1; i < num_of_nodes; i++) {
		int connect_to = rand() % i;
		add_edge(i, connect_to, pairwise, 0, num_of_nodes);
	}

	recompute_distances();
}


void Dynamic_W_graph::triangulated_tree_graph(const bool pairwise) {
	int hight = rand() % (num_of_nodes / 2) + 1;
	int i;
	for (i = 0; i < hight; i += 2) {
		add_edge(i, i + 1, pairwise, 0, 2);
		add_edge(i + 1, i + 2, pairwise, 0, 2);
		add_edge(i + 2, i, pairwise, 0, 2);
	}

	for (int j = i + 1; j < num_of_nodes - 1; j += 2) {
		int connect_to = rand() % j;
		add_edge(j, connect_to, pairwise, 0, 2);
		add_edge(j + 1, connect_to, pairwise, 0, 2);
		add_edge(j, j + 1, pairwise, 0, 2);
	}

	recompute_distances();
}

void Dynamic_W_graph::complete_graph(const bool pairwise) {
	for (int i = 0; i < num_of_nodes; i++)
		for (int j = i + 1; j < num_of_nodes; j++) {
			if (pairwise) add_edge(i, j, pairwise, 0, 2);
			else {
				int rand_seed = rand() % 2;
				if (rand_seed == 0) add_edge(i, j, pairwise, 0, 2);
				else add_edge(j, i, pairwise, 0, 2);
			}
			dist_matrix[i][j] = dist_matrix[j][i] = 1;
		}


}

void Dynamic_W_graph::triangulated_path_graph(const bool pairwise)
{
	for (int i = 0; i < num_of_nodes - 1; i += 2)
	{
		
		if (i + 2 < num_of_nodes) {
			add_edge(i, i + 1, pairwise, 0, 2);
			add_edge(i, i + 2, pairwise, 0, 2);
			add_edge(i + 1, i + 2, pairwise, 0, 2);
		} 
		else
			add_edge(i, i + 1, pairwise, 0, num_of_nodes);
	}

	recompute_distances();
}

void Dynamic_W_graph::triangulated_star(const bool pairwise, const int length_of_vane, const int num_of_vanes) {
	for (int j = 0; j < num_of_vanes; j++)
	{
		for (int i = 0; i < 2 * length_of_vane - 2; i += 2)
		{
			add_edge(2 * length_of_vane * j + i, 2 * length_of_vane * j + i + 1, pairwise, 0, 2);
			add_edge(j * length_of_vane * 2 + i + 1, j * length_of_vane * 2 + i + 2, pairwise, 0, 2);
			add_edge(j * length_of_vane * 2 + i, j * length_of_vane * 2 + i + 2, pairwise, 0, 2);
		}
		add_edge(2 * length_of_vane * (j)+2 * length_of_vane - 2, 2 * length_of_vane * (j)+2 * length_of_vane - 1, pairwise, 0, 2);
		add_edge(2 * length_of_vane * (j)+2 * length_of_vane - 2, num_of_nodes - 1, pairwise, 0, 2);
		add_edge(2 * length_of_vane * (j)+2 * length_of_vane - 1, num_of_nodes - 1, pairwise, 0, 2);
	}

	recompute_distances();
}

void Dynamic_W_graph::triangulated_cycle(const bool pairwise)
{
	for (int i = 0; i < num_of_nodes - 2; i += 2)
	{

		if (i + 2 < num_of_nodes) {
			add_edge(i, i + 1, pairwise, 0, 2);
			add_edge(i, i + 2, pairwise, 0, 2);
			add_edge(i + 1, i + 2, pairwise, 0, 2);
		}
		else
			add_edge(i, i + 1, pairwise, 0, 2);
	}
	add_edge(num_of_nodes - 1, 0, pairwise, 0, 2);
	add_edge(num_of_nodes - 1, num_of_nodes-3, pairwise, 0, 2);
	add_edge(num_of_nodes - 3, 0, pairwise, 0, 2);
	recompute_distances();
}

void Dynamic_W_graph::add_edge(const int v1, const int v2, const bool pairwise, const bool recompute_dist_and_edge_matrix, const int new_edge_cost) {
	if (recompute_dist_and_edge_matrix && dist_matrix[v1][v2] != this->infty && new_edge_cost != this->infty) //if new edge is not a bridge
	{//recompute weights of other edges
		for (int i = 0; i < num_of_nodes; i++)
			for (const auto i_neighb : players_strategy[i])
			{
				if (!pairwise || (i_neighb > i)) {
					if (edge_weights[i][i_neighb] == this->infty) {
						int tmp = std::min(dist_matrix[i][v1] + dist_matrix[i_neighb][v2], dist_matrix[i][v2] + dist_matrix[i_neighb][v1]);
						if (tmp == dist_matrix[v1][v2] - 1) //if the edge is a bridge and is in the new cycle
							edge_weights[i][i_neighb] = 1 + tmp;
					}
					else {
						int tmp = std::min(dist_matrix[i][v1] + dist_matrix[i_neighb][v2], dist_matrix[i][v2] + dist_matrix[i_neighb][v1]);
						if (edge_weights[i][i_neighb] > 1 + tmp)
							edge_weights[i][i_neighb] = dist_from_node_after_edge_modifications(i, { std::make_pair(v1,v2) }, { std::make_pair(i, i_neighb) }, i_neighb)[i_neighb];
					}
				}
				else if (i_neighb < i)
					edge_weights[i][i_neighb] = edge_weights[i_neighb][i];
			}
	}
	Graph::add_edge(v1, v2, pairwise, 0);
	if (new_edge_cost != -1) {
		edge_weights[v1][v2] = new_edge_cost;
	}
	else //compute_edge_weight
	{
		edge_weights[v1][v2] = dist_matrix[v1][v2];
	}

	if (pairwise)
		edge_weights[v2][v1] = edge_weights[v1][v2];

	if (recompute_dist_and_edge_matrix) {
		std::vector<std::vector<int>> tmp_dist_matrix(dist_matrix); //keep old distances here
		for (int i = 0; i < num_of_nodes; i++)
			for (int j = i + 1; j < num_of_nodes; j++) {
				int min_dist_tmp = std::min(tmp_dist_matrix[i][v2] + tmp_dist_matrix[j][v1], tmp_dist_matrix[i][v1] + tmp_dist_matrix[j][v2]);
				if (tmp_dist_matrix[i][j] > 1 + min_dist_tmp)
					dist_matrix[i][j] = dist_matrix[j][i] = min_dist_tmp+1;
			}
	}

}

void Dynamic_W_graph::remove_edge(const int v1, const int v2, const bool pairwise, const bool _recompute_dist_and_edge_matrix) {
	Graph::remove_edge(v1, v2, pairwise, 0);
	
	if (_recompute_dist_and_edge_matrix) {
		recompute_distances();

		if(edge_weights[v1][v2] != this->infty)
			for (int i = 0; i < num_of_nodes; i++) {
				for (const auto i_neighb : players_strategy[i]) {
					if (!pairwise || (i_neighb > i)) {
						if(edge_weights[i][i_neighb] != this->infty)
							if (edge_weights[i][i_neighb] >= edge_weights[v1][v2]) //edge weight can change only if its original weight was equal to the edge weight of the deleted edge
							{
								edge_weights[i][i_neighb] = distances_from_node_after_removed_edges(i, { {i, i_neighb} }, i_neighb)[i_neighb];//dist_after_edge_v1v2_removal(i, i_neighb)[i_neighb];
							}
					}
					else if (i_neighb < i)
						edge_weights[i][i_neighb] = edge_weights[i_neighb][i];
				}
			}
	}
	edge_weights[v1][v2] = 0;
	if (pairwise) edge_weights[v2][v1] = 0;
}

void Dynamic_W_graph::swap_edge(const int agent, const int v_from, const int v_to, const bool _recompute_dist_and_edge_matrix, const int new_edge_cost) {
	Graph::swap_edge(agent, v_from, v_to, _recompute_dist_and_edge_matrix);
	
	if (_recompute_dist_and_edge_matrix) {
		recompute_all_edge_weights(0);
		recompute_distances();
	}
}

void Dynamic_W_graph::set_strategy(const int agent, const std::set<int>& strategy, const bool pairwise, const bool recompute_distance_matrix) {
	Graph::set_strategy(agent, strategy, pairwise, recompute_distance_matrix);
	recompute_all_edge_weights(pairwise);
}

void Dynamic_W_graph::perform_step(const int agent, const Step& step, const bool pairwise, const bool _recompute_distance_matrix)
{
	switch (step.step_name)
	{
	case 'a':
	{
		add_edge(agent, step.to, pairwise, _recompute_distance_matrix);
		break;
	}
	case 'd':
	{
		remove_edge(agent, step.from, pairwise, _recompute_distance_matrix);
		break;
	}
	case 's':
	{
		if (!pairwise)	swap_edge(agent, step.from, step.to, _recompute_distance_matrix);
		break;
	}
	default:
		break;
	}
}

std::vector<int> Dynamic_W_graph::get_edge_weights(const int node) const {
	return edge_weights[node];
}


std::vector<int> Dynamic_W_graph::dist_from_node_after_edge_modifications(const int node, const std::set<std::pair<int, int>>& added_edges, const std::set<std::pair<int, int>>& deleted_edges, const int destination ) const {
	std::vector<int> dist_vector(num_of_nodes, infty);
	std::vector<int> bfs_queue; //vector (not a queue) because we save time if don't delete from a queue 
	std::vector<std::set<int>> copy_of_adj_matrix = adjacency_matrix_int;

	
	for (auto& del : deleted_edges) {
		copy_of_adj_matrix[del.first].erase(del.second);
		copy_of_adj_matrix[del.second].erase(del.first);
	}
	for (auto& add : added_edges) {
		copy_of_adj_matrix[add.first].insert(add.second);
		copy_of_adj_matrix[add.second].insert(add.first);
	}

	bfs_queue.push_back(node);
	dist_vector[node] = 0;

	size_t iter_to_curr_node = 0;

	while (iter_to_curr_node != bfs_queue.size()) //while do not reach end of the queue
	{
		int curr_node = bfs_queue[iter_to_curr_node];

		if (curr_node == destination) //if we need dist to the destination only, then stop and return the dest_vector
			return dist_vector;

		for (const auto& neighb : copy_of_adj_matrix[curr_node]) {
			if (dist_vector[neighb] == infty) {
				dist_vector[neighb] = dist_vector[curr_node] + 1;
				bfs_queue.push_back(neighb);
			}
		}
		iter_to_curr_node++;
	}

	return dist_vector;
}

std::vector<int> Dynamic_W_graph::edge_weights_after_1_step(const Step& step, const int source) const {
	if (!check_if_step_is_allowed(step, source))
		return get_edge_weights(source);

	switch (step.step_name)
	{
	case 'a':
	{
		std::vector<int> result(edge_weights[source]);
		result[step.to] = dist_matrix[source][step.to];

		if (dist_matrix[source][step.to] == this->infty)
			return result;

		for (const auto& i_neighb : players_strategy[source])
		{
			if (edge_weights[source][i_neighb] == this->infty)
			{//if the edge was a bridge and now closed in a cycle, than it is the only one which e-cost changes. 
				if (dist_matrix[step.to][i_neighb] < 1 + dist_matrix[source][step.to])
				{
					result[i_neighb] = 1 + dist_matrix[step.to][i_neighb];
					return result;
				}
				//if the edge is a bridge and is not in the new cycle, than its e-cost remains unchanged
			}
			else 
			{
				if (result[i_neighb] > dist_matrix[step.to][i_neighb] + 1)//if edge_price can potentialy decrease, than recompute its price
					result[i_neighb] = distances_from_node_after_edge_swap(source, i_neighb, step.to, i_neighb)[i_neighb];
			}
		}
		
		
		return result;
	}
	case 'd':
	{
		std::vector<int> result(edge_weights[source]);
		result[step.from] = 0;

		if (edge_weights[source][step.from] == this->infty)
			return result;

		for (const auto& i_neighb : players_strategy[source])
		{
			if(edge_weights[source][i_neighb] != this->infty)//if the edge was a bridge, its cost remains unchanged
				if (edge_weights[source][i_neighb] >= edge_weights[source][step.from] && i_neighb != step.from) //edge weight can change only if its original weight was larger or equal to the edge weight of the deleted edge
				{
					result[i_neighb] = distances_from_node_after_removed_edges(source, { {source, step.from}, {source, i_neighb} }, i_neighb)[i_neighb];
				}
		}
		
		return result;
	}
	case 's':
	{
		std::vector<int> result(edge_weights[source]);
		for (const auto neighb : players_strategy[source])
		{
			if (neighb != step.from)
				result[neighb] = dist_from_node_after_edge_modifications(source, { std::make_pair(source, step.to) }, { std::make_pair(source, step.from), std::make_pair(source, neighb) }, neighb)[neighb];
		}
		result[step.to] = dist_matrix[source][step.to];
		result[step.from] = 0;

		return result;
	}
	default:
		break;
	}
}