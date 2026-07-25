#define GRAPH_H
#define MESSAGE_H
#define NODE_H
#define PATH_H
#define PACKET_H
#define NETWORK_H
#define STRINGSPLITTER_H

#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <stack>
#include <iomanip>
#include <vector>
#include<bits/stdc++.h>

using namespace std;

void strip_cr(string &s)
{
    if (!s.empty() && s.back() == '\r')
    {
        s.pop_back();
    }
}

struct RouteEntry
{
    int destination;
    int nextHop;
    int cost;
};

class Node
{
private:
    int _id;
    int _load_factor;
    unordered_map<Node *, int> _edges;
    unordered_map<int, RouteEntry> _routing_table;
public:
    Node(int id);
    int getId() const;
    int getLoadFactor() const;
    int getWeight(Node *node);
    void increaseLoadFactor();
    void decreaseLoadFactor();
    void addEdge(Node *node, int weight);
    void removeEdge(Node *node);
    unordered_map<Node *, int> getEdges();
    
    void clearRoutingTable();
    void updateRouteEntry(int dest, int next_hop, int cost);
    int getRouteCost(int dest) const;
    const unordered_map<int, RouteEntry>& getRoutingTable() const;
};
Node::Node(int id)
{
    _id = id;
    _load_factor = 1;
}
int Node::getId() const
{
    return _id;
}
int Node::getLoadFactor() const
{
    return _load_factor;
}
int Node::getWeight(Node *node)
{
    return _edges[node];
}
void Node::increaseLoadFactor()
{
    _load_factor++;
}
void Node::decreaseLoadFactor()
{
    _load_factor--;
}
void Node::addEdge(Node *node, int weight)
{
    _edges[node] = weight;
}
void Node::removeEdge(Node *node)
{
    _edges.erase(node);
}
unordered_map<Node *, int> Node::getEdges()
{
    return _edges;
}
void Node::clearRoutingTable()
{
    _routing_table.clear();
}
void Node::updateRouteEntry(int dest, int next_hop, int cost)
{
    _routing_table[dest] = {dest, next_hop, cost};
}
int Node::getRouteCost(int dest) const
{
    auto it = _routing_table.find(dest);
    if (it != _routing_table.end())
    {
        return it->second.cost;
    }
    return 1e9;
}
const unordered_map<int, RouteEntry>& Node::getRoutingTable() const
{
    return _routing_table;
}

class Path
{
private:
    stack<Node *> _node_path;
    int _distance_traveled;
public:
    Path();
    Path(Node *node);
    Path(stack<Node *> previous_path, int distance);
    void addNodeToPath(Node *start_node, Node *end_node);
    Node *getTopNode();
    void dequeuePath();
    bool isEmpty() const;
    int getDistance() const;
    stack<Node *> getPath() const;
    stack<Node *> getPathQueue();
};

struct PathCompare
{
    bool operator()(const Path *path1, const Path *path2) const
    {
        return path1->getDistance() > path2->getDistance();
    }
};
Path::Path()
{
    _distance_traveled = 0;
}
Path::Path(Node *node)
{
    _node_path.push(node);
    _distance_traveled = 0;
}
Path::Path(stack<Node *> previous_path, int distance)
{
    _node_path = previous_path;
    _distance_traveled = distance;
}
void Path::addNodeToPath(Node *start_node, Node *end_node)
{
    _node_path.push(end_node);
    _distance_traveled += start_node->getWeight(end_node) * end_node->getLoadFactor();
}
Node* Path::getTopNode()
{
    return _node_path.top();
}
void Path::dequeuePath()
{
    _node_path.pop();
}
bool Path::isEmpty() const
{
    return _node_path.empty();
}
int Path::getDistance() const
{
    return _distance_traveled;
}
stack<Node *> Path::getPath() const
{
    return _node_path;
}
stack<Node *> Path::getPathQueue()
{
    stack<Node *> queue;
    while (_node_path.empty() == false)
    {
        Node *node = _node_path.top();
        queue.push(node);
        _node_path.pop();
    }
    return queue;
}

