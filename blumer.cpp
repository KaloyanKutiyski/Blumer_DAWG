#include<unordered_map>
#include<string>
#include<iostream>
#include<fstream>
#include<queue>


struct Vertex {
    Vertex* suffixPointer;
    std::unordered_map<char, Vertex*> edges;

    Vertex(): suffixPointer{nullptr} {
    }
};

struct Dawg {
    Vertex* source;
    Vertex* sink;
    
    Vertex* parent;
    std::queue<Vertex*> children;
    std::queue<Vertex*> oldsuffix;

    Vertex* currentState;

    int splits;

    int vertices;
    int edges;

    Dawg(): source{new Vertex()},
        splits{0},
        vertices{1},
        edges{0} {
        sink = source;
        parent = source;
    }

    void buildDawg(const std::string& str) {
        std::ifstream in(str);
        char c;
        while (in.read(&c, 1)) {
            update(c);
        }
    }

    void update(const char& character) {
        const char characterPrimary = character - 'a' + 'A';

        //1
        Vertex* newSink = new Vertex();
        sink->edges.emplace(characterPrimary, newSink);
        vertices++;
        edges++;

        //2        
        currentState = sink;
        Vertex* suffixState = nullptr;
        
        //3
        while(currentState != source && !suffixState) {
            //a
            currentState = currentState->suffixPointer;
            //b
                //1
            if (currentState->edges.find(characterPrimary) == currentState->edges.end()
                && currentState->edges.find(character) == currentState->edges.end()) {
                    //a
                for (int i = 1; i <= splits; i++) {
                    Vertex* topChild = children.front();
                    children.pop();
                    Vertex* topSuffix = oldsuffix.front();
                    oldsuffix.pop();
                    
                    split(topChild, topSuffix);
                }
                    //b
                if (splits) {
                    currentState = parent;
                    splits = 0;
                }
                    //c
                currentState->edges.emplace(character, newSink);
                edges++;
            }
                //2
            else if (!splits && currentState->edges.find(characterPrimary) != currentState->edges.end()) {
                suffixState = currentState->edges.at(characterPrimary);
            }
                //3
            else {
                    //a
                if (currentState->edges.find(characterPrimary) != currentState->edges.end()) {
                    suffixState = currentState->edges.at(characterPrimary);
                } else {
                    suffixState = currentState->edges.at(character);
                }
                    //b
                    //c
                if (++splits == 1) {
                    parent = currentState;
                }
                    //d
                children.push(suffixState);
                oldsuffix.push(newSink);
            }
        }
        //4
        if (!suffixState) {
            suffixState = source;
        }
        //5
        newSink->suffixPointer = suffixState;
        sink = newSink;
    }

    void split(Vertex* childState, Vertex* oldSuffixState) {
        //1
        Vertex* newChildState = new Vertex();
        vertices++;
        //2
        char transition;
        for (auto elem : parent->edges) {
            if(elem.second == childState) {
                transition = elem.first;
                break;
            }
        }
        parent->edges.erase(transition);
        parent->edges.emplace(transition - 'a' + 'A', newChildState);
        //3
        for (auto elem : childState->edges) {
            char transition = elem.first;
            if (transition >= 'A' && transition <= 'Z' || transition == 64) {
                transition += (-'A' + 'a');
            } 
            newChildState->edges.emplace(transition, elem.second);
            edges++;
        }
        //4
        newChildState->suffixPointer = childState->suffixPointer;
        //5
        childState->suffixPointer = newChildState;
        //6
        oldSuffixState->suffixPointer = newChildState;
        //7
        currentState = parent;
        //8
        while (currentState != source) {
            //a
            currentState = currentState->suffixPointer;
            //b
            char transition = 0;
            for (auto elem : currentState->edges) {
                if (elem.second == childState) {
                    if (elem.first >= 'a' && elem.first <= 'z' || elem.first == 96) {
                        transition = elem.first;
                    } 
                    break;
                }
            }
            if (transition) {
                currentState->edges[transition] = newChildState;
            }
            //c
            else {
                break;
            }
        }
        //9
        parent = newChildState;
    }

    void destroy(Vertex* v) {
        for (auto elem : v->edges) {
            if (elem.first >= 'A' && elem.first <= 'Z' || elem.first == 64) {
                destroy(elem.second);
            }
        }
        delete v;
    }

    ~Dawg() {
        destroy(source);
        source = nullptr;
    }
};

int main(int argc, char *argv[]) {
    Dawg dawg;
    dawg.buildDawg(argv[1]);
    std::cout << dawg.vertices << '\n' << dawg.edges << '\n';
    int edges = dawg.edges;
    dawg.update(96);
    std::cout << dawg.edges - edges << '\n';

    return 0;
}