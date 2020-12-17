#pragma once
#include "graph.h"
#include "definitions.h"





template<typename Graph_type, typename cost_f_type>
class Game {
public:
	//constructors
	explicit Game(const Graph_type& input_graph, cost_f_type _alpha, std::set<char>& _list_of_prohib_moves, bool _bilateral) : graph(input_graph), alpha(_alpha), radius_of_steps(graph.graph_num_of_nodes()), list_of_prohib_moves(_list_of_prohib_moves), bilateral(_bilateral)
	{
		infty = static_cast<cost_f_type>(this->graph.infty_value());
	};

	
	explicit Game(const Graph_type& input_graph, cost_f_type _alpha, int _radius_of_steps, std::set<char>& _list_of_prohib_moves, bool _bilateral) : graph(input_graph), alpha(_alpha), radius_of_steps(_radius_of_steps), list_of_prohib_moves(_list_of_prohib_moves), bilateral(_bilateral)
	{
		infty = static_cast<cost_f_type>(this->graph.infty_value());
	};

	
	explicit Game(const Graph_type& input_graph, cost_f_type _alpha, bool _bilateral) : graph(input_graph), alpha(_alpha), bilateral(_bilateral) {
		infty = static_cast<cost_f_type>(this->graph.infty_value());
		radius_of_steps = this->graph.graph_num_of_nodes();
		list_of_prohib_moves;
	};

	
	explicit Game(const Graph_type& input_graph, cost_f_type _alpha, int _radius_of_steps, bool _bilateral) : graph(input_graph), alpha(_alpha), radius_of_steps(_radius_of_steps), bilateral(_bilateral) {
		infty = static_cast<cost_f_type>(this->graph.infty_value());
		list_of_prohib_moves;
	};

	virtual cost_f_type edge_cost(const int agent) const {
		return alpha * this->graph.strategy_of_player(agent).size();
	}



	virtual cost_f_type distance_cost(const int agent) const// , const std::vector<cost_f_type>& _vector_of_dist = std::vector<cost_f_type>()) const 
	{ // Attention!!! If graph is disconnected, distance = infty
		auto	distances(this->graph.get_vector_of_dist(agent));

		if (!(this->graph.storage_dist()))
			distances = this->graph.vector_of_dist_weighted(agent);


		cost_f_type sum = 0.0;
		for (const auto& item : distances) {
			if (item != infty)
				sum += item;
			else
				return infty;
		}

		return static_cast<cost_f_type>(sum);
	}

	virtual cost_f_type cost(const int agent) const {
		return edge_cost(agent) + distance_cost(agent);
	}

	cost_f_type social_cost() const;

	virtual cost_f_type players_dist_cost_after_1_step(const int agent, const Step& step) const {
		auto distances = this->graph.distances_from_node_after_1_step(step, agent);
		cost_f_type sum = 0.0;
		for (const auto& item : distances) {
			if (item != infty)
				sum += item;
			else
				return infty;
		}
		
		return static_cast<cost_f_type>(sum);
	}


	virtual cost_f_type price_of_1_edge(const int v1, const int v2) const {
		if (this->graph.nodes_are_connected(v1, v2))
			return this->alpha;
		else
			return static_cast<cost_f_type>(0);
	}

	virtual cost_f_type players_edge_cost_after_1_step(const int agent, const Step& step) const {
		std::set<int> _curr_strategy = this->graph.strategy_of_player(agent);

		cost_f_type _edge_cost = edge_cost(agent);
		switch (step.step_name)
		{
		case 'a':
		{
			if (_curr_strategy.find(step.to) == _curr_strategy.end() && step.to != agent)
				_edge_cost += this->alpha;
			break;
		}
		case 'd':
		{
			if (_curr_strategy.find(step.to) == _curr_strategy.end() && step.from != agent)
				_edge_cost -= this->alpha;
			break;
		}
		default:
			break;
		}

		return  _edge_cost;
	}

	cost_f_type players_cost_after_1_step(const int agent, const Step& step) const {
		cost_f_type e_cost = players_edge_cost_after_1_step(agent, step);
		cost_f_type d_cost = players_dist_cost_after_1_step(agent, step);
		return e_cost + d_cost;
	}

	Step improving_response(const int agent, const bool random = 0) const;
		//greedy looking for improving response for agent, first check all additios, then deletions, then swaps (if applicable)
	// random means randomly chosen node to which strategy changes.

	Step pairwise_improving_response(const int agent, const bool _random) const;
	//random = choose endpoint to which add or delete edge u.a.r.

	std::set<int> best_response(const int agent);

	Step best_greedy_response(const int agent) const;
	Step best_greedy_response_pairwise(const int agent) const;

	bool check_if_GE_unilateral(const bool output_impr_step = 0) const;

	//by playing imoroving response, agents are activated randomly (because it is faster)
	//outputs .dot-file with GE and 3 three additional files with the degree distribution, the edge-costs distribution and the node degrees x local clustering 
	//random parameter means how enpoints for new nodes are generated
	void greedy_find_GE (const std::string& extra_notes_in_output_filename, //each produced file name start with this string
		const bool round_robin, 
		const bool random_endpoint, //if the endpoint for an edge that will be created or deleted should be chosen uniformly at random
		const bool _best_response, //if ==1, each agent plays her best response, if ==0, her first improving response
		const int when_output_results //if snapshots of the intermediate steps are needed, i.e., each "when_output_results" modification of the graph, the program outputs all information about the current graph in four files as for the final state
		);
	void best_of_the_best_response_GE(const std::string& extra_notes_in_output_filename);

