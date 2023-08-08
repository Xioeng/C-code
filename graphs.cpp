#include <vector> 
#include <iostream>
#include <fstream>
#include <random>
#include <assert.h>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

/* Implements 5 clasees:
    -HeapNode: Node with value and priority, base of a MinHeap
    -MinHeap: a min-heap to manage min priorties (using a HeapNode to represent the nodes)
    -Graph: graph with basic methods attributes, whose nodes can have values and are indexed by integers, and
    the edges are saved using an adjacency matrix.
    -ShortestPath: designed for  Dijkstra's algorithm to store and build the shortest path between a source and goal nodes
    -Triple: Class to save triples for MST with the edges and the weight for that edge (edge1, edge2, weight)

 Implements Dijkstra's, Prim's and Kruskal's algorithms
 -Dijktra's solution is stored in a ShortestPath class 
 -Prrim's and Kruskal's MSTs are stored as vectors of Triples 
 Contains examples to verify algorithm's functioning */


//Setting generator number, it returns a random float
float generateRandomFloat(float min, float max) {
    random_device rd;  // Seed the random number generator
    default_random_engine generator(rd());
    uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

/*---------------------------------------------------------------------------------------------------------------------
---------------------------------------------Printing Templates--------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------*/

//Template for printing matrices
template<typename T>
ostream& operator<<(ostream &os, const vector<vector<T>> & matrix){
    for (const auto & row : matrix ){
        for(const auto & element : row){
            cout<< setprecision(4) << setfill(' ') << setw(6) << element<< ", ";
        }
        cout<< endl;
    }
    cout<< endl;
    return os;
}


//Template for printing vectors
template<typename T>
ostream& operator<<(ostream &os, vector<T>  & vec){
    int k = 0;
    for (const auto & element : vec){
        cout<< setprecision(4) << setfill(' ') << setw(6) << element<< ", ";
        if (++k % 7 == 0){ cout<<endl;}
        }
    cout<< endl;
    return os;
}

/* Function to generate random float matrices to be used as adjacent matrices whose values range between [1.0, max]
Density is a number in the interval [0, 1.] that controls the probability of having a edge added to a vertex
n_vertices is the number of nodes in the graph */
vector<vector<float>> geneateRandomMatrix(int n_vertices, float density, float max){
    assert(density>0.0 && density < 1.0 && max > 0.0);
    vector<vector<float>> matrix(n_vertices, vector<float>(n_vertices,0));

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

/*---------------------------------------------------------------------------------------------------------------------
---------------------------------------------------Classes-------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------*/

//Implement the graph class declared using and adjacency matrix and its basic operations. Also it has a vector storing the node values
class Graph{
private:
    vector<vector<float>> adjacency_matrix;
    vector<float> node_values;

public:
    Graph(vector<vector<float>> adjacency_matrix ) 
    : adjacency_matrix(adjacency_matrix), node_values(vector<float>(adjacency_matrix.size(), 0.0)){}
    
    inline int vertices() {
        return adjacency_matrix.size();}
    
    int edges(){
        int n_edges = 0;
        for (const auto & row : adjacency_matrix){
            for(const auto & element : row){
                if (element > 0.0){n_edges++;}
            }
        }
        return n_edges;
    }

    vector<vector<float>> adjacencyMatrix() {
        return adjacency_matrix;}
    
    inline bool adjacent(int u, int v){
        return adjacency_matrix[u][v] > 0.0;}
    
    vector<int> neighbors(int u){
        assert(u < vertices());
        vector<int> neighbors_vector;
        for (int j = 0; j < vertices(); j++){
            if (adjacency_matrix[u][j]>0.0){
                neighbors_vector.push_back(j);
            }
        }
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
        node_values = vector<float>( vertices(), INFINITY );
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
        heapifyUp(heap.size() - 1); // Restore the heap property by moving the element up if needed
        } 
    
    MinHeapNode extract(){
        MinHeapNode min = heap[0];
        swap(heap[0], heap[heap.size()-1]);
        heap.pop_back();
        restorationDown(); //Moves the root node dow to preserve MinHeap property
        return min;
        }

    inline MinHeapNode top(){return heap[0];}

    inline int size(){return heap.size();};

    void changePriority(int node_name, float priority){
        int index;
        for(int i = 0; i < size(); i++ ){
            if (heap[i].name == node_name){
                assert(heap[i].priority >= priority);
                heap[i].priority = priority;
                index = i;
            }
        }
        heapifyUp(index);
    }

    inline bool contains(int node_name){
        for(int i = 0; i < size(); i++ ){
            if (heap[i].name == node_name){              
                return true;}
        }
        return false;
    }

    inline void printHeap() {
        for (int i= 0; i <heap.size(); i++) {
            cout <<  "Name: " <<heap[i].name << " priority: " << heap[i].priority << endl;}
    }    
};

//Class in charge of storing the shortest path of a graph
class ShortestPath {
    vector<int> path;
    float path_size = 0.0;

public:
    //Creates the shortest path given a graph, a previous vector from Dijktra's algorithm and the source and goal positions
    ShortestPath(Graph G, vector<int> previous, int start, int goal){
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

    inline vector<int> getPath(){return path;}
    inline float getSize(){return path_size;}

    //Overriding << operator for the ShortestPath
    friend ostream& operator<<(ostream &os, ShortestPath & shortest_path){
        vector<int> shp = shortest_path.getPath();
        cout<<"Shortest path between "<<shp[0] <<" and "<< shp[shp.size()-1] << " with a length of " << shortest_path.getSize()<<endl;

        for (const auto & element : shp){
            cout<< setprecision(4) << setfill(' ') << setw(6) << element<< ", ";
            }
        cout<< endl;
        return os;
    }
    };

class Triple{
    public:
        int u; 
        int v;
        float weight;
        Triple( int a=0, int b=0, float w=0.0): u(a), v(b), weight(w){}

    inline bool operator<(const Triple triple) { return weight < triple.weight;}

    //Overloading << operator for Triple class
    friend ostream& operator<<(ostream &os, const Triple & triple){
        cout<< "(" << setprecision(4) << setfill(' ') << setw(2) << triple.u << ", ";
        cout<< setprecision(4) << setfill(' ') << setw(2) << triple.v << ", ";
        cout<< setprecision(4) << setfill(' ') << setw(2) << triple.weight << ")";
        return os;
    }
};

/*---------------------------------------------------------------------------------------------------------------------
---------------------------------------------------Algorithms----------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------*/

//Creates and initializes a heap for a graph with a source vertex for Dijkstra's algorithm
MinHeap initialize(int n_vertices, int start){
    MinHeap heap;
    for(int i = 0; i < n_vertices; i++){
        if (i!= start){
            heap.insert(MinHeapNode(i, INFINITY ));}

        else{
            heap.insert(MinHeapNode(i, 0.0 ));}
    }
        return heap;
    }

//Implements Dijkstra's algorithim. It stops when the goal is reached (delete !current_distances.contains(goal) at line 279 to generate each shortest path)
ShortestPath Dijkstra(Graph G, int start, int goal){
    MinHeapNode current_node, neighbor;
    float new_distance;
    vector<int> curr_node_neighbors;

    MinHeap current_distances = initialize(G.vertices(), start);
    vector<int> previous = vector<int>(G.vertices(), -1);
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
    return ShortestPath(G, previous, start, goal);
    }

//Checks if a key is in a given vector
inline bool IsInVec(vector<int> v, int key ){
    if (find(v.begin(), v.end(), key) != v.end()){return true;}
    else {return false;}
}

//Implements Prim's algorithm using IsInVec function
vector<Triple>  Prim(Graph G){
    int u, v; 
    float current_weight, edge_value;
    vector<int> MSTNodes = {0};
    vector<Triple> Prim_list;

    for (int i = 1; i < G.vertices(); i++ ){
        current_weight = INFINITY;
        for (const auto& mst_node : MSTNodes ){
            for(const auto& neighbor: G.neighbors(mst_node)){
                if ( IsInVec(MSTNodes, neighbor)){continue;}
                edge_value = float( G.getEdgeValue(mst_node, neighbor));
                if ( edge_value < current_weight && edge_value >0 ){
                    u = mst_node;
                    v = neighbor;
                    current_weight = edge_value;
                }
            }
        }
        MSTNodes.push_back(v);
        Prim_list.push_back(Triple { u,v,current_weight });
    }
    
    float sum = 0.0;
    for(auto & triple : Prim_list){sum += triple.weight;}
    if (sum == INFINITY){printf("Graph is disconnected no MST found. \n");}

    return Prim_list;
}

//Checks if a key is in a given set
bool IsInSet( set<int> x, int key ){
    for (const auto & el : x){
        if (el == key){return true;}
    }
    return false;
} 

//From a set of sets returns the set that key belongs to
set<int> FindSet( set<set<int>> X, int key ){
    for (const auto & el : X){
        if (IsInSet(el, key )){return el;}
    }
    printf("Error");
    return set<int> {};
}

//Implements Kruskal's algorithm using FindSet and IsInSet functions
vector<Triple>  Kruskal(Graph G){
    int n_nodes= G.vertices();
    float weight;
    set<set<int>> set_of_sets;
    set<int> union_set, set_u, set_v;
    vector<Triple> edge_list, Kruskal_list;

    for (int i = 0; i < n_nodes; i++){
        for (int j = 0; j < n_nodes; j++){
            weight = G.getEdgeValue(i,j);
            if (weight > 0.0){edge_list.push_back(Triple {i, j, weight});}
        }
    }
    sort(edge_list.begin(), edge_list.end());

    for (int node = 0; node < n_nodes; node++){
        set_of_sets.insert(set<int> {node});
    }
    for (const auto& edge : edge_list){
        set_u = FindSet(set_of_sets, edge.u);
        set_v = FindSet(set_of_sets, edge.v);
        if ( set_u != set_v ){
            union_set = {};
            set_union(set_u.begin(), set_u.end(), set_v.begin(), set_v.end(), inserter(union_set, union_set.begin()));
            set_of_sets.erase(set_u); set_of_sets.erase(set_v);
            set_of_sets.insert(union_set);
            Kruskal_list.push_back(edge);
        }  
    }
    if (set_of_sets.size()>1){printf("Graph is disconnected no MST found. \n");}
    return Kruskal_list;   
}

/*---------------------------------------------------------------------------------------------------------------------
--------------------------------------------------Helper functions-----------------------------------------------------
---------------------------------------------------------------------------------------------------------------------*/

/*Reads a graph from a file the file that has the format

number of nodes
node1 node2 weight  //first edge
node1 node2 weight  //second edge
..... ..... ......

*/
Graph readGraphFromFile(char filename[]){
    FILE *file = fopen(filename, "r");
    int i,j,k=0,n_nodes = 0;
    float weight = 0.0;
    
    assert(file != NULL);
    // Allocate matrix
    fscanf(file, "%d", &n_nodes);
    vector<vector<float>> adjacency_matrix(n_nodes, vector<float>(n_nodes,0));
    
    while (fscanf(file, "%d %d %f", &i, &j, &weight)==3){
        if ( i != j ){adjacency_matrix[i][j] = weight;}
    }
    return Graph(adjacency_matrix);
}

/*Function to test implementation, contains solved examples with customizable sources and goals
it has several 3 examples 1-3*/
void test(int example, int source = 0, int goal = 4){
    vector<vector<float>> matrix;
    string web_source;

    switch (example){
    case 1: // https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif
        web_source = "https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#/media/File:Dijkstra_Animation.gif";
        matrix = {
            { 0.0, 7.0, 9.0, 0.0, 0.0, 14.0},
            {7.0, 0.0, 10.0, 15.0, 0.0, 0.0},
            {9.0, 10.0, 0.0, 11.0, 0.0, 2.0},
            {0.0, 15.0, 11.0, 0.0, 6.0, 0.0},
            {0.0, 0.0, 0.0, 6.0, 0.0, 9.0},
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

    Graph G = Graph(matrix);
    ShortestPath solution = Dijkstra(G, source, goal);
    cout<<"Solution example from: "<< web_source <<endl<< solution;

    float sum = 0.0;

    vector<Triple> MSTPrim = Prim(G);
    for(auto & triple : MSTPrim){
        sum += triple.weight;}
    cout<< "MST Prim's algorithm weight "<< sum<<endl;
    cout<<"Prim's algorithm: "<< endl<<  MSTPrim;

    sum = 0.0;
    vector<Triple> MSTKruskal = Kruskal(G);
    for(auto & triple : MSTKruskal){
        sum += triple.weight;}
    cout<< "MST Kruskal's algorithm weight "<< sum<<endl;
    cout<<"Kruskal's algorithm: "<< endl<<  MSTKruskal;
    


}

/* Computes the average shortest distance between node 0 and its neighbors in a randomly
generated graph with a given number of nodes and a vector of densities */
void averageShortestDistance(int n_nodes = 50, vector<float> densities = {0.2, 0.4} ){

    vector<vector<float>> matrix;
    
    float avg_distance;
    Graph G = geneateRandomMatrix(n_nodes, 0.5, 10.0);

    for (float density: densities){
        avg_distance = 0.0;
        matrix = geneateRandomMatrix(n_nodes, density, 10.0);
        G = Graph(matrix);
        for (int i = 1; i < n_nodes; i++){
            avg_distance += Dijkstra(G, 0, i).getSize();}
        printf("Average distance is %.4f for a node with %d nodes and density of %.2f\n", avg_distance/(n_nodes-1), n_nodes, density);
    }
}

int main(){
    cout<< "----------------------Test-----------------------"<<endl;
    test(2); // Test function see the function's description
    cout<< "----------------------Average shortest distance-----------------------"<<endl;
    averageShortestDistance(50, vector<float> {0.2, 0.4, 0.6, 0.8}); //Shortest distance computation
    
    cout<< "----------------------Graph from a file-----------------------"<<endl;
    // Read graph from a file
    char file_name[] = "graph_file_sample.txt";
    Graph G = readGraphFromFile(file_name);

    // Prim's algorithm
    cout<< "----------------------Prim's algorithm-----------------------"<<endl;
    float sum = 0.0;
    vector<Triple> MSTPrim = Prim(G);
    for(auto & triple : MSTPrim){
        sum += triple.weight;}
    cout<< "MST Prim's algorithm weight sum: "<< sum<<endl;
    cout<<"Prim's algorithm edge list: "<< endl<<  MSTPrim;

    // Kruskal's algorithm
    cout<< "----------------------Kruskal's algorithm-----------------------"<<endl;
    sum = 0.0;
    vector<Triple> MSTKruskal = Kruskal(G);
    for(auto & triple : MSTKruskal){
        sum += triple.weight;}
    cout<< "MST Kruskal's algorithm weight sum: "<< sum<<endl;
    cout<<"Kruskal's algorithm edge list: "<< endl<<  MSTKruskal;
    
    return 0;
}