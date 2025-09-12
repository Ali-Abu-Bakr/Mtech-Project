#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <fstream>
using namespace std;

bool bfs(const vector<vector<int>>& rGraph, vector<int>& parent, int source, int sink, vector<bool>& visited) {
    int V = rGraph.size();
    fill(visited.begin(), visited.end(), false);

    queue<int> q;
    q.push(source);
    visited[source] = true;
    parent[source] = -1;

    while (!q.empty()) {
        int node = q.front();
        q.pop();

        for (int j = 0; j < V; ++j) {
            if (!visited[j] && rGraph[node][j] > 0) {
                parent[j] = node;
                visited[j] = true;
                q.push(j);
            }
        }
    }

    return visited[sink];
}

int fordFulkerson(vector<vector<int>>& graph, int source, int sink, vector<pair<int, int>>& minCutEdges) {
    int V = graph.size();
    vector<vector<int>> rGraph = graph;
    vector<int> parent(V);
    vector<bool> visited(V);
    int maxFlow = 0;

    while (bfs(rGraph, parent, source, sink, visited)) {
        int bottleNeck = INT_MAX;

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            bottleNeck = min(bottleNeck, rGraph[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            rGraph[u][v] -= bottleNeck;
            rGraph[v][u] += bottleNeck;
        }

        maxFlow += bottleNeck;
    }

    // Run BFS one last time to find reachable vertices from source
    bfs(rGraph, parent, source, sink, visited);

    // Find edges from reachable to non-reachable nodes in original graph
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            if (visited[i] && !visited[j] && graph[i][j] > 0) {
                minCutEdges.push_back(make_pair(i, j));
            }
        }
    }

    return maxFlow;
}

int main() {
    ifstream infile("example.txt");
    if (!infile.is_open()) {
        cerr << "Error opening input file.\n";
        return 1;
    }

    int V, E;
    infile >> V >> E;

    char graphType;
    infile >> graphType;

    vector<vector<int>> graph(V, vector<int>(V, 0));

    for (int i = 0; i < E; ++i) {
        int u, v, cap;
        infile >> u >> v >> cap;

        graph[u][v] += cap;
        if (graphType == 'U' || graphType == 'u') {
            graph[v][u] += cap;  // For undirected
        }
    }

    int source, sink;
    infile >> source >> sink;
    infile.close();

    vector<pair<int, int>> minCutEdges;
    int maxFlow = fordFulkerson(graph, source, sink, minCutEdges);

    cout << "\nThe maximum possible flow is: " << maxFlow << endl;

    cout << "\nEdges in the minimum cut are:\n";
    for (const auto& edge : minCutEdges) {
        cout << edge.first << " - " << edge.second << endl;
    }

    return 0;
}
