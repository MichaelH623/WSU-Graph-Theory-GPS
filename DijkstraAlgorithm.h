#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include "Node.h"
#include "Edge.h"

struct PathResult {
    float distance = -1.0f; // -1 indicates no path found
    std::vector<int> path;   // Sequence of Node IDs
};

PathResult findShortestPath(int startID, int endID, const std::vector<Node>& nodes, const std::vector<Edge>& edges) {
    // Build Adjacency List: Map Node ID -> list of (Neighbor ID, Weight)
    std::map<int, std::vector<std::pair<int, float>>> adj;
    for (const auto& edge : edges) {
        adj[edge.u].push_back({ edge.v, edge.weight });
        adj[edge.v].push_back({ edge.u, edge.weight }); // Assuming undirected
    }

    // Dijkstra's algorithm setup
    std::map<int, float> distances;
    std::map<int, int> predecessors;
    for (const auto& node : nodes) distances[node.id] = std::numeric_limits<float>::infinity();

    // Priority Queue: pair<distance, nodeID>
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> pq;

    distances[startID] = 0;
    pq.push({ 0, startID });

    while (!pq.empty()) {
        int u = pq.top().second;
        float d = pq.top().first;
        pq.pop();

        if (d > distances[u]) continue;
        if (u == endID) break; // Optimization: Found the target

        for (auto& neighbor : adj[u]) {
            int v = neighbor.first;
            float weight = neighbor.second;

            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                pq.push({ distances[v], v });
            }
        }
    }

    // Reconstruct Path
    PathResult result;
    if (distances[endID] != std::numeric_limits<float>::infinity()) {
        result.distance = distances[endID];
        for (int at = endID; at != startID; at = predecessors[at]) {
            result.path.push_back(at);
        }
        result.path.push_back(startID);
        std::reverse(result.path.begin(), result.path.end());
    }
    return result;
}