	//for tests
	void make_move(const int agent, const Step& step);

	void output_graph(std::string file_name) const;

	bool check_if_greedy_pairwise_stable(const bool output_impr_step = 0) const;

protected:
	Graph_type graph;
	cost_f_type alpha;
	cost_f_type beta = 0.0;
	int radius_of_steps;

	std::set<char> list_of_prohib_moves; // \subseteq of {'s','d','a'}

	bool bilateral;
	cost_f_type infty;

private:
	void increase_bitstring_by_1(std::string& bitstr, const std::set<int>& avoid_agent) {//avoids edges to itself
		for (auto i = 0; i < bitstr.size(); i++)
		{
			if (bitstr[i] == 1)
				bitstr[i] = 0;
			else if (avoid_agent.find(i) == avoid_agent.end())
			{
				bitstr[i] = 1;
				break;
			}
		}
	}
};

template <typename cost_f_type>
class distNCG : public Game<Dynamic_W_graph, cost_f_type> {
private:
	cost_f_type pow_index; //beta value
public:
	
	//for the linear sigma-function
	explicit distNCG(const Dynamic_W_graph& input_graph, cost_f_type _alpha, std::set<char>& _list_of_prohib_moves, bool _bilateral) : Game<Dynamic_W_graph, cost_f_type>(input_graph, _alpha, _list_of_prohib_moves, _bilateral), pow_index(1) {};
	
	explicit distNCG(const Dynamic_W_graph& input_graph, cost_f_type _alpha, int _max_radius, std::set<char>& _list_of_prohib_moves, bool _bilateral) : Game<Dynamic_W_graph, cost_f_type>(input_graph, _alpha, _max_radius, _list_of_prohib_moves, _bilateral), pow_index(1) {};

	explicit distNCG(const Dynamic_W_graph& input_graph, cost_f_type _alpha, bool _bilateral) :Game<Dynamic_W_graph, cost_f_type>(input_graph, _alpha, _bilateral), pow_index(1) {};
	
	explicit distNCG(const Dynamic_W_graph& input_graph, cost_f_type _alpha, int _max_radius, bool _bilateral) :Game<Dynamic_W_graph, cost_f_type>(input_graph, _alpha, _max_radius, _bilateral), pow_index(1) {};

	//non-linear sigma function
	explicit distNCG(const Dynamic_W_graph& _input_graph, cost_f_type _alpha, std::set<char>& _list_of_prohib_moves, bool _bilateral, cost_f_type _pow_index) : Game<Dynamic_W_graph, cost_f_type>(_input_graph, _alpha, _list_of_prohib_moves, _bilateral), pow_index(_pow_index) {};

	cost_f_type edge_cost(const int agent) const
		//overload of the standart edge_cost function with default parameter
	{
		std::vector<int> default_vector;
		return this->edge_cost(agent, default_vector);
	}

	cost_f_type edge_cost(const int agent, std::vector<int>& _agents_edge_weights) const {
		cost_f_type result = static_cast<cost_f_type>(0);

		if (_agents_edge_weights.size() == 0)
		{
			_agents_edge_weights = this->graph.get_edge_weights(agent);
		}

		for (int i = 0; i < _agents_edge_weights.size(); i++)
			result += price_of_1_edge(_agents_edge_weights[i]);

		return  result;
	}

	cost_f_type cost(const int agent)  const {
		return edge_cost(agent) + this->distance_cost(agent);
	}

	cost_f_type sigma(const cost_f_type _alpha, const cost_f_type _beta, const cost_f_type _dist) const {
		if (_dist == 0)
			return 0.0;
		else
			return static_cast<cost_f_type>(_alpha * pow(_dist, _beta));
	}

	cost_f_type price_of_1_edge(const int v1, const int v2) const {
		if (this->graph.get_edge_weights(v1)[v2] == this->graph.infty_value())
			return sigma(this->alpha, pow_index, this->graph.graph_num_of_nodes());
		else
			return sigma(this->alpha, pow_index, static_cast<cost_f_type>(this->graph.get_edge_weights(v1)[v2]));
	}

	cost_f_type price_of_1_edge(const cost_f_type _dist) const {
		if (_dist == this->graph.infty_value())
			return sigma(this->alpha, this->pow_index, static_cast<cost_f_type>(this->graph.graph_num_of_nodes()));
		else
			return sigma(this->alpha, this->pow_index, _dist);
	}

	cost_f_type players_edge_cost_after_1_step(const int agent, const Step& step) const {
		std::vector<int> _edge_cost_vector = this->graph.edge_weights_after_1_step(step, agent);
		cost_f_type _edge_cost = 0;

		for (int i = 0; i < this->graph.graph_num_of_nodes(); i++) {
			_edge_cost += price_of_1_edge(_edge_cost_vector[i]);
			/*if (_edge_cost_vector[i] == this->graph.infty_value())
				_edge_cost += pow(static_cast<cost_f_type>( this->graph.graph_num_of_nodes()), pow_index);
			else
				_edge_cost += pow( static_cast<cost_f_type>(_edge_cost_vector[i]), pow_index);
				*/
		}
		return _edge_cost;
	}

	
};



//****************** Implementation 
template <typename Graph_type, typename cost_f_type>
cost_f_type Game<Graph_type, cost_f_type>::social_cost() const {
	cost_f_type tot_cost = static_cast<cost_f_type>(0);
	for (auto i = 0; i < graph.graph_num_of_nodes(); i++)
		tot_cost += cost(i);
	return tot_cost;
}


