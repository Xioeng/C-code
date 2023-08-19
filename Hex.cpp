#include <vector> 
#include <iostream>
#include <random>
#include <assert.h>
#include <string>
#include <iomanip>
#include <cmath>
#include <tuple>
using namespace std;

//Symbols for players
string player_1_symbol = "\u2716", player_2_symbol = "\u25CF", not_played = "\u2B21"; //(1)

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
//Overloading * operator for strings
string operator*(const int repetitionCount, const string& str) {
    string result;
    for (int i = 0; i < repetitionCount; ++i) {result += str;}
    return result;
}

//Implements the graph class declared using and adjacency matrix and its basic operations. Also it has a vector storing the node values
class Graph{
protected:
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

//HexBoard class, a subclass of Graph
class HexBoard : public Graph{
    private:
        int size;
        vector<bool> DFS_labels;
    public:
        HexBoard(int size): 
            Graph(vector<vector<float>> (size * size, vector<float>(size * size, 0.0))), size(size), DFS_labels(vector<bool>( size * size , false)) {}

        inline void resetDFSLabels(){
            DFS_labels = vector<bool>( size * size , false);}

        void printHexBoard(){
            int tab_num = 0;
            printf("Printing hex board of size %d x %d:\n\n", size, size);
            /*Board symbols, prints two types of line:
            node_line: line with the nodes using characters in (1) preamble
            edge_line: line with the links between nodes using characters in (2)
            */
            
            string base_edge_line ="\\ / " , node_link = " — ", node_line = ""; //(2)
            string edge_line =(string)" " + (size - 1) * base_edge_line + "\\";

            for (int node = 0; node < vertices(); node++ ){
                if (getNodeValue(node) == 1.0){node_line += player_1_symbol;}//Player 1 played on that node
                else if (getNodeValue(node) == 2.0){node_line += player_2_symbol;}//Player 2 played on that node
                else {node_line += not_played;}//No one has played on that node

                if ( (node % size) == size - 1){
                    //Print lines
                    cout<<node_line<<endl;
                    if (node/size == size-1){break;} //It has printed the last line, no edge line is necessary
                    cout<<edge_line<<endl;
                    //Lines re-initialization (blank node line, double tabbed edge_line)
                    edge_line = 2*(string)" " + edge_line;
                    node_line = 2*(++tab_num)*(string)" ";
                    }
                else{node_line +=node_link;}
            }
            cout<<endl;
        }

        vector<int> neighborsColor(int u, float color){
            assert(u < vertices() && (color == 1.0 || color == 2.0));
            vector<int> neighbors_vector;
            for (int j = 0; j < vertices(); j++){
                // cout<<j<< "  "<<getNodeValue(j)<<"  "<<(getNodeValue(j) == color)<<"  "<<adjacency_matrix[u][j]<<endl;
                if (adjacency_matrix[u][j]>0.0 && getNodeValue(j) == color){
                    neighbors_vector.push_back(j);
                }
            }
            return neighbors_vector;
        }