class Packet
{
private:
    char _value;
    int _order;
    int _current_wait;
    int _arrival_time;
    int _start_time;
    Node *_destination;
    Node *_previous_location;
    Node *_next_location;
    queue<Node *> _visited_nodes;
public:
    Packet(char value, int order, Node *destination, Node *start_location);
    char getValue() const;
    int getOrder() const;
    int getCurrentWait() const;
    Node *getDestination() const;
    Node *getPreviousLocation() const;
    Node *getNextLocation() const;
    int getArrivalTime() const;
    int getStartTime() const;
    void setPreviousLocation(Node *node);
    void setNextLocation(Node *node);
    void setCurrentWait(int wait);
    void setStartTime(int start);
    bool tick();
    void addVisitedNode(Node *node);
    int getTransmissionTime() {return _order;};
    int getPriority() {return _order;};
    queue<Node *> getVisitedNodes();
};
Packet::Packet(char value, int order, Node *destination, Node *start_location)
{
    _value = value;
    _order = order;
    _current_wait = -1; // Means not in the network yet
    _arrival_time = 0;
    _start_time = 0;
    _destination = destination;
    _previous_location = start_location;
    _next_location = nullptr;
}
char Packet::getValue() const
{
    return _value;
}
int Packet::getOrder() const
{
    return _order;
}
int Packet::getCurrentWait() const
{
    return _current_wait;
}
Node* Packet::getDestination() const
{
    return _destination;
}
Node* Packet::getPreviousLocation() const
{
    return _previous_location;
}
Node* Packet::getNextLocation() const
{
    return _next_location;
}
int Packet::getArrivalTime() const
{
    return _arrival_time;
}
int Packet::getStartTime() const
{
    return _start_time;
}
void Packet::setPreviousLocation(Node *node)
{
    _previous_location = node;
}
void Packet::setNextLocation(Node *node)
{
    _next_location = node;
}
void Packet::setCurrentWait(int wait)
{
    _current_wait = wait;
    _arrival_time += wait;
}
void Packet::setStartTime(int start)
{
    _arrival_time = start;
    _start_time = start;
}
bool Packet::tick()
{
    if (_current_wait > 0)
    {
        _current_wait--;
        return false;
    }
    
    else if (_current_wait == 0)
    {
        return true;
    }
    
    else
    {
        return true;
    }
}
void Packet::addVisitedNode(Node *node)
{
    _visited_nodes.push(node);
}
queue<Node *> Packet::getVisitedNodes()
{
    return _visited_nodes;
}
class Graph
{
private:
    unordered_map<int, Node *> _nodes;
public:
    void addNode(int node_id);
    void addEdge(int start_id, int end_id, int weight);
    void removeEdge(int start_id, int end_id);
    Node *getNode(int node_id);
    Path *computeShortestPathDijkstra(int start_id, int end_id);
    Path *computeShortestPathBellmanFord(int start_id, int end_id);
    vector<int> getNodeIds() const;
};
void Graph::addNode(int node_id)
{
    _nodes[node_id] = new Node(node_id);
}
void Graph::addEdge(int start_id, int end_id, int weight)
{
    _nodes[start_id]->addEdge(_nodes[end_id], weight);
}
Node* Graph::getNode(int node_id)
{
    return _nodes[node_id];
}
Path* Graph::computeShortestPathDijkstra(int start_id, int end_id)
{
    unordered_map<int, int> dist;
    unordered_map<int, Node *> parent;
    
    for (auto const& pair : _nodes)
    {
        dist[pair.first] = 1e9;
    }
    dist[start_id] = 0;
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start_id});
    
    while (!pq.empty())
    {
        pair<int, int> top_element = pq.top();
        pq.pop();
        int d = top_element.first;
        int u_id = top_element.second;
        
        if (d > dist[u_id]) continue;
        if (u_id == end_id) break;
        
        Node *u_node = _nodes[u_id];
        unordered_map<Node *, int> edges = u_node->getEdges();
        for (auto const& edge : edges)
        {
            Node *v_node = edge.first;
            int weight = edge.second;
            int v_id = v_node->getId();
            int cost = weight * v_node->getLoadFactor();
            
            if (dist[u_id] + cost < dist[v_id])
            {
                dist[v_id] = dist[u_id] + cost;
                parent[v_id] = u_node;
                pq.push({dist[v_id], v_id});
            }
        }
    }
    
    if (dist[end_id] == 1e9)
    {
        return nullptr;
    }
    
    vector<Node *> path_nodes;
    Node *curr = _nodes[end_id];
    while (curr->getId() != start_id)
    {
        path_nodes.push_back(curr);
        curr = parent[curr->getId()];
    }
    path_nodes.push_back(_nodes[start_id]);
    reverse(path_nodes.begin(), path_nodes.end());
    
    Path *shortest_path = new Path(_nodes[start_id]);
    for (size_t i = 1; i < path_nodes.size(); i++)
    {
        shortest_path->addNodeToPath(path_nodes[i - 1], path_nodes[i]);
    }
    
    return shortest_path;
}