template <typename Graph_type, typename cost_f_type>
Step Game<Graph_type, cost_f_type>::improving_response(const int agent, const bool random)  const 
//greedy looking for an improving response for agent, first check all additions, then deletions, then swaps
//do not consider self-loops
{
	std::vector<int> accessible_nodes;
	std::set<int> k_neighbors;
	if (radius_of_steps < graph.graph_num_of_nodes()) { //if the game is local
		k_neighbors = graph.k_neighborhood(agent, radius_of_steps);
	}


	auto curr_dist_cost = distance_cost(agent); //need this for faster del-computations
	auto curr_agent_cost = edge_cost(agent) + curr_dist_cost;

	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		if (!graph.nodes_are_connected(agent, i) && i != agent) //for add-only version creating the list of accessible nodes takes too long !!!
			if (k_neighbors.size() == 0 || k_neighbors.find(i) != k_neighbors.end()) //if the game is non-local or the element is in k-neighborhood
				accessible_nodes.push_back(i);
	}

	//check additions
	if (list_of_prohib_moves.find('a') == list_of_prohib_moves.end()) {
		if (random) {
			std::set<int> list_of_still_accessible_elem(accessible_nodes.begin(), accessible_nodes.end());
			for (auto i = 0; i < accessible_nodes.size(); i++) {
				auto add_to = list_of_still_accessible_elem.begin();
				std::advance(add_to, rand() % list_of_still_accessible_elem.size()); //get k-th random element of the set 
				Step step('a', -1, *add_to);
				auto new_cost = players_cost_after_1_step(agent, step);
				if (new_cost < curr_agent_cost)
					return  step;
				list_of_still_accessible_elem.erase(add_to);
			}
		}
		else
			for (const auto& i : accessible_nodes) {
				Step step('a', -1, i);
				cost_f_type new_cost = players_cost_after_1_step(agent, step);
				if (new_cost < curr_agent_cost)
					return  step;
			}
	}

	//check deletions
	if (list_of_prohib_moves.find('d') == list_of_prohib_moves.end()) {
		for (const auto& i : graph.strategy_of_player(agent)) {
			Step step('d', i, -1);
			cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(agent, step);
			if (new_dist_cost < curr_dist_cost + price_of_1_edge(agent, i) && new_dist_cost != this->graph.infty_value()) {
				cost_f_type new_cost = players_edge_cost_after_1_step(agent, step) + new_dist_cost;
				if (new_cost < curr_agent_cost)
					return step;
			}
		}
	}

	//check swaps
	if (list_of_prohib_moves.find('s') == list_of_prohib_moves.end()) {
		for (const auto& node_from : graph.strategy_of_player(agent)) {
			for (const auto& node_to : accessible_nodes) {
				Step step('s', node_from, node_to);
				cost_f_type new_cost = players_cost_after_1_step(agent, step);
				if (new_cost < curr_agent_cost)
					return step;
			}
		}
	}

	//if no improvement, return empty step
	return  Step(' ', -1, -1);														
}

template <typename Graph_type, typename cost_f_type>
Step Game<Graph_type, cost_f_type>::pairwise_improving_response(const int agent, const bool _random) const {	
	std::vector<int> accessible_nodes;
	std::set<int> k_neighborhood;
	if (radius_of_steps < graph.graph_num_of_nodes()) { //if the game is local
		k_neighborhood = graph.k_neighborhood(agent, radius_of_steps);
	}

	auto curr_dist_cost = distance_cost(agent); //need this for faster del-computations
	auto curr_agent_cost = edge_cost(agent) + curr_dist_cost;


	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		if (!graph.nodes_are_connected(agent, i) && i != agent)
			if (k_neighborhood.size() == 0 || std::binary_search(k_neighborhood.begin(), k_neighborhood.end(), i)) //if the game is local and the element is in k-neighborhood
				accessible_nodes.push_back(i);
	}

	if (_random) {
		std::set<int> list_of_not_checked_endpoints;

		if (radius_of_steps < graph.graph_num_of_nodes()) //if local
		{
			for (int i = 0; i < graph.graph_num_of_nodes(); i++)
				if (std::binary_search(k_neighborhood.begin(), k_neighborhood.end(), i) && i != agent)
					list_of_not_checked_endpoints.insert(i);
		}
		else
		{
			for (int i = 0; i < graph.graph_num_of_nodes(); i++)
			{
				if (i != agent)
					list_of_not_checked_endpoints.insert(i);
			}
		}

		while (list_of_not_checked_endpoints.size() > 0) {
			auto i = list_of_not_checked_endpoints.begin();
			std::advance(i, rand() % list_of_not_checked_endpoints.size()); // get i-th element of the set
			int active_endpoint = *i;

			list_of_not_checked_endpoints.erase(active_endpoint);

			if (this->list_of_prohib_moves.find('d') == this->list_of_prohib_moves.end()
				&& this->graph.nodes_are_connected(agent, active_endpoint))
			{//try deletion
				Step step('d', active_endpoint, -1);
				cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(agent, step);
				if (new_dist_cost < curr_dist_cost + price_of_1_edge(agent, active_endpoint) && new_dist_cost != this->graph.infty_value()) {
					cost_f_type new_cost = players_edge_cost_after_1_step(agent, step) + new_dist_cost;
					if (new_cost < curr_agent_cost)
						return step;
				}
			}
			else if (this->list_of_prohib_moves.find('a') == this->list_of_prohib_moves.end())
			{//try to perform addition
				Step step('a', -1, active_endpoint);

				cost_f_type new_cost_agent_1 = players_cost_after_1_step(agent, step);
				cost_f_type cost_agent_2 = cost(active_endpoint);
				cost_f_type new_cost_agent_2 = players_cost_after_1_step(active_endpoint, Step('a', -1, agent));
				if (new_cost_agent_1 < curr_agent_cost && new_cost_agent_2 < cost_agent_2)
					return  step;
			}
		}
	}
	else {
		//check additions
		if (list_of_prohib_moves.find('a') == list_of_prohib_moves.end()) {
			for (const auto& i : accessible_nodes) {
				Step step('a', -1, i);

				cost_f_type new_cost_agent_1 = players_cost_after_1_step(agent, step);
				cost_f_type cost_agent_2 = cost(i);
				cost_f_type new_cost_agent_2 = players_cost_after_1_step(i, Step('a', -1, agent));
				if (new_cost_agent_1 < curr_agent_cost && new_cost_agent_2 < cost_agent_2)
					return  step;
			}
		}

		//check deletions
		if (list_of_prohib_moves.find('d') == list_of_prohib_moves.end()) {
			for (const auto& i : graph.strategy_of_player(agent)) {
				Step step('d', i, -1);
				cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(agent, step);
				if (new_dist_cost < curr_dist_cost + price_of_1_edge(agent, i) && new_dist_cost != this->graph.infty_value()) {
					cost_f_type new_cost = players_edge_cost_after_1_step(agent, step) + new_dist_cost;
					if (new_cost < curr_agent_cost)
						return step;
				}
			}
		}
	}

	//if no improvement, return empty step
	return  Step(' ', -1, -1);
}