        /*Classifies each node (hexagon) in a Hex board according to its location
        The list is for printing purposes:
        0 - top-right corner = node_assignations[0] char array defined just before the main function
        1 - top boundary    
        2 - top-left corner
        3 - left boundary
        4 - bottom-left corner
        5 - bottom boundary
        6 - bottom-right corner
        7 - right boundary
        8 - middle = node_assignations[8]
        */  
        int nodeType(int node){
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

        void initializeHexBoard(){
            int board_nodes = vertices();
            Graph hex_board(vector<vector<float>> (board_nodes, vector<float>(board_nodes, 0.0)));
            for (int node = 0; node < board_nodes; node++ ){
                switch (nodeType(node)){
                case 0: //top-right corner
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size);
                    break;
                case 1: //top boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size - 1);
                    addSetEdgeValue(node, node + size);
                    break;
                case 2://top-left corner
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node + size - 1);
                    break;
                case 3: //left boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + size - 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    break;
                case 4://bottom-left corner
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node - size);
                    break;
                case 5: //bottom boundary
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node - size);
                    break;
                case 6://bottom-right corner
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size);
                    addSetEdgeValue(node, node - size + 1);
                    break;
                case 7: //right boundary
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    break;
                case 8: //middle
                    addSetEdgeValue(node, node - 1);
                    addSetEdgeValue(node, node + 1);
                    addSetEdgeValue(node, node + size);
                    addSetEdgeValue(node, node - size);
                    addSetEdgeValue(node, node - size + 1);
                    addSetEdgeValue(node, node + size - 1);
                    break;  
                default:
                    break;
                }
            }
        }

        /*Plays a movement and returns an integer depending whether the movement is legal or not
        Assigns a float to the adjacency matrix 1.0 for player 1 and 2.0 for player 2, 0.0 is the default unplayed value*/
        int playMovement(string player, tuple<int, int> & movement, bool print_board = true){
                int row = get<0>(movement), column = get<1>(movement);
                assert(player == "player 1" ||player == "player 2");
                assert(row >= 0 && row < size);
                assert(column >= 0 && column < size);
                int node = (size * row) + column;

                if (getNodeValue(node) == 0.0){
                    if (player == "player 1"){setNodeValue(node, 1.0);}
                    if (player == "player 2"){setNodeValue(node, 2.0);}
                    if (print_board){
                        cout<< player + " did a movement at position ("<< row<<", "<<column <<")\n";
                        printHexBoard();
                    }
                    return 0;
                }
                cout<<"Invalid movement, choose another one.\n";
                return 1;
            }
        void DFSColor(int source, float color){
            int row, column;
            assert(source >=0 && source < vertices());
            DFS_labels[source] = true;
            vector<int> neighbors =neighborsColor(source, color);
            // cout <<neighbors<<endl;
            for(const auto & neighbor : neighbors){if (!DFS_labels[neighbor]){DFSColor(neighbor, color);} //Do DFS recursively
            }
        }

        string winnerChecker(){
            string players[2] = {"player 1", "player 2"};
            float colors[2] = {1.0, 2.0};

            
            for(int source = 0; source < size; source ++){
                if (getNodeValue(source) != 1.0){continue;}
                resetDFSLabels();
                DFSColor(source, 1.0);
                // cout<<DFS_labels;
                for (int k = 0; k < size; k++){
                    if (DFS_labels[ size*(size-1) + k ]){
                        return "player 1";
                    }
                }
            }
            for(int k = 0, source = 0; k < size; k ++){
                source = k*size;
                if (getNodeValue(source) != 2.0){continue;}
                resetDFSLabels();
                DFSColor(source, 2.0);
                // cout<<DFS_labels;
                for (int k = 1; k <= size; k++){
                    if (DFS_labels[ k*size - 1]){
                        return "player 2";
                    }
                }
            }
            return "no winner yet";
        }

};

// const vector<string> node_assignations ={"top-right corner","top boundary", "top-left corner",
//                 "left boundary", "bottom-left corner", "bottom boundary",
//                 "bottom-right corner", "right boundary", "middle"};

                // row = neighbor/size, column = neighbor % size;
                // cout<<"col"<<column<<"row"<<row<<endl;
                // if (column == size - 1 && color == 1.0){
                //     resetDFSLabels();
                //     printf("ffff");
                //     return true;
                // } //Blue goes from left to right
                // if (row == size - 1 && color == 2.0){
                //     resetDFSLabels();
                //     return true;
                // } //Red goes from top to bottom

int main(){
    int size = 5 ;
    HexBoard hex_board =  HexBoard(size);
    hex_board.initializeHexBoard();
    cout<< "Example Hex board, each non hexagonal character represents each player's move.\nPlayer 1 symbol: " + player_1_symbol + " Player 2 symbol: " + player_2_symbol + "\nOriginal board:\n";
    hex_board.printHexBoard();
    cout<<"Example movements:\n";
    auto movement = make_tuple(1,4); //Pair row column
    hex_board.playMovement((string)"player 1", movement, false);
    movement = make_tuple(3,2);
    hex_board.playMovement((string)"player 2", movement, false);
    movement = make_tuple(3,1);
    hex_board.playMovement((string)"player 2", movement, false);
    movement = make_tuple(3,3);
    hex_board.playMovement((string)"player 2", movement, false);
    movement = make_tuple(3,0);
    hex_board.playMovement((string)"player 2", movement, false);
    movement = make_tuple(0,4);
    hex_board.playMovement((string)"player 1", movement, false);
    movement = make_tuple(2,3);
    hex_board.playMovement((string)"player 1", movement, false);
    movement = make_tuple(4,3);
    hex_board.playMovement((string)"player 1", movement, false);
    movement = make_tuple(3,4);
    hex_board.playMovement((string)"player 1", movement);

    // cout<<"neigh"<<(bool)hex_board.DFSColor(3*size+1, 1.0)<<endl;
    cout<<(string)hex_board.winnerChecker();
    return 0;
}