Path* Graph::computeShortestPathBellmanFord(int start_id, int end_id)
{
    unordered_map<int, int> dist;
    unordered_map<int, Node *> parent;
    
    for (auto const& pair : _nodes)
    {
        dist[pair.first] = 1e9;
    }
    dist[start_id] = 0;
    
    int num_vertices = _nodes.size();
    for (int i = 0; i < num_vertices - 1; i++)
    {
        bool any_update = false;
        for (auto const& pair : _nodes)
        {
            int u_id = pair.first;
            Node *u_node = pair.second;
            if (dist[u_id] == 1e9) continue;
            
            unordered_map<Node *, int> edges = u_node->getEdges();
            for (auto const& edge : edges)
            {
                Node *v_node = edge.first;
                int weight = edge.second;
                int v_id = v_node->getId();
                int cost = weight * v_node->getLoadFactor();
                
                if (dist[u_id] + cost < dist[v_id])
                {
                    dist[v_id] = dist[u_id] + cost;
                    parent[v_id] = u_node;
                    any_update = true;
                }
            }
        }
        if (!any_update) break;
    }
    
    if (dist.find(end_id) == dist.end() || dist[end_id] == 1e9)
    {
        return nullptr;
    }
    
    vector<Node *> path_nodes;
    Node *curr = _nodes[end_id];
    while (curr->getId() != start_id)
    {
        path_nodes.push_back(curr);
        curr = parent[curr->getId()];
    }
    path_nodes.push_back(_nodes[start_id]);
    reverse(path_nodes.begin(), path_nodes.end());
    
    Path *shortest_path = new Path(_nodes[start_id]);
    for (size_t i = 1; i < path_nodes.size(); i++)
    {
        shortest_path->addNodeToPath(path_nodes[i - 1], path_nodes[i]);
    }
    
    return shortest_path;
}

void Graph::removeEdge(int start_id, int end_id)
{
    if (_nodes.find(start_id) != _nodes.end() && _nodes.find(end_id) != _nodes.end())
    {
        _nodes[start_id]->removeEdge(_nodes[end_id]);
    }
}

vector<int> Graph::getNodeIds() const
{
    vector<int> ids;
    for (auto const& pair : _nodes)
    {
        ids.push_back(pair.first);
    }
    sort(ids.begin(), ids.end());
    return ids;
}
class Message
{
private:
    queue<Packet *> _packets;
    Node *_starting_node;
    Node *_ending_node;
    int _size;
public:
    Message();
    Message(string message, Node *starting_node, Node *ending_node);
    bool isEmpty() const;
    Packet *getNextPacket();
    int getSize() const;
};
Message::Message()
{
    _starting_node = nullptr;
    _ending_node = nullptr;
    _size = 0;
}
Message::Message(string message, Node *starting_node, Node *ending_node)
{
    _starting_node = starting_node;
    _ending_node = ending_node;
    for (int index = 0; index < message.length(); index++)
    {
        _packets.push(new Packet(message[index], index, ending_node, starting_node));
    }
    _size = _packets.size();
}
bool Message::isEmpty() const
{
    return _packets.empty();
}
Packet* Message::getNextPacket()
{
    Packet *packet = _packets.front();
    _packets.pop();
    return packet;
}
int Message::getSize() const
{
    return _size;
}
class StringSplitter
{
public:
    