template <typename Graph_type, typename cost_f_type>
std::set<int> Game<Graph_type, cost_f_type>::best_response(const int agent) {		
	//greedy check all possible strategies
	std::set<int> best_strategy = {};
	graph.set_strategy(agent, best_strategy);
	cost_f_type best_cost = cost(agent);

	std::string bitstring(graph.graph_num_of_nodes(), 0);
	std::string final_str(graph.graph_num_of_nodes(), 1);

	std::set<int> restrict_set = graph.neighborhood(agent); //do not consider edges to this nodes because they are already in the graph
	restrict_set.insert(agent);

	if (radius_of_steps < graph.graph_num_of_nodes()) { //if the game is local
		std::set<int> k_neighborhood;
		k_neighborhood = graph.k_neighborhood(agent, radius_of_steps);
		for (int i = 0; i < graph.graph_num_of_nodes(); i++)
			if (k_neighborhood.find(i) != k_neighborhood.end())
				restrict_set.insert(i);
	}

	for (auto& i : restrict_set)
		final_str[i] = 0;

	do {
		increase_bitstring_by_1(bitstring, restrict_set);

		std::set<int> new_strategy;
		for (auto i = 0; i < bitstring.size(); i++)
			if (bitstring[i])
				new_strategy.insert(i);

		graph.set_strategy(agent, new_strategy);

		cost_f_type new_cost = cost(agent);
		if (new_cost < best_cost) {
			best_cost = new_cost;
			best_strategy = new_strategy;
		}
	} while (bitstring != final_str);

	return best_strategy;
}


template <typename Graph_type, typename cost_f_type>
Step Game<Graph_type, cost_f_type>::best_greedy_response(const int agent) const {
	std::vector<int> accessible_nodes;
	std::set<int> k_neighborhood;
	if (radius_of_steps < graph.graph_num_of_nodes()) { //if the game is local
		k_neighborhood = graph.k_neighborhood(agent, radius_of_steps);
	}

	cost_f_type curr_agent_cost = cost(agent);

	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		if (graph.neighborhood(agent).count(i) == 0 && i != agent)
			if (k_neighborhood.size() == 0 || std::binary_search(k_neighborhood.begin(), k_neighborhood.end(), i)) //if the game is local and the element is in k-neighborhood
				accessible_nodes.push_back(i);
	}

	Step best_move(' ', -1, -1);

	if (this->list_of_prohib_moves.find('a') == list_of_prohib_moves.end()) {
		//check additions
		for (const auto& i : accessible_nodes) {
			cost_f_type new_cost = players_cost_after_1_step(agent, Step('a', -1, i));

			if (new_cost < curr_agent_cost)
			{
				best_move = Step('a', -1, i);
				curr_agent_cost = new_cost;
				std::cout << "add " << agent << ' ' << i << ", dist=" << distance_cost(agent) << ", edge_cost=" << edge_cost(agent) << ", new cost = " << new_cost << '\n';
			}
		}
	}

	if (this->list_of_prohib_moves.find('d') == this->list_of_prohib_moves.end()) {
		//check deletions
		for (const auto& i : graph.strategy_of_player(agent)) {
			cost_f_type new_cost = players_cost_after_1_step(agent, Step('d', i, -1));
			if (new_cost < curr_agent_cost)
			{
				best_move = Step('d', i, -1);
				curr_agent_cost = new_cost;
				std::cout << "del " << agent << ' ' << i << ", dist=" << distance_cost(agent) << ", edge_cost=" << edge_cost(agent) << ", new cost = " << new_cost << '\n';
			}
		}
	}

	if (this->list_of_prohib_moves.find('s') == this->list_of_prohib_moves.end()) {
		//check swaps
		for (const auto& node_from : graph.strategy_of_player(agent)) {
			for (const auto& node_to : accessible_nodes) {
				cost_f_type new_cost = players_cost_after_1_step(agent, Step('s', node_from, node_to));
				if (new_cost < curr_agent_cost)
				{
					best_move = Step('s', node_from, node_to);
					curr_agent_cost = new_cost;
					std::cout << "swap " << agent << ' ' << node_from << " to: " << node_to << ", dist=" << distance_cost(agent) << ", edge_cost=" << edge_cost(agent) << ", new cost = " << new_cost << '\n';
				}
			}
		}
	}

	//if no improvement, return empty step
	return  best_move;														

}

