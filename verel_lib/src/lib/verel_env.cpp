#include "verel_env.h"
#include "graph.h"
#include <cassert>
#include <random>
#include <algorithm>
#include <iostream>

VerelEnv::VerelEnv(double _norm) : IEnv(_norm)
{
}

void VerelEnv::s0(std::shared_ptr<Graph> _g)
{
  graph = _g;                // pointer to the graph object
  eliminated_set.clear();    // eliminated set
  numEliminatedNodes = 0;    // num of eliminated nodes
  action_list.clear();       // action list

  state_seq.clear();         // sequence of states
  act_seq.clear();           // sequence of actions
  reward_seq.clear();        // sequence of rewards
  sum_rewards.clear();       // sum of rewards
}

// next step action
double VerelEnv::step(int a)
{
  assert(graph);
  // checking whether the node a is in the list of eliminated nodes
  assert(eliminated_set.count(a) == 0);
  assert(a < graph->num_nodes);
  
  state_seq.push_back(action_list);
  act_seq.push_back(a);
  
  // adding the node to the list of eliminated nodes
  action_list.push_back(a);
  eliminated_set.insert(a);
  numEliminatedNodes += 1;
  // VERTEX ELIMINATION ACTION
  double r_t = eliminate_node(a);
  
  reward_seq.push_back(r_t);
  sum_rewards.push_back(r_t);

  return r_t;
}

// Pick a random node to eliminate
int VerelEnv::randomAction()
{
  assert(graph);
  avail_list.clear();

  for (int i = 0; i < graph->num_nodes; ++i)
    if (eliminated_set.count(i) == 0)
      avail_list.push_back(i);

  assert(avail_list.size());
  int idx = rand() % avail_list.size();
  return avail_list[idx];
}

// Terminatation state when all the vertices are eliminated
bool VerelEnv::isTerminal()
{
  assert(graph);
  return graph->num_nodes == numEliminatedNodes;
}

// Define reward value
double VerelEnv::eliminate_node(int new_node) {
  
  int num_new_edges = 0;

  while(graph->adj_list_modified[new_node].empty() == false) {
    int neigh1_val = graph->adj_list_modified[new_node].back();
    
    // deleting from the dajacency list from both copies
    graph->adj_list_modified[new_node].pop_back();
    auto it = std::find(graph->adj_list_modified[neigh1_val].begin(),
		    graph->adj_list_modified[neigh1_val].end(),
		    new_node);
    graph->adj_list_modified[neigh1_val].erase(it);

    // extra
    for (auto i = 0; i < graph->edge_list_modified.size(); ++i) {
      if ((std::get<0>(graph->edge_list_modified[i]) == new_node && std::get<1>(graph->edge_list_modified[i]) == neigh1_val) 
	  || (std::get<0>(graph->edge_list_modified[i]) == neigh1_val && std::get<1>(graph->edge_list_modified[i]) == new_node)) {
	graph->edge_list_modified.erase(graph->edge_list_modified.begin() + i);
	i--;
      }
    }

    // updating the adjacency list and adding new edges
    for (auto& neigh2_val : graph->adj_list_modified[new_node]) {
      if (std::find(graph->adj_list_modified[neigh2_val].begin(),
		    graph->adj_list_modified[neigh2_val].end(),
		    neigh1_val) == graph->adj_list_modified[neigh2_val].end()) {
	num_new_edges += 1;
	graph->adj_list_modified[neigh2_val].push_back(neigh1_val);
	graph->adj_list_modified[neigh1_val].push_back(neigh2_val);

	// extra
	graph->edge_list_modified.push_back(std::make_pair(neigh1_val, neigh2_val));
	graph->edge_list_modified.push_back(std::make_pair(neigh2_val, neigh1_val));
      }
    }
  }

  //return ((-1.0) * num_new_edges) / norm;
  return num_new_edges;
}