    static string * split(string text, string delimiter, int &items_found)
    {
        //vectors are dynamically expanding arrays
        vector<string> pieces;
        
        //find the first delimiter
        int location = text.find(delimiter);
        
        //we are starting at the beginning of our string
        int start = 0;
        
        //go until we have no more delimiters
        while(location != string::npos)
        {
            //add the current piece to our list of pieces
            string piece = text.substr(start, location - start);
            pieces.push_back(piece);
            
            //update our index markers for the next round
            start = location + 1;
            location = text.find(delimiter, start);
        }
        
        //at the end of our loop, we're going to have one trailing piece to take care of.
        //handle that now.
        string piece = text.substr(start, location - start);
        pieces.push_back(piece);
        
        //convert from vector into an array of strings
        int size = pieces.size();
        string *pieces_str = new string[size];
        for(int i = 0; i < size; i++)
        {
            pieces_str[i] = pieces.at(i);
        }
        items_found = size;
        return pieces_str;			
    }
};
vector<Packet*> schedulePacketsDP(vector<Packet*>& packets) {
    // Sort packets by finish time (current implementation time + transmission time)
    sort(packets.begin(), packets.end(), [](Packet* a, Packet* b) {
        return (a->getArrivalTime() + a->getTransmissionTime()) < 
               (b->getArrivalTime() + b->getTransmissionTime());
    });

    int n = packets.size();
    vector<int> dp(n + 1, 0);
    vector<int> prev(n + 1, -1);

    // Precompute closest non-conflicting packet
    vector<int> lastNonConflict(n, -1);
    for (int i = 1; i < n; i++) {
        for (int j = i-1; j >= 0; j--) {
            if (packets[j]->getArrivalTime() + packets[j]->getTransmissionTime() <= 
                packets[i]->getArrivalTime()) {
                lastNonConflict[i] = j;
                break;
            }
        }
    }

    // DP table construction
    for (int i = 1; i <= n; i++) {
        int inclProfit = packets[i-1]->getPriority();
        if (lastNonConflict[i-1] != -1)
            inclProfit += dp[lastNonConflict[i-1] + 1];
        
        if (inclProfit > dp[i-1]) {
            dp[i] = inclProfit;
            prev[i] = lastNonConflict[i-1] + 1;
        } else {
            dp[i] = dp[i-1];
        }
    }

    // Backtrack to find selected packets
    vector<Packet*> result;
    int i = n;
    while (i > 0) {
        if (dp[i] != dp[i-1]) {
            result.push_back(packets[i-1]);
            i = prev[i];
        } else {
            i--;
        }
    }
    reverse(result.begin(), result.end());
    return result;
}
int fordFulkerson(const vector<vector<int>>& capacity,int source,int sink){
    int n = capacity.size();
    // residual capacities initialized to original capacities
    vector<vector<int>> residual = capacity;
    vector<int> parent(n);
    int maxFlow = 0;

    // Helper: find an augmenting path via BFS, fill parent[], return flow
    auto bfs = [&]() -> int {
    fill(parent.begin(), parent.end(), -1);
    parent[source] = source;
    queue<pair<int,int>> q;
    q.push({source, numeric_limits<int>::max()});

    while (!q.empty()) {
    auto it = q.front();
    int u = it.first;
    int flow = it.second;
    q.pop();

    for (int v = 0; v < n; ++v) {
    if (parent[v] == -1 && residual[u][v] > 0) {
        // possible to send flow
        parent[v] = u;
        int new_flow = min(flow, residual[u][v]);
        if (v == sink) {
            return new_flow;
        }
        q.push({v, new_flow});
    }
    }
    }
    return 0; // no augmenting path found
    };

    // Repeatedly find augmenting paths and update residual graph
    while (int flow = bfs()) {
    maxFlow += flow;
    int v = sink;
    // walk back from sink to source via parent[]
    while (v != source) {
    int u = parent[v];
    residual[u][v] -= flow;      // reduce forward capacity
    residual[v][u] += flow;      // increase reverse capacity
    v = u;
    }
    }

    return maxFlow;
}
class Network
{
public:
    enum RoutingAlgorithm
    {
        DIJKSTRA,
        BELLMAN_FORD
    };
private:
    Graph _graph;
    int _tick_count;
    Message _message;
    unordered_map<int, Packet *> _packets;
    unordered_map<int, Packet *> _delivered;
    RoutingAlgorithm _routing_algorithm;
    long long _total_routing_time_ns;
public:
    Network();
    void addNode(int node_id);
    void addEdge(int start_id, int end_id, int weight);
    void removeEdge(int start_id, int end_id);
    void addMessage(string message, int start_id, int end_id);
    Path *computeShortestPath(int start_id, int end_id);
    void createGraph(ifstream &input_file);
    void tick();
    bool isRunning() const;
    void displayResults();
    