template<typename Graph_type, typename cost_f_type>
Step Game<Graph_type, cost_f_type>::best_greedy_response_pairwise(const int agent) const {
	std::vector<int> accessible_nodes;
	std::set<int> k_neighborhood;
	if (radius_of_steps < graph.graph_num_of_nodes()) { //if the game is local
		k_neighborhood = graph.k_neighborhood(agent, radius_of_steps);
	}

	cost_f_type curr_dist_cost = distance_cost(agent);
	cost_f_type curr_agent_cost = curr_dist_cost + edge_cost(agent);

	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		if (graph.neighborhood(agent).count(i) == 0 && i != agent)
			if (k_neighborhood.size() == 0 || std::binary_search(k_neighborhood.begin(), k_neighborhood.end(), i)) //if the game is local and the element is in k-neighborhood
				accessible_nodes.push_back(i);
	}

	Step best_move(' ', -1, -1);

	if (this->list_of_prohib_moves.find('a') == this->list_of_prohib_moves.end()) {
		//check additions
		for (const auto& i : accessible_nodes) {
			Step step('a', -1, i);
			cost_f_type new_dist_cost_agent_1 = players_dist_cost_after_1_step(agent, step);
			cost_f_type new_cost_agent_1 = new_dist_cost_agent_1 + players_edge_cost_after_1_step(agent, step);
			cost_f_type cost_agent_2 = cost(i);
			cost_f_type new_cost_agent_2 = players_cost_after_1_step(i, Step('a', -1, agent));

			if (new_cost_agent_1 < curr_agent_cost && new_cost_agent_2 < cost_agent_2)

			{
				best_move = Step('a', -1, i);
				curr_agent_cost = new_cost_agent_1;
			}
		}
	}

	if (this->list_of_prohib_moves.find('d') == this->list_of_prohib_moves.end()) {
		//check deletions
		for (const auto& active_endpoint : graph.strategy_of_player(agent)) {
			Step step('d', active_endpoint, -1);
			cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(agent, step);
			if (new_dist_cost < curr_dist_cost + price_of_1_edge(agent, active_endpoint) && new_dist_cost != this->graph.infty_value()) {
				cost_f_type new_cost = players_edge_cost_after_1_step(agent, step) + new_dist_cost;
				if (new_cost < curr_agent_cost) {
					best_move = Step('d', active_endpoint, -1);
					curr_agent_cost = new_cost;
					curr_dist_cost = new_dist_cost;
					//std::cout << "del " << agent << ' ' << active_endpoint << ", dist=" << curr_dist_cost << ", edge_cost=" << edge_cost(agent) << ", new cost = " << new_cost << '\n';
					std::cout << "del " << agent << ' ' << active_endpoint << '\n';
				}
			}
		}
	}


	//if no improvement, return empty step
	return  best_move;														
}

template <typename Graph_type, typename cost_f_type>
bool Game<Graph_type, cost_f_type>::check_if_GE_unilateral(const bool output_impr_step) const {
	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		{
			Step move;
			move = improving_response(i);

			if (move.step_name != ' ')
			{
				if (output_impr_step) std::cout << i << " has improving move: " << move;
				return 0;
			}
		}
	}
	return 1;
}

