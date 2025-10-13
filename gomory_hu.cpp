#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9; // Use a large value for infinity
const int MAXN = 55;  // Maximum number of nodes

int V; // Number of vertices
int capacity[MAXN][MAXN];
int residual_graph[MAXN][MAXN];

// Stores the final Gomory-Hu tree as an adjacency list
// Each entry is a pair {neighbor, edge_weight}
vector<pair<int, int>> gomory_hu_tree[MAXN];

// --- Edmonds-Karp Max-Flow Section ---

// BFS to find an augmenting path in the residual graph
// Returns true if a path is found, false otherwise
bool bfs(int s, int t, vector<int>& parent) {
    fill(parent.begin(), parent.end(), -1);
    vector<bool> visited(V, false);
    queue<int> q;

    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; ++v) {
            if (!visited[v] && residual_graph[u][v] > 0) {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
                if (v == t) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Calculates max flow from s to t using Edmonds-Karp
int max_flow(int s, int t) {
    // Initialize residual graph with original capacities
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            residual_graph[i][j] = capacity[i][j];
        }
    }

    vector<int> parent(V);
    int total_max_flow = 0;

    // Augment the flow while a path exists from source to sink
    while (bfs(s, t, parent)) {
        int path_flow = INF;
        // Find the bottleneck capacity of the path found by BFS
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, residual_graph[u][v]);
        }

        // Update residual capacities of the edges and reverse edges
        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            residual_graph[u][v] -= path_flow;
            residual_graph[v][u] += path_flow;
        }

        total_max_flow += path_flow;
    }
    return total_max_flow;
}


// --- Gomory-Hu Tree Construction Section ---

// A helper function to find the set of nodes on the 's' side of the s-t min-cut
// It performs a traversal (like DFS) on the final residual graph
void find_cut_set(int s, vector<bool>& visited) {
    visited[s] = true;
    for (int i = 0; i < V; ++i) {
        if (!visited[i] && residual_graph[s][i] > 0) {
            find_cut_set(i, visited);
        }
    }
}

// The main function to construct the Gomory-Hu Tree
void construct_gomory_hu() {
    // parent[i] stores the node that i is contracted into in the tree construction
    vector<int> parent(V, 0);

    // The algorithm runs for n-1 stages, connecting each node i to its parent
    for (int s = 1; s < V; ++s) {
        // Find the min-cut between node 's' and its current parent 't'
        int t = parent[s];
        
        // The value of the min-cut is the max-flow
        int cut_value = max_flow(s, t);

        // Add the edge to our Gomory-Hu tree
        gomory_hu_tree[s].push_back({t, cut_value});
        gomory_hu_tree[t].push_back({s, cut_value});

        // Find the s-side of the min-cut partition
        vector<bool> s_side_of_cut(V, false);
        find_cut_set(s, s_side_of_cut);

        // For all nodes that were in t's component but are now on s's side of the cut,
        // update their parent to be s. This correctly partitions the graph for the next stage.
        for (int i = s + 1; i < V; ++i) {
            if (parent[i] == t && s_side_of_cut[i]) {
                parent[i] = s;
            }
        }
    }
}

int main() {
    cout << "Enter the number of vertices: ";
    cin >> V;

    cout << "Enter the number of edges: ";
    int E;
    cin >> E;

    // Initialize capacity matrix
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            capacity[i][j] = 0;
        }
    }

    cout << "Enter the edges (u v weight) one per line:" << endl;
    for (int i = 0; i < E; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        capacity[u][v] = w;
        capacity[v][u] = w; // For undirected graphs
    }

    construct_gomory_hu();

    cout << "\n--- Gomory-Hu Tree Edges (Min-Cuts) ---" << endl;
    vector<bool> printed(V, false);
    for (int u = 0; u < V; ++u) {
        printed[u] = true;
        for (auto const& [v, weight] : gomory_hu_tree[u]) {
            if (!printed[v]) {
                cout << "Min-cut between (" << u << ", " << v << ") is " << weight << endl;
            }
        }
    }

    return 0;
}