    void setRoutingAlgorithm(RoutingAlgorithm algorithm);
    void printRoutingTables();
    void displayPerformanceMetrics();
    void resetRoutingTime();
    
    void initializeRoutingTables();
    void runDistanceVectorExchange();
};
Network::Network()
{
    _tick_count = 0;
    _routing_algorithm = DIJKSTRA;
    _total_routing_time_ns = 0;
}
void Network::addNode(int node_id)
{
    _graph.addNode(node_id);
}
void Network::addEdge(int start_id, int end_id, int weight)
{
    _graph.addEdge(start_id, end_id, weight);
}
void Network::removeEdge(int start_id, int end_id)
{
    _graph.removeEdge(start_id, end_id);
}
void Network::addMessage(string message, int start_id, int end_id)
{
    _message = Message(message, _graph.getNode(start_id), _graph.getNode(end_id));
}
Path* Network::computeShortestPath(int start_id, int end_id)
{
    if (_routing_algorithm == DIJKSTRA)
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        Path *path = _graph.computeShortestPathDijkstra(start_id, end_id);
        auto end_time = std::chrono::high_resolution_clock::now();
        _total_routing_time_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        return path;
    }
    else
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        Path *path = nullptr;
        Node *curr = _graph.getNode(start_id);
        Node *dest = _graph.getNode(end_id);
        if (curr != nullptr && dest != nullptr)
        {
            path = new Path(curr);
            Node *temp = curr;
            unordered_set<int> visited;
            visited.insert(temp->getId());
            while (temp->getId() != end_id)
            {
                const auto& routing_table = temp->getRoutingTable();
                auto it = routing_table.find(end_id);
                if (it == routing_table.end())
                {
                    delete path;
                    path = nullptr;
                    break;
                }
                int next_hop_id = it->second.nextHop;
                if (next_hop_id == -1 || visited.find(next_hop_id) != visited.end())
                {
                    delete path;
                    path = nullptr;
                    break;
                }
                Node *next_node = _graph.getNode(next_hop_id);
                path->addNodeToPath(temp, next_node);
                temp = next_node;
                visited.insert(temp->getId());
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        _total_routing_time_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        return path;
    }
}

void Network::initializeRoutingTables()
{
    vector<int> node_ids = _graph.getNodeIds();
    for (int src_id : node_ids)
    {
        Node *src_node = _graph.getNode(src_id);
        src_node->clearRoutingTable();
        src_node->updateRouteEntry(src_id, src_id, 0);
        
        unordered_map<Node *, int> edges = src_node->getEdges();
        unordered_set<int> neighbors;
        for (auto const& edge : edges)
        {
            int dest_id = edge.first->getId();
            int weight = edge.second;
            src_node->updateRouteEntry(dest_id, dest_id, weight);
            neighbors.insert(dest_id);
        }
        
        for (int dest_id : node_ids)
        {
            if (dest_id != src_id && neighbors.find(dest_id) == neighbors.end())
            {
                src_node->updateRouteEntry(dest_id, -1, 1e9);
            }
        }
    }
}

