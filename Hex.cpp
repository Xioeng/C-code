#include <vector> 
#include <iostream>
#include <random>
#include <assert.h>
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;

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
string operator*(const int repetitionCount, const string& str) {
    string result;
    for (int i = 0; i < repetitionCount; ++i) {result += str;}
    return result;
}

//Implements the graph class declared using and adjacency matrix and its basic operations. Also it has a vector storing the node values
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
    
    inline void addSetEdgeValue(int u, int v, float value = 1.0){
        assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
        adjacency_matrix[u][v] = value;}
    
    inline void addSetUndirectedEdgeValue(int u, int v, float value = 1.0){
        addSetEdgeValue(u, v, value );
        addSetEdgeValue(v, u, value );}
    
    inline float getEdgeValue(int u, int v){
        assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
        return adjacency_matrix[u][v];}
    
    inline void addDeleteEdge(int u, int v){
        addSetEdgeValue(u, v, 0.0);}
    
    //For Dijkstra's algorithm
    void initializeNodes(int start){
        node_values = vector<float>( vertices(), INFINITY );
        node_values[start] = 0.0;}
};

int nodeType(int node, int size){
    int row = node/size, column = node % size;

    if (row == 0){
        if (column == 0){return 0;}
        else if (column == size - 1){return 2;}
        else{return 1;}
        }

    if ( row == size - 1){
        if (column == 0){return 6;}
        else if (column == size - 1){return 4;}
        else {return 5;}
        }
    
    if (column == 0){return 7;}
    else if(column ==  size - 1 ){return 3;}
    else {return 8;}
    }

vector<string> node_assignations ={"top-right corner","top boundary", "top-left corner",
                "left boundary", "bottom-left corner", "bottom boundary",
                "bottom-right corner", "right boundary", "middle"};
/*
0 - top-right corner
1 - top boundary    
2 - top-left corner
3 - left boundary
4 - bottom-left corner
5 - bottom boundary
6 - bottom-right corner
7 - right boundary
8 - middle
*/  

Graph initializeHexBoard(int size){
    int board_nodes = size*size, row, column;
    Graph hex_board(vector<vector<float>> (board_nodes, vector<float>(board_nodes, 0.0)));
    for (int node = 0; node < board_nodes; node++ ){
        row = node/size, column = node % size;
        switch (nodeType(node, size)){
        case 0: //top-right corner
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node + size);
            break;
        case 1: //top boundary
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node + size - 1);
            hex_board.addSetEdgeValue(node, node + size);
            break;
        case 2://top-left corner
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node + size);
            hex_board.addSetEdgeValue(node, node + size - 1);
            break;
        case 3: //left boundary
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node + size - 1);
            hex_board.addSetEdgeValue(node, node + size);
            hex_board.addSetEdgeValue(node, node - size);
            break;
        case 4://bottom-left corner
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node - size);
            break;
        case 5: //bottom boundary
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node - size + 1);
            hex_board.addSetEdgeValue(node, node - size);
            break;
        case 6://bottom-right corner
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node - size);
            hex_board.addSetEdgeValue(node, node - size + 1);
            break;
        case 7: //right boundary
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node - size + 1);
            hex_board.addSetEdgeValue(node, node + size);
            hex_board.addSetEdgeValue(node, node - size);
            break;
        case 8: //middle
            hex_board.addSetEdgeValue(node, node - 1);
            hex_board.addSetEdgeValue(node, node + 1);
            hex_board.addSetEdgeValue(node, node + size);
            hex_board.addSetEdgeValue(node, node - size);
            hex_board.addSetEdgeValue(node, node - size + 1);
            hex_board.addSetEdgeValue(node, node + size - 1);
            break;  
        default:
            break;
        }
    }
    return hex_board;
}

void printHexBoard(Graph hex_board){
    int size = sqrt(hex_board.vertices()), tab_num = 0;
    cout<< size<<endl;
    assert(size*size == hex_board.vertices());
    string player_1_symbol = "X", player_2_symbol = "O", not_played = "*";
    string base_edge_line ="\\ / " , node_link = " - ", node_line = "";
    string edge_line =(string)" " + (size - 1) * base_edge_line + "\\";

    for (int node = 0; node < hex_board.vertices(); node++ ){
        // node_line = tab_num*(string)" ";
        if (hex_board.getNodeValue(node) == 1.0){node_line += player_1_symbol;}
        else if (hex_board.getNodeValue(node) == 2.0){node_line += player_2_symbol;}
        else {node_line += not_played;}

        if ( (node % size) == size - 1){
            //Print lines
            cout<<node_line<<endl;
            if (node/size == size-1){break;}
            cout<<edge_line<<endl;
            //Re-initialization
            edge_line = (2)*(string)" " + edge_line;
            tab_num++;
            node_line = 2*tab_num*(string)" ";
            }
        else{node_line +=node_link;}
    
        
    }
}




int main(){
    int size = 7;
    
    Graph hex_board =  initializeHexBoard(size);
    // for (int i = 0; i< size * size; i++ ){ 
    //     cout<<"node "<<i <<" type: "<< node_assignations[nodeType(i, size)]<< endl;}
    int node = 12;
    vector<int> v = hex_board.neighbors(node);
    cout<<v<<endl<< 5*node_assignations[nodeType(node, size)]<<nodeType(node, size)<< endl<< endl;
    hex_board.setNodeValue(1, 1.0);
    hex_board.setNodeValue(14, 2.0);
    printHexBoard(hex_board);
    return 0;
}