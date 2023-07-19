#include <vector> 
#include <iostream>
#include <random>
#include <assert.h>
#include <iomanip>
#include <cmath>

using namespace std;

/* Implements min-heap and graph as classes with their basic operations.
 Also, it implements Dijkstra's algorithm and stores its solution in a ShortestPath class (implemented too).
 Contains examples to verify algorithm's functioning */

//Naming long datastructores as easy objects
typedef vector<vector<float>> FloatMatrix;
typedef vector<int> IntVector;
typedef vector<float> FloatVector;

//Setting generator number, returns a random float
float generateRandomFloat(float min, float max) {
    random_device rd;  // Seed the random number generator
    default_random_engine generator(rd());
    uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

//Template for printing matrices
template<typename T>
ostream& operator<<(ostream &os, const vector<vector<T>> & matrix){
    for (int i = 0; i < matrix.size(); i++ ){
        for(int j = 0; j < matrix[0].size(); j++){
            cout<< setprecision(4) << setfill(' ') << setw(6) << matrix[i][j]<< ", ";
        }
        cout<< endl;
    }
    cout<< endl;
    return os;
}

//Template for printing vectors
template<typename T>
ostream& operator<<(ostream &os, vector<T>  & vec){
    for (int i = 0; i < vec.size(); i++ ){
        cout<< setprecision(4) << setfill(' ') << setw(6) << vec[i]<< ", ";
        }
        cout<< endl;
    cout<< endl;
    return os;
}

/* Function to generate random float matrices to be used as adjacent matrices whose values range between [1.0, max]
Density is a number in the interval [0, 1.] that controls the probability of having a edge added to a vertex
n_vertices is the number of nodes in the graph */
FloatMatrix geneateRandomMatrix(int n_vertices, float density, float max){
    assert(density>0.0 && density < 1.0 && max > 0.0);
    FloatMatrix matrix(n_vertices, FloatVector(n_vertices,0));

    for (int i = 0; i < matrix.size(); i++ ){
        for(int j = 0; j < matrix[0].size(); j++){
            if (generateRandomFloat(0.0, 1.0) < density && i!=j){
                matrix[i][j] = generateRandomFloat(1.0, max);
            }
            else{
                matrix[i][j] = 0.0;
            }
        }
    }
    return matrix;
}

//Implement the graph class declared using and adjacency matrix and its basic operations. Also it has a vector storing the node values
class graph{
private:
    FloatMatrix adjacency_matrix;
    FloatVector node_values;

public:
    graph(FloatMatrix adjacency_matrix ) 
    : adjacency_matrix(adjacency_matrix), node_values(FloatVector(adjacency_matrix.size(), 0.0)){}
    
    inline int vertices() {
        return adjacency_matrix.size();}
    
    int edges(){
        int n_edges = 0;
        for (int i = 0; i < adjacency_matrix.size(); i++ ){
            for(int j = 0; j < adjacency_matrix[0].size(); j++){
                if (adjacency_matrix[i][j] > 0.0){
                    n_edges++;}}}
        return n_edges;
    }
    
    inline bool adjacent(int u, int v){
        return adjacency_matrix[u][v] > 0.0;}
    
    IntVector neighbors(int u){
        assert(u < vertices());
        IntVector neighbors_vector;
        for (int j = 0; j < vertices(); j++){
            if (adjacency_matrix[u][j]>0.0){
                neighbors_vector.push_back(j);}}
        return neighbors_vector;
    }
    
    inline void setNodeValue(int u, float value){
        assert(u < vertices() && u >= 0);
        node_values[u] = value;}

    inline float getNodeValue(int u){
        assert(u < vertices() && u >= 0);
        return node_values[u];}
    
    inline void addSetEdgeValue(int u, int v, float value){
        assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
        adjacency_matrix[u][v] = value;}
    
    inline float getEdgeValue(int u, int v){
        assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
        return adjacency_matrix[u][v];}
    
    inline void addDeleteEdge(int u, int v){
        addSetEdgeValue(u, v, 0.0);}
    
    void initializeNodes(int start){
        node_values = FloatVector( vertices(), INFINITY );
        node_values[start] = 0.0;}
};

//Creates a class for each node of a MinHeap with two fields: its name and its priority
class MinHeapNode{
public:
    int name;
    float priority;
    MinHeapNode(int nam = 0, float prior = INFINITY):
        name(nam), priority(prior) {}
    };

//Creates the class for a MinHeap to be used as a priority queue and its basic operations
class MinHeap {
private:
    vector<MinHeapNode> heap;

    void heapifyUp(int index) {// Implements heapifyUp algorithm to fix MinHeap property
        int parent = (index - 1) / 2;

        while (index > 0 && heap[index].priority < heap[parent].priority ) {
            swap(heap[index], heap[parent]);
            index = parent;
            parent = (index - 1) / 2; 
        }
    }

    void restorationDown(){ // Assumes that the root and the last element were swapped
        int node_index = 0;
        int left_child = 2*node_index + 1, right_child = 2*node_index + 2;

        while( ( left_child < heap.size()  && heap[node_index].priority > heap[left_child].priority ) || \
               ( right_child < heap.size() && heap[node_index].priority > heap[right_child].priority) ) {
            //If there is no right child, so the left one is unique with smaller priority or there is a right child with higher priority than the left one
            if ( right_child  >= heap.size() || heap[left_child].priority <= heap[right_child].priority ) { 
                swap(heap[node_index], heap[left_child]);
                node_index = left_child;
            }
            
            else {// Right child exists and its priority is smaller than the left one and the parent
                swap(heap[node_index],heap[right_child]);
                node_index = right_child;
            }

            left_child = 2*node_index + 1, right_child = 2*node_index + 2;
        }
     }

public:
    void insert(MinHeapNode node) {
        heap.push_back(node); // Add element to the end of the heap
        heapifyUp(heap.size() - 1);} // Restore the heap property by moving the element up if needed
    
    MinHeapNode extract(){
        MinHeapNode min = heap[0];
        swap(heap[0], heap[heap.size()-1]);
        heap.pop_back();
        restorationDown(); //Moves the root node dow to preserve MinHeap property
        return min;}

    inline MinHeapNode top(){return heap[0];}

    inline int size(){return heap.size();};

    void changePriority(int node_name, float priority){
        int index;
        for(int i = 0; i < size(); i++ ){
            if (heap[i].name == node_name){
                assert(heap[i].priority >= priority);
                heap[i].priority = priority;
                index = i;}}
        heapifyUp(index);
    }

    inline bool contains(int node_name){
        for(int i = 0; i < size(); i++ ){
            if (heap[i].name == node_name){              
                return true;}}
        return false;
    }

    inline void printHeap() {
        for (int i= 0; i <heap.size(); i++) {
            cout <<  "Name: " <<heap[i].name << " priority: " << heap[i].priority << endl;}
    }    
};

//Class in charge of storing the shortest path of a graph
class ShortestPath {
    IntVector path;
    float path_size = 0.0;

public:
    //Creates the shortest path given a graph, a previous vector from Dijktra's algorithm and the source and goal positions
    ShortestPath(graph G, IntVector previous, int start, int goal){
        int current_node = goal;

        if (previous[current_node] == -1){ //When Dijkstra's algorithm does not find any solution
            printf("No solution found\n");
            path.insert( path.begin(), current_node );
            path_size = -1.0;
        }

        else{            
        while ( current_node != start ){
            path.insert( path.begin(), current_node );
            path_size += G.getEdgeValue( previous[current_node], current_node );
            current_node = previous[current_node];
        }
        path.insert( path.begin(), current_node );
        path_size = G.getNodeValue( goal );
        }
    }

    inline IntVector getPath(){return path;}
    inline float getSize(){return path_size;}
    };

//Overriding << operator for the ShortestPath
ostream& operator<<(ostream &os, ShortestPath & shortest_path){
    IntVector shp = shortest_path.getPath();
    cout<<"Shortest path between "<<shp[0] <<" and "<< shp[shp.size()-1] << " with a length of " << shortest_path.getSize()<<endl;

    for (int i = 0; i < shp.size(); i++ ){
        cout<< setprecision(4) << setfill(' ') << setw(6) << shp[i]<< ", ";
        }
    cout<< endl;
    return os;}

//Creates and initializes a heap for a graph with a
MinHeap initialize(int n_vertices, int start){
    MinHeap heap;
    for(int i = 0; i < n_vertices; i++){
        if (i!= start){
            heap.insert(MinHeapNode(i, INFINITY ));}

        else{
            heap.insert(MinHeapNode(i, 0.0 ));}
    }
        return heap;}

//Implements Dijkstra's algorithim. It stops when the goal is reached (delete !current_distances.contains(goal) at line 274 to generate each shortest path)
ShortestPath dijkstra(graph G, int start, int goal){
    MinHeapNode current_node, neighbor;
    float new_distance;
    IntVector curr_node_neighbors;

    MinHeap current_distances = initialize(G.vertices(), start);
    IntVector previous = IntVector(G.vertices(), -1);
    previous[start] = start;
    G.initializeNodes(start);
    
    while( !current_distances.contains(goal) || current_distances.size() > 0){
        current_node = current_distances.extract();

        if ( current_node.name == goal ) {break;}
        
        curr_node_neighbors = G.neighbors( current_node.name );

        for(int i = 0; i < curr_node_neighbors.size(); i++ ){
            neighbor = curr_node_neighbors[i];

            if (! current_distances.contains( neighbor.name )){continue;}

            new_distance = G.getNodeValue( current_node.name ) + G.getEdgeValue( current_node.name, neighbor.name);
            
            if (new_distance < G.getNodeValue( neighbor.name )){
                G.setNodeValue( neighbor.name, new_distance);
                previous[neighbor.name] = current_node.name;
                current_distances.changePriority( neighbor.name, new_distance);
            }
        }
    }

    return ShortestPath(G, previous, start, goal);}

//Function to test implementation, contains solved examples with customizable sources and goals
void test(int example, int source = 0, int goal = 4){
    FloatMatrix matrix;
    string web_source;

    switch (example){
    case 1: // https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif
        web_source = "https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif";
        matrix = {
            { 0.0, 7.0, 9.0, 0.0, 0.0, 14.0},
            {7.0, 0.0, 10.0, 15.0, 0.0, 0.0},
            {9.0, 10.0, 0.0, 11.0, 0.0, 2.0},
            {0.0, 15.0, 11.0, 0.0, 6.0, 0.0},
            {0.0, 0.0, 0.0, 0.0, 6.0, 0.0, 9.0},
            {14.0, 0.0, 2.0, 0.0, 9.0, 0.0}};
            break;
    case 2: // https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
        web_source = "https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/";
        matrix = {
            {0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8.0, 0.0},
            {4.0, 0.0, 8.0, 0.0, 0.0, 0.0, 0.0, 11.0, 0.0},
            {0.0, 8.0, 0.0, 7.0, 0.0, 4.0, 0.0, 0.0, 2.0},
            {0.0, 0.0, 7.0, 0.0, 9.0, 14.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 9.0, 0.0, 10.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 4.0, 14.0, 10.0, 0.0, 2.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 1.0, 6.0},
            {8.0, 11.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 7.0},
            {0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 6.0, 7.0, 0.0}};
            break;
    case 3: // https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif
        web_source = "https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif (disconnected version)";
        matrix = {
            { 0.0, 7.0, 9.0, 0.0, 0.0, 14.0},
            {7.0, 0.0, 10.0, 15.0, 0.0, 0.0},
            {9.0, 10.0, 0.0, 11.0, 0.0, 2.0},
            {0.0, 15.0, 11.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
            {14.0, 0.0, 2.0, 0.0, 0.0, 0.0}};
            break;
    default:
        cout << "No valid number provided"<< endl;
        break;
    }
    graph G = graph(matrix);
    ShortestPath solution = dijkstra(G, source, goal);
    cout<<"Solution example from: "<< web_source <<endl<< solution;
}

/* Computes the average shortest distance between node 0 and its neighbors in a randomly
generated graph with a given number of nodes and a vector of densities */
void averageDistance(int n_nodes = 50, FloatVector densities = {0.2, 0.4} ){

    FloatMatrix matrix;
    
    float avg_distance;
    graph G = geneateRandomMatrix(n_nodes, 0.5, 10.0);

    for (float density: densities){
        avg_distance = 0.0;
        matrix = geneateRandomMatrix(n_nodes, density, 10.0);
        G = graph(matrix);
        for (int i = 1; i < n_nodes; i++){
            avg_distance += dijkstra(G, 0, i).getSize();}
        printf("Average distance is %.4f for a node with %d nodes and density of %.2f\n", avg_distance/(n_nodes-1), n_nodes, density);
    }
}

int main(){

    averageDistance();
    // test(2);

    return 0;
}