#include <vector> 
#include <iostream>
#include <random>
#include <assert.h>
#include <string>
#include <iomanip>
#include <cmath>
#include <tuple>
#include <algorithm>
using namespace std;

//Symbols for players
// string red_player_symbol = "\u2716", blue_player_symbol = "\u25CF", not_played_symbol = "\u2B21"; //(1) RED == cross, BLUE == circle, NOT_PLAYED == Hexagon
string red_player_symbol = "x", blue_player_symbol = "o", not_played_symbol = "*";

//Template for printing vectors
template<typename T>
ostream& operator<<(ostream &os, vector<T> &vec){
    int k = 0;
    for (const auto & element : vec){
        cout<< setprecision(4) << setfill(' ') << setw(6) << element<< ", ";
        if (++k % 7 == 0){ cout<<endl;}
        }
    cout<< endl;
    return os;
}

template<typename T>
int argmax(const vector<T> & vec) {
    if (vec.empty()) {throw invalid_argument("Vector is empty"); }
    return static_cast<int> (distance(vec.begin(), max_element(vec.begin(), vec.end())));
}
//Overloading * operator for strings
string operator*(const int repetitionCount, const string &str) {
    string result;
    for (int i = 0; i < repetitionCount; ++i) {result += str;}
    return result;
}

//Defines Player enumeration to tell who has played which movement
enum Player{NO_WINNER= -1, NOT_PLAYED = 0, RED = 1, BLUE = 2};

ostream& operator<<(ostream &os, const Player &player){
    switch (player){
        case RED:
            cout<<"Red player";
            break;
        case BLUE:
            cout<<"Blue player";
            break;
        case NO_WINNER:
            cout<<"No one has won yet";
            break;
        default:
            break;
    }
    return os;
}

class HexBoard {
    private:
        int size;
        vector<vector<int>> adjacency_matrix;
        vector<Player> node_values;
        vector<int> nodes_to_play;
        vector<bool> DFS_labels;
        
    public:
        HexBoard(int size): 
            size(size), 
            adjacency_matrix(vector<vector<int>>(size*size, vector<int>(size*size, 0))),
            node_values(vector<Player>(adjacency_matrix.size(), NOT_PLAYED)), 
            nodes_to_play(vector<int>(adjacency_matrix.size(), 0)),
            DFS_labels(vector<bool>( size*size , false)) {
                initializeHexBoard();
                for(int i = 1; i < size*size; i++){nodes_to_play[i] = i;}
            }

        inline int vertices() {
        return adjacency_matrix.size();}

        vector<vector<int>> adjacencyMatrix() { //For debugging purposes
            return adjacency_matrix;}
        
        inline bool adjacent(int u, int v){
            return adjacency_matrix[u][v] > 0;}
        
        inline void setNodeValue(int u, Player value){
            assert(u < vertices() && u >= 0);
            node_values[u] = value;}

        inline Player getNodeValue(int u){
            assert(u < vertices() && u >= 0);
            return node_values[u];}
        
        inline vector<int> getNodesToPlay(){
            return nodes_to_play;}
        
        inline void addSetEdgeValue(int u, int v, int value = 1 ){
            assert(u < vertices() && v < vertices() && u>= 0 && v >= 0);
            adjacency_matrix[u][v] = value;}

        inline void resetDFSLabels(){
            DFS_labels = vector<bool>( size*size , false);}

        inline int deleteNodeToPlay(int node){
            
            auto index = find(nodes_to_play.begin(), nodes_to_play.end(), node);
            if (index != nodes_to_play.end()){
                nodes_to_play.erase(index);
                return 0;
            }
            cout<< "Unsuccesful deletion\n";
            return 1;
        }

        /*Classifies each node (hexagon) in a Hex board according to its location
        The list is for printing purposes:
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
            for (int node = 0; node < vertices(); node++ ){
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

        void printHexBoard(){
            int tab_num = 0;
            printf("Printing hex board of size %d x %d:\n\n", size, size);
            /*Board symbols, prints two types of line:
            node_line: line with the nodes using characters in (1) preamble
            edge_line: line with the links between nodes using characters in (2)
            */

