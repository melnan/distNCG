#include "graph.h"
#include "NCG.h"


//The edge cost function, sigma(x), can be definied in a different way (see NCG.h distNCG::sigma() function)
// By default sigma((u,v)) = alpha * pow(dist(u,v), beta)
//since each edge is paid by both endpoints, edge cost = 2sigma(edge)


int main() {
	srand(time(0));


	int n = 100;			// number of nodes;
	double  alpha = 10;		//alpha parameter for the sigma-function
	int beta = 2;			//beta parameter for the sigma function
	int infty = 1000;		//distance between two nodes in a disconnected network. It is not an actual infty value, it works like a marker, i.e., this value is never incremented
	bool pairwise = 1;		//if pairvise==0, the game is unilateral. However, in the paper we consider only bilateral version of the game.
	std::set<char> list_of_prohib_moves = { 's' }; //set of the moves that are not allowed for player (to model add-only, del-only, and so on game)
	bool round_robin = 0;	//activation order: round robin means "according to the numeration", not round robin means "uniformly at random"
	bool best_resp = 1;		//if players should play the best strategy (=1) or the first improving strategy (=0)
	bool storage_distances = 1;	//the code is optimized for the case when distances are kept in memory, so better leave it like it is
	bool rand_endpoint = 1;	//tells if the endpoint for an edge that will be created or deleted should be chosen uniformly at random

	
	//1st, initialize a network
	Dynamic_W_graph g_tree(n, infty);

	//2nd, generate a network you want (more network types see in Dynamic_W_graph class)
		g_tree.triangulated_cycle(pairwise);

	//output the initial network if needed
		g_tree.output_graph_to_dot_file("my_init_graph.dot");

	//3rd, initialize a game. 		
		distNCG<double> game(g_tree, alpha, list_of_prohib_moves, pairwise, beta);
		
	//4th, run the algorithm to compute a stable state
		std::string extra_notes = std::to_string(alpha) + "_alpha_" + std::to_string(beta) + "_beta_PSN_from_cycle_rand_BR_";
		game.greedy_find_GE(extra_notes, round_robin, rand_endpoint, best_resp, -1);
	

	/*
	Just an example of many sequential runs producing the solutions for different combinations of the parameters
	std::string extra_notes; 
	for (int num_of_run=1; num_of_run<=10; num_of_run++)
		for (auto& n : { 100, 500, 1000, 1500, 2000, 2500, 3000 })
		{	
			for (auto& beta : {  3 })
			{
				//alpha = my_log(2,n);
				Dynamic_W_graph g_tree(n, infty);
				g_tree.tree_graph(pairwise, 2);
				extra_notes = std::to_string(alpha) + "_alpha_" + std::to_string(beta) + "_beta_PSN_from_tree_rand_BR_run_" + std::to_string(num_of_run);
				distNCG<double> game_tree(g_tree, alpha, list_of_prohib_moves, pairwise, beta);
				game_tree.greedy_find_GE(extra_notes, round_robin, rand_endpoint, best_resp, -1);

				Dynamic_W_graph g_cycle(n, infty);
				g_cycle.cycle_graph(pairwise);
				extra_notes = std::to_string(alpha) + "_alpha_" + std::to_string(beta) + "_beta_PSN_from_cycle_rand_BR_run_" + std::to_string(num_of_run);
				distNCG<double> game_cycle(g_cycle, alpha, list_of_prohib_moves, pairwise, beta);
				game_cycle.greedy_find_GE(extra_notes, round_robin, rand_endpoint, best_resp, -1);
			}
		}
		*/

		std::cout << "\n done \n";
	return 0;
}
 