void Network::runDistanceVectorExchange()
{
    initializeRoutingTables();
    vector<int> node_ids = _graph.getNodeIds();
    int iteration = 0;
    bool converged = false;
    
    while (!converged)
    {
        iteration++;
        bool any_update = false;
        unordered_map<int, unordered_map<int, RouteEntry>> old_tables;
        for (int node_id : node_ids)
        {
            old_tables[node_id] = _graph.getNode(node_id)->getRoutingTable();
        }
        
        bool printed_iteration_header = false;
        
        for (int u_id : node_ids)
        {
            Node *u_node = _graph.getNode(u_id);
            unordered_map<Node *, int> edges = u_node->getEdges();
            
            for (auto const& edge : edges)
            {
                Node *v_node = edge.first;
                int v_id = v_node->getId();
                int cost_to_v = edge.second;
                
                const auto& v_table = old_tables[v_id];
                
                for (int d_id : node_ids)
                {
                    if (d_id == u_id) continue;
                    
                    if (v_table.find(d_id) != v_table.end())
                    {
                        int v_cost_to_d = v_table.at(d_id).cost;
                        if (v_cost_to_d == 1e9) continue;
                        
                        int new_cost = cost_to_v + v_cost_to_d;
                        int current_cost = u_node->getRouteCost(d_id);
                        
                        if (new_cost < current_cost)
                        {
                            if (!printed_iteration_header)
                            {
                                cout << "Iteration " << iteration << endl;
                                printed_iteration_header = true;
                            }
                            
                            int old_cost = current_cost;
                            u_node->updateRouteEntry(d_id, v_id, new_cost);
                            any_update = true;
                            
                            cout << "Router " << u_id << " updated route to " << d_id << endl;
                            cout << "Old Cost : " << (old_cost == 1e9 ? -1 : old_cost) << endl;
                            cout << "New Cost : " << new_cost << endl;
                            cout << "via Router " << v_id << endl;
                        }
                    }
                }
            }
        }
        
        if (!any_update)
        {
            converged = true;
            cout << "Distance Vector converged after " << iteration - 1 << " iterations." << endl;
        }
    }
}

void Network::setRoutingAlgorithm(RoutingAlgorithm algorithm)
{
    _routing_algorithm = algorithm;
}

void Network::printRoutingTables()
{
    vector<int> node_ids = _graph.getNodeIds();
    for (int src_id : node_ids)
    {
        cout << "Node " << src_id << endl;
        cout << left << setw(13) << "Destination" << setw(9) << "NextHop" << "Cost" << endl;
        for (int dest_id : node_ids)
        {
            if (src_id == dest_id) continue;
            Path *path = computeShortestPath(src_id, dest_id);
            if (path != nullptr && !path->isEmpty())
            {
                stack<Node *> path_stack = path->getPathQueue();
                if (path_stack.size() >= 2)
                {
                    Node *prev = path_stack.top();
                    path_stack.pop();
                    Node *next = path_stack.top();
                    int next_hop = next->getId();
                    int cost = path->getDistance();
                    cout << left << setw(13) << dest_id << setw(9) << next_hop << cost << endl;
                }
                delete path;
            }
        }
        cout << endl;
    }
}

void Network::displayPerformanceMetrics()
{
    cout << "\n==================================" << endl;
    cout << "Performance Metrics Summary" << endl;
    cout << "==================================" << endl;
    
    cout << "Routing Algorithm: ";
    if (_routing_algorithm == DIJKSTRA)
    {
        cout << "Dijkstra (Link State)" << endl;
    }
    else
    {
        cout << "Bellman-Ford (Distance Vector)" << endl;
    }
    
    double execution_time_ms = (double)_total_routing_time_ns / 1e6;
    cout << "Execution Time: " << fixed << setprecision(3) << execution_time_ms << " ms ("
         << _total_routing_time_ns / 1000 << " us)" << endl;
    
    int packets_delivered = _delivered.size();
    cout << "Packets Delivered: " << packets_delivered << endl;
    
    double total_delay = 0;
    int max_delay = 0;
    for (int index = 0; index < _message.getSize(); index++)
    {
        if (_delivered.find(index) != _delivered.end() && _delivered[index] != nullptr)
        {
            int delay = _delivered[index]->getArrivalTime() - _delivered[index]->getStartTime();
            total_delay += delay;
            if (delay > max_delay)
            {
                max_delay = delay;
            }
        }
    }
    
    double avg_delay = (packets_delivered > 0) ? (total_delay / packets_delivered) : 0.0;
    cout << "Average Packet Delay: " << fixed << setprecision(2) << avg_delay << " ticks" << endl;
    cout << "Maximum Packet Delay: " << max_delay << " ticks" << endl;
    cout << "Total Simulation Ticks: " << _tick_count << endl;
}