            string base_edge_line ="\\ / " , node_link = " — ", node_line = ""; //(2)
            string edge_line =(string)" " + (size - 1) * base_edge_line + "\\";
            node_link = " - ";
            for (int node = 0; node < vertices(); node++ ){
                if (getNodeValue(node) == RED){node_line += red_player_symbol;}//Player 1 played on that node
                else if (getNodeValue(node) == BLUE){node_line += blue_player_symbol;}//Player 2 played on that node
                else {node_line += not_played_symbol;}//No one has played on that node

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

        vector<int> neighborsColor(int u, Player color){
            assert(u < vertices() && (color == RED || color == BLUE));
            vector<int> neighbors_vector;
            for (int j = 0; j < vertices(); j++){
                if (adjacent(u,j) && getNodeValue(j) == color){
                    neighbors_vector.push_back(j);
                }
            }
            return neighbors_vector;
        }

        int executeMovement(Player player, int node, bool print_board = true){
                assert(player == RED || player == BLUE);
                if (getNodeValue(node) == NOT_PLAYED){
                    if (player == RED){setNodeValue(node, RED);}
                    if (player == BLUE){setNodeValue(node, BLUE);}
                    deleteNodeToPlay(node);
                    if (print_board){
                        int row = node/size, column = node % size;
                        cout<< player << " did a movement at position ("<< row<<", "<<column <<")\n";
                        printHexBoard();
                    }
                    return 0;
                }
                cout<<"Invalid movement, choose another one.\n";
                return 1;
        }

        /*Plays a movement and returns an integer depending whether the movement is legal or not
        Assigns an int to the node_values RED for player 1 and BLUE for player 2, NOT_PLAYED is the default unplayed value*/
        int playMovement(Player player, tuple<int, int> & movement, bool print_board = true){
                int row = get<0>(movement), column = get<1>(movement);
                assert(row >= 0 && row < size);
                assert(column >= 0 && column < size);
                int node = (size * row) + column;
                return executeMovement(player, node, print_board);
            }
        
        int playMovement(Player player, int movement, bool print_board = true){
            assert(movement < size*size);
            return executeMovement(player, movement, print_board);
        }

        void DFSColor(int source, Player color){
            int row, column;
            assert(source >=0 && source < vertices());
            DFS_labels[source] = true;
            vector<int> neighbors = neighborsColor(source, color);
            // cout <<neighbors<<endl;
            for(const auto & neighbor : neighbors){if (!DFS_labels[neighbor]){DFSColor(neighbor, color);} //Do DFS recursively
            }
        }

        //Tells the winner if any or no winner 
        Player winnerChecker(){           
            for(int source = 0; source < size; source ++){
                if (getNodeValue(source) != RED){continue;}
                resetDFSLabels();
                DFSColor(source, RED);
                // cout<<DFS_labels;
                for (int k = 0; k < size; k++){
                    if (DFS_labels[ size*(size-1) + k ]){
                        return RED;
                    }
                }
            }
            for(int k = 0, source = 0; k < size; k ++){
                source = k*size;
                if (getNodeValue(source) != BLUE){continue;}
                resetDFSLabels();
                DFSColor(source, BLUE);
                // cout<<DFS_labels;
                for (int k = 1; k <= size; k++){
                    if (DFS_labels[ k*size - 1]){
                        return BLUE;
                    }
                }
            }
            return NO_WINNER;
        }
};

class AIPlayer{
    public:
        Player color;
        int n_attempts_per_move;

        AIPlayer(Player color, int n_attempts_per_move = 100) : 
        color(color),
        n_attempts_per_move(n_attempts_per_move)
        {}

        void fillBoard(HexBoard & board){
            vector<int> nodes_to_play = board.getNodesToPlay();
            bool your_turn = true;
            Player other_player;
            static random_device rd;
            static mt19937 gen(rd());

            if (color == RED){other_player = BLUE;}
            else {other_player = RED;}

            // Shuffle the vector
            shuffle(nodes_to_play.begin(), nodes_to_play.end(), gen);
            for ( auto movement : nodes_to_play){
                
                if (your_turn){ 
                    cout<<movement<<" player "<< color<<endl;
                    board.playMovement(color, movement, false);
                    your_turn = false;}
                else{
                    cout<<movement<<" player "<< other_player<<endl;
                    board.playMovement(other_player, movement, false);
                    your_turn = true;
                }
            }
            assert(board.getNodesToPlay().size() == 0);
        }

        int nextMovement(HexBoard current_hex_board){
            HexBoard test_board = current_hex_board;
            Player winner;
            vector<int> nodes_to_play = current_hex_board.getNodesToPlay();
            vector<int> counts(nodes_to_play.size(), 0);
            for (int i = 0; i < counts.size() ; i++ ){
                test_board = current_hex_board;
                for(int attempt = 0; attempt < n_attempts_per_move; attempt++){
                    fillBoard(test_board);
                    winner = test_board.winnerChecker();
                    if (winner == color){counts[i] += 1;}
                }
            }
        return nodes_to_play[argmax(counts)];
        }

};



int main(){
    int size = 5 ;
    HexBoard hex_board =  HexBoard(size);
    hex_board.initializeHexBoard();
    cout<< "Example Hex board, each non hexagonal character represents each player's move.\n Red player symbol: " + red_player_symbol 
    + " Blue player symbol: " + blue_player_symbol + " Non played symbol: " + not_played_symbol + "\nOriginal board:\n\n";
    hex_board.printHexBoard();
    cout<<"Example movements:\n";
    auto movement = make_tuple(1,4); //Pair row column
    hex_board.playMovement(RED, movement, false);
    movement = make_tuple(3,2);
    hex_board.playMovement(BLUE, movement, false);
    movement = make_tuple(3,4);
    hex_board.playMovement(RED, movement, false);
    

    movement = make_tuple(3,1);
    hex_board.playMovement(BLUE, movement, false);
    movement = make_tuple(4,3);
    hex_board.playMovement(RED, movement, false);
    movement = make_tuple(3,3);
    hex_board.playMovement(BLUE, movement, false);
    movement = make_tuple(2,3);
    hex_board.playMovement(RED, movement, false);
    movement = make_tuple(3,0);
    hex_board.playMovement(BLUE, 3*5+0, true);
    // movement = make_tuple(0,4);
    // hex_board.playMovement(RED, movement, false);
    
    
    
    
    
    AIPlayer red_machine = AIPlayer(RED, 100);
    red_machine.fillBoard(hex_board);
    vector<int> neg = hex_board.getNodesToPlay();

    cout<<"neigh"<< neg<<endl;
    hex_board.printHexBoard();
    cout<<hex_board.winnerChecker();

    return 0;
}