template <typename Graph_type, typename cost_f_type>
void Game<Graph_type, cost_f_type>::greedy_find_GE(const std::string& extra_notes_in_output_filename, //each produced file name start with this string
													const bool round_robin, const bool random_endpoint, 
													const bool _best_response, const int when_output_results //if snapshots of the intermediate steps are needed, i.e., each "when_output_results" modification of the graph, the program outputs all information about the current graph in four files as for the final state
													) {

	std::set<int> set_of_nonactive_agents, full_set;
	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		set_of_nonactive_agents.insert(i);
		full_set.insert(i);
	}
	int count = 0;
	int active_agent = -1;

	while (set_of_nonactive_agents.size() != 0) {
		auto i = set_of_nonactive_agents.begin();

		if (round_robin)
			active_agent = (active_agent + 1) % (this->graph.graph_num_of_nodes());
		else
		{
			std::advance(i, rand() % set_of_nonactive_agents.size()); // get i-th random element of the set
			active_agent = *i;
		}

		Step move;
		if (this->bilateral)
			if (_best_response)
				move = best_greedy_response_pairwise(active_agent);
			else
				move = pairwise_improving_response(active_agent, random_endpoint);
		else
			if (_best_response)
				move = best_greedy_response(active_agent);
			else
				move = improving_response(active_agent, random_endpoint);

		if (move.step_name == ' ')
		{
			set_of_nonactive_agents.erase(active_agent);
			std::cout << "agent " << active_agent << " is happy\n";
		}
		else {
			count++;
			this->graph.perform_step(active_agent, move, this->bilateral, 1);

			if ((when_output_results > 0) && (count % when_output_results) == 0) {
				//output edge-weights distribution
				std::vector<int> edge_distribution(this->graph.graph_num_of_nodes(), 0);
				for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
				{
					if (!bilateral)
						for (int j = 0; j < this->graph.graph_num_of_nodes(); j++) {
							if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
								edge_distribution[this->graph.get_edge_weights(i)[j]]++;
							else if (this->graph.get_edge_weights(i)[j] == this->infty)
								edge_distribution[this->graph.graph_num_of_nodes() - 1]++;
						}
					else
						for (int j = i; j < this->graph.graph_num_of_nodes(); j++) {
							if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
								edge_distribution[this->graph.get_edge_weights(i)[j]]++;
							else if (this->graph.get_edge_weights(i)[j] == this->infty)
								edge_distribution[this->graph.graph_num_of_nodes() - 1]++;
						}
				}
				std::ofstream out_edge_distr_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + "_edge_distribution.dot");
				if (out_edge_distr_file.is_open())
				{
					for (const auto i : edge_distribution)
						out_edge_distr_file << std::to_string(i) << '\n';
					out_edge_distr_file.close();
				}
				else
				{
					std::cout << "Cannot open out_file for the edge_distribution output";
					exit(0);
				}

				//output degree distribution
				std::vector<int> deg_distribution(this->graph.graph_num_of_nodes(), 0);
				for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
					deg_distribution[this->graph.node_degree(i)]++;

				std::ofstream out_deg_distr_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + "_deg_distribution.dot");
				if (out_deg_distr_file.is_open())
				{
					for (const auto i : deg_distribution)
						out_deg_distr_file << std::to_string(i) << '\n';
					out_deg_distr_file.close();
				}
				else
				{
					std::cout << "Cannot open out_file for the degree_distribution output";
					exit(0);
				}

				graph.output_graph_to_dot_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + ".dot");
			}
			set_of_nonactive_agents = full_set;					//check this for better variant
			std::cout << "agent " << active_agent << " does " << move << '\n';
		}
	}
	cost_f_type cost = social_cost();

	std::string extra_notes = "";
	if (radius_of_steps < graph.graph_num_of_nodes()) {
		extra_notes += "_" + std::to_string(radius_of_steps) + "_local";
	}
	else
		extra_notes += "_global";

	extra_notes += "_" + std::to_string(graph.avg_clustering()) + "_avg_clust_" + std::to_string(this->graph.diameter()) + "_D";
	this->graph.output_graph_to_dot_file("GE_" + std::to_string(graph.graph_num_of_nodes()) + "_n_" + extra_notes_in_output_filename + extra_notes + ".dot");

	//output all statistics for the resulting graph

	//1. degree distribution
	std::vector<int> deg_distribution(this->graph.graph_num_of_nodes(), 0);
	for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
		deg_distribution[this->graph.node_degree(i)]++;

	std::ofstream out_deg_distr_file(std::to_string(graph.graph_num_of_nodes()) + "_n_" + extra_notes_in_output_filename + "_deg_distribution.dot");
	if (out_deg_distr_file.is_open())
	{
		for (const auto i : deg_distribution)
			out_deg_distr_file << std::to_string(i) << '\n';
		out_deg_distr_file.close();
	}
	else
	{
		std::cout << "Cannot open out_file for the degree_distribution output";
		exit(0);
	}

	//2. edge_cost distribution
	std::vector<int> edge_distribution(this->graph.graph_num_of_nodes(), 0);
	for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
	{
		if (!bilateral)
			for (int j = 0; j < this->graph.graph_num_of_nodes(); j++) {
				if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
					edge_distribution[this->graph.get_edge_weights(i)[j]]++;
				else if (this->graph.get_edge_weights(i)[j] == this->infty)
					edge_distribution[0]++;
			}
		else
			for (int j = i; j < this->graph.graph_num_of_nodes(); j++) {
				if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
					edge_distribution[this->graph.get_edge_weights(i)[j]]++;
				else if (this->graph.get_edge_weights(i)[j] == this->infty)
					edge_distribution[0]++;
			}
	}
	std::ofstream out_edge_distr_file(std::to_string(graph.graph_num_of_nodes()) + "_n_" + extra_notes_in_output_filename + "_edge_distribution.dot");
	if (out_edge_distr_file.is_open())
	{
		for (const auto i : edge_distribution)
			out_edge_distr_file << std::to_string(i) << '\n';
		out_edge_distr_file.close();
	}
	else
	{
		std::cout << "Cannot open out_file for the edge_distribution output";
		exit(0);
	}

	//3. degree x local CC
	std::ofstream out_deg_CC_file(std::to_string(graph.graph_num_of_nodes()) + "_n_" + extra_notes_in_output_filename + "_node_deg_local_CC.dot");
	if (out_deg_CC_file.is_open())
	{
		for (int i = 0; i < this->graph.graph_num_of_nodes(); i++) {

			out_deg_CC_file << this->graph.node_degree(i) << '	' << this->graph.local_clustering(i) << '\n';
		}
		out_deg_CC_file.close();
	}
	else
	{
		std::cout << "Cannot open out_file for the deg x CC output";
		exit(0);
	}
}