void Network::resetRoutingTime()
{
    _total_routing_time_ns = 0;
}
void Network::createGraph(ifstream &input_file)
{
    string line;
    int items_found = 0;
    while (input_file.good() == true)
    {
        getline(input_file, line);
        strip_cr(line);
        if (line.empty()) continue;
        string *pieces = StringSplitter::split(line, " ", items_found);
        if (items_found == 1)
        {
            addNode(stoi(pieces[0]));
        }
        
        else if (items_found == 3)
        {
            addEdge(stoi(pieces[0]), stoi(pieces[1]), stoi(pieces[2]));
        }
        
        else
        {
            exit(0);
        }
    }
}
void Network::tick()
{
    _tick_count++;
    if (_message.isEmpty() == false)
    {
        Packet *packet = _message.getNextPacket();
        Path *path = computeShortestPath(packet->getPreviousLocation()->getId(), packet->getDestination()->getId());
        stack<Node *> path_stack = path->getPathQueue();
        Node *previous = path_stack.top();
        path_stack.pop(); 
        Node *next = path_stack.top();
        path_stack.pop();
        packet->setPreviousLocation(previous);
        packet->setNextLocation(next);
        packet->setStartTime(_tick_count);
        packet->setCurrentWait(_graph.getNode(previous->getId())->getWeight(next) * next->getLoadFactor());
        packet->addVisitedNode(previous);
        previous->increaseLoadFactor();
        next->increaseLoadFactor();
        _packets[packet->getOrder()] = packet;
        cout << "Sending packet " << packet->getValue() << " to vertex " << packet->getNextLocation()->getId() <<
        " with a wait of " << packet->getCurrentWait() << " at time " << _tick_count << endl;
        delete path;
    }
    
    for (unordered_map<int, Packet *>::iterator it = _packets.begin(); it != _packets.end(); it++)
    {
        bool finished = it->second->tick();
        if (finished == true)
        {
            if (it->second->getDestination() != it->second->getNextLocation())
            {
                it->second->getPreviousLocation()->decreaseLoadFactor();
                it->second->getNextLocation()->decreaseLoadFactor();
                Path *path = computeShortestPath(it->second->getNextLocation()->getId(), it->second->getDestination()->getId());
                stack<Node *> path_stack = path->getPathQueue();
                it->second->addVisitedNode(it->second->getNextLocation());
                Node *previous = path_stack.top();
                path_stack.pop();
                Node *next = path_stack.top();
                path_stack.pop();
                it->second->setPreviousLocation(previous);
                it->second->setNextLocation(next);
                it->second->setCurrentWait(_graph.getNode(previous->getId())->getWeight(next) * next->getLoadFactor());
                previous->increaseLoadFactor();
                next->increaseLoadFactor();
                cout << "Sending packet " << it->second->getValue() << " to vertex " << it->second->getNextLocation()->getId() <<
                " with a wait of " << it->second->getCurrentWait() << " at time " << _tick_count << endl;
                delete path;
            }
            
            else if (_delivered[it->second->getOrder()] == nullptr)
            {
                it->second->addVisitedNode(it->second->getNextLocation());
                it->second->getPreviousLocation()->decreaseLoadFactor();
                it->second->getNextLocation()->decreaseLoadFactor();
                _delivered[it->second->getOrder()] = it->second;
            }
        }
    }
}
bool Network::isRunning() const
{
    if (_delivered.size() == _message.getSize())    // terminates when message size and delivered map size becomes equal
    {
        return false;
    }
    else
    {
        return true;
    }
}
void Network::displayResults()
{
    cout << "Packet - Arrival Time - Route" << endl;
    for (int index = 0; index < _message.getSize(); index++)
    {
        cout << _delivered[index]->getValue() << setw(10) << _delivered[index]->getArrivalTime() << setw(14);
        queue<Node *> visited = _delivered[index]->getVisitedNodes();
        while (visited.empty() == false)
        {
            cout << visited.front()->getId();
            if (visited.size() > 1)
            {
                cout << ", ";
                visited.pop();
            }
            else
            {
                cout << endl;
                visited.pop();
            }
        }
    }
}
int main()
{
    Network network;
    string line;
    ifstream input_file;
    int start_id = 0, end_id = 0;

    cout << "Choose Routing Algorithm" << endl;
    cout << "1. Dijkstra (Link State)" << endl;
    cout << "2. Bellman-Ford (Distance Vector)" << endl;
    cout << "Enter choice: ";
    string choice_str;
    getline(cin, choice_str);
    strip_cr(choice_str);
    int choice = 1;
    try
    {
        choice = stoi(choice_str);
    }
    catch (...)
    {
        choice = 1;
    }

    if (choice == 2)
    {
        network.setRoutingAlgorithm(Network::BELLMAN_FORD);
    }
    else
    {
        network.setRoutingAlgorithm(Network::DIJKSTRA);
    }

    cout << "Enter graph file: ";
    getline(cin, line);
    strip_cr(line);
    input_file.open(line);
    if (!input_file.is_open())
    {
        cout << "Error: Could not open graph file: " << line << endl;
        return 1;
    }
    network.createGraph(input_file);

    if (choice == 2)
    {
        cout << "\nRunning Distance Vector routing protocol updates..." << endl;
        network.runDistanceVectorExchange();
    }

    cout << "\nRouting Tables:" << endl;
    network.printRoutingTables();

    // Optional Bonus: Dynamic topology updates
    while (true)
    {
        cout << "Do you want to update topology? (y/n): ";
        string ans;
        getline(cin, ans);
        strip_cr(ans);
        if (ans != "y" && ans != "Y")
        {
            break;
        }

        cout << "1. Add/Update edge" << endl;
        cout << "2. Remove edge" << endl;
        cout << "Choose option: ";
        string opt_str;
        getline(cin, opt_str);
        strip_cr(opt_str);
        int opt = 0;
        try
        {
            opt = stoi(opt_str);
        }
        catch (...)
        {
        }

        if (opt == 1)
        {
            cout << "Enter start node, end node, and weight (space-separated): ";
            string edge_line;
            getline(cin, edge_line);
            strip_cr(edge_line);
            int start_node, end_node, weight;
            stringstream ss(edge_line);
            if (ss >> start_node >> end_node >> weight)
            {
                network.addEdge(start_node, end_node, weight);
                cout << "Edge added/updated successfully!" << endl;
            }
            else
            {
                cout << "Invalid input!" << endl;
            }
        }
        else if (opt == 2)
        {
            cout << "Enter start node and end node (space-separated): ";
            string edge_line;
            getline(cin, edge_line);
            strip_cr(edge_line);
            int start_node, end_node;
            stringstream ss(edge_line);
            if (ss >> start_node >> end_node)
            {
                network.removeEdge(start_node, end_node);
                cout << "Edge removed successfully!" << endl;
            }
            else
            {
                cout << "Invalid input!" << endl;
            }
        }

        if (choice == 2)
        {
            cout << "\nRunning Distance Vector routing protocol updates..." << endl;
            network.runDistanceVectorExchange();
        }

        cout << "\nRecomputed Routing Tables:" << endl;
        network.printRoutingTables();
    }

    cout << "Enter a starting vertex: ";
    getline(cin, line);
    strip_cr(line);
    start_id = stoi(line);
    cout << "Enter a destination vertex: ";
    getline(cin, line);
    strip_cr(line);
    end_id = stoi(line);
    cout << "Enter a message to transmit: ";
    getline(cin, line);
    strip_cr(line);
    network.addMessage(line, start_id, end_id);

    // Reset routing timers right before the tick-based simulation starts
    network.resetRoutingTime();

    while (network.isRunning() == true)
    {
        network.tick();
    }
    network.displayResults();
    network.displayPerformanceMetrics();
    return 0;
}