template <typename Graph_type, typename cost_f_type>
void Game<Graph_type, cost_f_type>::best_of_the_best_response_GE(const std::string& extra_notes_in_output_filename) {

	int count = 0;
	//int active_agent = -1;
	Step best_BR;
	int BR_active_agent;
	cost_f_type best_delta_cost;

	do {
		std::vector<std::pair<cost_f_type, cost_f_type>> list_of_agents_costs;
		for (int agent = 0; agent < this->graph.graph_num_of_nodes(); agent++) {
			list_of_agents_costs.push_back(std::pair<cost_f_type, cost_f_type>(edge_cost(agent), distance_cost(agent)));
			//			list_of_agents_costs[agent].first = edge_cost(agent);
				//		list_of_agents_costs[agent].second = distance_cost(agent);
		}

		best_delta_cost = 0.0;

		for (int active_agent_1 = 0; active_agent_1 < this->graph.graph_num_of_nodes(); active_agent_1++) {
			if (this->bilateral)
				for (int active_agent_2 = active_agent_1 + 1; active_agent_2 < this->graph.graph_num_of_nodes(); active_agent_2++)
				{
					if (this->graph.nodes_are_connected(active_agent_1, active_agent_2) && (this->list_of_prohib_moves.find('d') == this->list_of_prohib_moves.end())) //then try delete the edge by both endpoints sequentialy
					{
						Step move('d', active_agent_2, -1);
						cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(active_agent_1, move);
						if (new_dist_cost < list_of_agents_costs[active_agent_1].second + price_of_1_edge(active_agent_1, active_agent_2) && new_dist_cost != this->graph.infty_value()) {
							cost_f_type new_delta_cost = players_edge_cost_after_1_step(active_agent_1, move) + new_dist_cost - (list_of_agents_costs[active_agent_1].first + list_of_agents_costs[active_agent_1].second);

							if (new_delta_cost < best_delta_cost) {
								best_BR = move;
								BR_active_agent = active_agent_1;
								best_delta_cost = new_delta_cost;
								std::cout << "new BR: del " << std::to_string(active_agent_1) << "->" << std::to_string(active_agent_2) << ", new delta_cost = " << best_delta_cost << '\n';
							}
						}
						//check del by the second endpoint
						move = Step('d', active_agent_1, -1);
						new_dist_cost = this->players_dist_cost_after_1_step(active_agent_2, move);
						if (new_dist_cost < list_of_agents_costs[active_agent_2].second + price_of_1_edge(active_agent_2, active_agent_1) && new_dist_cost != this->graph.infty_value()) {
							cost_f_type new_delta_cost = players_edge_cost_after_1_step(active_agent_2, move) + new_dist_cost - (list_of_agents_costs[active_agent_2].first + list_of_agents_costs[active_agent_2].second);

							if (new_delta_cost < best_delta_cost) {
								best_BR = move;
								BR_active_agent = active_agent_2;
								best_delta_cost = new_delta_cost;
								std::cout << "new BR: del " << std::to_string(active_agent_2) << "->" << std::to_string(active_agent_1) << ", new delta_cost = " << best_delta_cost << '\n';
							}
						}
					}
					else if (!(this->graph.nodes_are_connected(active_agent_1, active_agent_2)) && (this->list_of_prohib_moves.find('a') == this->list_of_prohib_moves.end())) //try addition of the missing edge
					{
						Step move('a', -1, active_agent_2);
						cost_f_type new_delta_cost_agent_1 = players_cost_after_1_step(active_agent_1, move) - (list_of_agents_costs[active_agent_1].first + list_of_agents_costs[active_agent_1].second);
						cost_f_type new_delta_cost_agent_2 = players_cost_after_1_step(active_agent_2, Step('a', -1, active_agent_1)) - (list_of_agents_costs[active_agent_2].first + list_of_agents_costs[active_agent_2].second);
						if (new_delta_cost_agent_1 < 0 && new_delta_cost_agent_2 < 0 && std::min(new_delta_cost_agent_1, new_delta_cost_agent_2) < best_delta_cost)
						{
							best_BR = move;
							BR_active_agent = active_agent_1; //since bilateral version, we don't care who is the owner of the best edge
							best_delta_cost = std::min(new_delta_cost_agent_1, new_delta_cost_agent_2);
							std::cout << "new BR: add " << std::to_string(active_agent_1) << "->" << std::to_string(active_agent_2) << ", new delta_cost = " << best_delta_cost << '\n';
						}

					}
				}
			else
				for (int active_agent_2 = 0; active_agent_2 < this->graph.graph_num_of_nodes(); active_agent_2++)
				{
					if (this->graph.nodes_are_connected(active_agent_1, active_agent_2) && (this->list_of_prohib_moves.find('d') == this->list_of_prohib_moves.end())) //then try delete the edge by both endpoints sequentialy
					{
						Step move('d', active_agent_2, -1);
						cost_f_type new_dist_cost = this->players_dist_cost_after_1_step(active_agent_1, move);
						if (new_dist_cost < list_of_agents_costs[active_agent_1].second + price_of_1_edge(active_agent_1, active_agent_2) && new_dist_cost != this->graph.infty_value()) {
							cost_f_type new_delta_cost = players_edge_cost_after_1_step(active_agent_1, move) + new_dist_cost - (list_of_agents_costs[active_agent_1].first + list_of_agents_costs[active_agent_1].second);
							if (new_delta_cost < best_delta_cost) {
								best_BR = move;
								BR_active_agent = active_agent_1;
								best_delta_cost = new_delta_cost;
								std::cout << "new BR: del " << std::to_string(active_agent_1) << "->" << std::to_string(active_agent_2) << ", new delta_cost = " << best_delta_cost << '\n';
							}
						}
						//check del by the second endpoint
						move = Step('d', active_agent_1, -1);
						new_dist_cost = this->players_dist_cost_after_1_step(active_agent_2, move);
						if (new_dist_cost < list_of_agents_costs[active_agent_2].second + price_of_1_edge(active_agent_2, active_agent_1) && new_dist_cost != this->graph.infty_value()) {
							cost_f_type new_delta_cost = players_edge_cost_after_1_step(active_agent_2, move) + new_dist_cost - (list_of_agents_costs[active_agent_2].first + list_of_agents_costs[active_agent_2].second);
							if (new_delta_cost < best_delta_cost) {
								best_BR = move;
								BR_active_agent = active_agent_2;
								best_delta_cost = new_delta_cost;
								std::cout << "new BR: del " << std::to_string(active_agent_2) << "->" << std::to_string(active_agent_1) << ", new delta_cost = " << best_delta_cost << '\n';
							}
						}
					}
					else if (!(this->graph.nodes_are_connected(active_agent_1, active_agent_2)) && (this->list_of_prohib_moves.find('a') == this->list_of_prohib_moves.end()))//try addition of the missing edge
					{
						Step move('a', -1, active_agent_2);
						cost_f_type new_delta_cost_agent_1 = players_cost_after_1_step(active_agent_1, move) - (list_of_agents_costs[active_agent_1].first + list_of_agents_costs[active_agent_1].second);

						if (new_delta_cost_agent_1 < best_delta_cost)
						{
							best_BR = move;
							BR_active_agent = active_agent_1;
							best_delta_cost = new_delta_cost_agent_1;
							std::cout << "new BR: add " << std::to_string(active_agent_1) << "->" << std::to_string(active_agent_2) << ", new delta_cost = " << best_delta_cost << '\n';
						}

						move = Step('a', -1, active_agent_1);
						cost_f_type new_delta_cost_agent_2 = players_cost_after_1_step(active_agent_2, move) - (list_of_agents_costs[active_agent_2].first + list_of_agents_costs[active_agent_2].second);
						if (new_delta_cost_agent_2 < best_delta_cost)
						{
							best_BR = move;
							BR_active_agent = active_agent_2;
							best_delta_cost = new_delta_cost_agent_2;
							std::cout << "new BR: add " << std::to_string(active_agent_2) << "->" << std::to_string(active_agent_2) << ", new delta_cost = " << best_delta_cost << '\n';
						}

					}
				}
		}

		if (best_delta_cost != 0) {
			count++;
			this->graph.perform_step(BR_active_agent, best_BR, this->bilateral, 1);
			std::cout << "WINNER: " << std::to_string(BR_active_agent) << ", " << best_BR << '\n';

			if ((count % 250) == 0) {
				//output edge-weights distribution
				std::vector<int> edge_distribution(this->graph.graph_num_of_nodes(), 0);
				for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
				{
					if (!bilateral)
						for (int j = 0; j < this->graph.graph_num_of_nodes(); j++) {
							if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
								edge_distribution[this->graph.get_edge_weights(i)[j]]++;
							else if (this->graph.get_edge_weights(i)[j] == this->infty)
								edge_distribution[0]++;
						}
					else
						for (int j = i; j < this->graph.graph_num_of_nodes(); j++) {
							if (this->graph.get_edge_weights(i)[j] != 0 && this->graph.get_edge_weights(i)[j] != this->infty)
								edge_distribution[this->graph.get_edge_weights(i)[j]]++;
							else if (this->graph.get_edge_weights(i)[j] == this->infty)
								edge_distribution[0]++;
						}
				}
				std::ofstream out_edge_distr_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + "_edge_distribution.dot");
				if (out_edge_distr_file.is_open())
				{
					for (const auto i : edge_distribution)
						out_edge_distr_file << std::to_string(i) << '\n';
					out_edge_distr_file.close();
				}
				else
				{
					std::cout << "Cannot open out_file for the edge_distribution output";
					exit(0);
				}

				//output degree distribution
				std::vector<int> deg_distribution(this->graph.graph_num_of_nodes(), 0);
				for (int i = 0; i < this->graph.graph_num_of_nodes(); i++)
					deg_distribution[this->graph.node_degree(i)]++;

				std::ofstream out_deg_distr_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + "_deg_distribution.dot");
				if (out_deg_distr_file.is_open())
				{
					for (const auto i : deg_distribution)
						out_deg_distr_file << std::to_string(i) << '\n';
					out_deg_distr_file.close();
				}
				else
				{
					std::cout << "Cannot open out_file for the degree_distribution output";
					exit(0);
				}

				graph.output_graph_to_dot_file("alpha_" + std::to_string(this->alpha) + "_step_" + std::to_string(count) + extra_notes_in_output_filename + ".dot");
			}
		}
	} while (best_delta_cost != 0);


	cost_f_type cost = social_cost();

	std::string extra_notes = "";
	if (radius_of_steps < graph.graph_num_of_nodes()) {
		extra_notes += "_" + std::to_string(radius_of_steps) + "_local";
	}
	else
		extra_notes += "_global";

	extra_notes += "_" + std::to_string(graph.avg_clustering()) + "_avg_clust" + extra_notes_in_output_filename;
	this->graph.output_graph_to_dot_file("BBR_GE_" + std::to_string(graph.graph_num_of_nodes()) + "_n_" + std::to_string(alpha) + "_alpha_" + std::to_string(cost) + extra_notes + ".dot");
}




template <typename Graph_type, typename cost_f_type>
void Game<Graph_type, cost_f_type>::make_move(const int agent, const Step& step) {
	graph.perform_step(agent, step, this->bilateral, 1);
}


template <typename Graph_type, typename cost_f_type>
void Game<Graph_type, cost_f_type>::output_graph(std::string file_name) const {
	graph.output_graph_to_dot_file(file_name);
}

template <typename Graph_type, typename cost_f_type>
bool Game<Graph_type, cost_f_type>::check_if_greedy_pairwise_stable(const bool output_impr_step) const {
	for (auto i = 0; i < graph.graph_num_of_nodes(); i++) {
		{
			Step move = pairwise_improving_response(i, 0);

			if (move.step_name != ' ')
			{
				if (output_impr_step) std::cout << i << " has improving move: " << move;
				return 0;
			}
		}
	}
	return 1;
}
