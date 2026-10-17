#include<iostream>
#include<string>
#include<vector>
using namespace std;

const bool debug_mode = false;

struct Node{
    long value = 0;
    Node *left = NULL, *right = NULL, *father = NULL;
};

void splay(Node* node, Node*& tree);
void add(long a, Node*& tree);
void del(long a, Node*& tree);
bool find(long a, Node*& tree);
long sum_recursive(long a, long b, Node* tree);
long sum(long a, long b, Node* tree);
void debug(Node* tree, long i);
void print_tree(Node* tree);

int main(){
    string input;
    Node* tree = NULL;
    long n, a, b;
    cin >> n;
    for(long i=0; i<n; i++){
        cin >> input;
        if(input.compare("add") == 0){
            cin >> a;
            add(a, tree);
        }
        else if(input.compare("del") == 0){
            cin >> a;
            del(a, tree);
        }
        else if(input.compare("find") == 0){
            cin >> a;
            cout << (find(a, tree) ? "true" : "false") << endl;
        }
        else if(input.compare("sum") == 0){
            cin >> a >> b;
            cout << sum(a, b, tree) << endl;
        }
        debug(tree, i);
    }
    vector<Node*> pending;
    if (tree != NULL) pending.push_back(tree);
    while (!pending.empty()) {
        Node* current = pending.back();
        pending.pop_back();
        if (current->left != NULL) pending.push_back(current->left);
        if (current->right != NULL) pending.push_back(current->right);
        delete current;
    }
    return 0;
}

void add(long a, Node*& tree){
    if(tree == NULL){
        tree = new Node;
        tree->value = a;
        return;
    }
    Node* temp = tree;
    while(true){
        if(temp->value < a){
            if(temp->right == NULL){
                temp->right = new Node;
                temp->right->value = a;
                temp->right->father = temp;
                debug(tree, -2);
                splay(temp->right, tree);
                return;
            }
            temp = temp->right;
        }
        else if(temp->value > a){
            if(temp->left == NULL){
                temp->left = new Node;
                temp->left->value = a;
                temp->left->father = temp;
                debug(tree, -2);
                splay(temp->left, tree);
                return;
            }
            temp = temp->left;
        }
        else{
            return;
        }
    }
}

void del(long a, Node*& tree){
    if(tree == NULL){
        return;
    }
    Node *temp = tree, *temp_father = NULL;
    while(true){
        if(temp->value < a){
            if(temp->right == NULL){
                return;
            }
            temp_father = temp;
            temp = temp->right;
        }
        else if(temp->value > a){
            if(temp->left == NULL){
                return;
            }
            temp_father = temp;
            temp = temp->left;
        }
        else{
            if(temp->left != NULL){
                if(temp->left->right == NULL){
                    temp->value = temp->left->value;
                    Node* delete_temp = temp->left;
                    temp->left = temp->left->left;
                    if(temp->left != NULL){
                        temp->left->father = temp;
                    }
                    delete delete_temp;
                    debug(tree, -2);
                    splay(temp, tree);
                    return;
                }
                Node* temp2 = temp->left;
                temp_father = temp;
                while(temp2->right != NULL){
                    temp_father = temp2;
                    temp2 = temp2->right;
                }
                temp->value = temp2->value;
                temp_father->right = temp2->left;
                if(temp2->left != NULL){
                    temp2->left->father = temp_father;
                }
                delete temp2;
                debug(tree, -2);
                splay(temp, tree);
                return;
            }
            else if(temp->right != NULL){
                if(temp->right->left == NULL){
                    temp->value = temp->right->value;
                    Node* delete_temp = temp->right;
                    temp->right = temp->right->right;
                    if(temp->right != NULL){
                        temp->right->father = temp;
                    }
                    delete delete_temp;
                    debug(tree, -2);
                    splay(temp, tree);
                    return;
                }
                Node* temp2 = temp->right;
                temp_father = temp;
                while(temp2->left != NULL){
                    temp_father = temp2;
                    temp2 = temp2->left;
                }
                temp->value = temp2->value;
                temp_father->left = temp2->right;
                if(temp2->right != NULL){
                    temp2->right->father = temp_father;
                }
                delete temp2;
                debug(tree, -2);
                splay(temp, tree);
                return;
            }
            else{
                if(temp_father == NULL){
                    delete temp;
                    tree = NULL;
                    return;
                }
                if(temp_father->right == temp){
                    temp_father->right = NULL;
                }
                else{
                    temp_father->left = NULL;
                }
                Node* parent = temp->father;
                delete temp;
                debug(tree, -2);
                splay(parent, tree);
                return;
            }
        }
    }
}

bool find(long a, Node*& tree){
    if(tree == NULL){
        return false;
    }
    Node* temp = tree;
    while(true){
        if(temp->value < a){
            if(temp->right == NULL){
                return false;
            }
            temp = temp->right;
        }
        else if(temp->value > a){
            if(temp->left == NULL){
                return false;
            }
            temp = temp->left;
        }
        else{
            debug(tree, -2);
            splay(temp, tree);
            return true;
        }
    }
}

long sum(long a, long b, Node* tree){
    if(tree == NULL){
        return 0;
    }
    return sum_recursive(a, b, tree);
}

long sum_recursive(long a, long b, Node* tree){
    long sum = 0;
    if(tree->value >= a && tree->value <= b){
        sum += tree->value;
        if(tree->right != NULL){
            sum += sum_recursive(a, b, tree->right);
        }
        if(tree->left != NULL){
            sum += sum_recursive(a, b, tree->left);
        }
    }
    else if(tree->value > b){
        if(tree->left != NULL){
            sum += sum_recursive(a, b, tree->left);
        }
    }
    else if(tree->value < a){
        if(tree->right != NULL){
            sum += sum_recursive(a, b, tree->right);
        }
    }
    return sum;
}

void splay(Node* node, Node*& tree){
    if(node == tree || node == NULL){
        return;
    }
    if(node->father == NULL){
        tree = node;
        return;
    }
    if(node->father == tree || node->father->father == NULL){
        tree = node->father;
        if(tree->left == node){
            Node* temp = node->right;
            node->right = tree;
            tree->father = node;
            node->father = NULL;
            tree = node;
            node->right->father = node;
            node->right->left = temp;
            if(temp != NULL){
                temp->father = node->right;
            }
            return;
        }
        if(tree->right == node){
            Node* temp = node->left;
            node->left = tree;
            tree->father = node;
            node->father = NULL;
            tree = node;
            node->left->father = node;
            node->left->right = temp;
            if(temp != NULL){
                temp->father = node->left;
            }
            return;
        }
        if(debug_mode){
            cout << node->value << endl << tree->value << endl;
            cout << "UNREACHABLE1" << endl;
        }
    }
    else{
        if(node == node->father->left && node->father == node->father->father->left){
            Node *x = node, *p = node->father, *g = node->father->father;
            if(g->father != NULL){
                if(g->father->right == g){
                    g->father->right = x;
                }
                else{
                    g->father->left = x;
                }
            }
            else{
                tree = x;
            }
            x->father = g->father;
            p->father = x;
            g->father = p;
            p->left = x->right;
            if(x->right != NULL){
                x->right->father = p;
            }
            x->right = p;
            g->left = p->right;
            if(p->right != NULL){
                p->right->father = g;
            }
            p->right = g;
            debug(tree, -103);
            splay(x, tree);
            return;
        }
        if(node == node->father->right && node->father == node->father->father->right){
            Node *x = node, *p = node->father, *g = node->father->father;
            if(g->father != NULL){
                if(g->father->right == g){
                    g->father->right = x;
                }
                else{
                    g->father->left = x;
                }
            }
            else{
                tree = x;
            }
            x->father = g->father;
            p->father = x;
            g->father = p;
            p->right = x->left;
            if(x->left != NULL){
                x->left->father = p;
            }
            x->left = p;
            g->right = p->left;
            if(p->left != NULL){
                p->left->father = g;
            }
            p->left = g;
            debug(tree, -103);
            splay(x, tree);
            return;
        }
        if(node == node->father->right && node->father == node->father->father->left){
            Node *x = node, *p = node->father, *g = node->father->father;
            if(g->father != NULL){
                if(g->father->right == g){
                    g->father->right = x;
                }
                else{
                    g->father->left = x;
                }
            }
            else{
                tree = x;
            }
            x->father = g->father;
            p->father = x;
            g->father = x;
            p->right = x->left;
            if(x->left != NULL){
                x->left->father = p;
            }
            g->left = x->right;
            if(x->right != NULL){
                x->right->father = g;
            }
            x->left = p;
            x->right = g;
            debug(tree, -103);
            splay(x, tree);
            return;
        }
        if(node == node->father->left && node->father == node->father->father->right){
            Node *x = node, *p = node->father, *g = node->father->father;
            if(g->father != NULL){
                if(g->father->right == g){
                    g->father->right = x;
                }
                else{
                    g->father->left = x;
                }
            }
            else{
                tree = x;
            }
            x->father = g->father;
            p->father = x;
            g->father = x;
            p->left = x->right;
            if(x->right != NULL){
                x->right->father = p;
            }
            g->right = x->left;
            if(x->left != NULL){
                x->left->father = g;
            }
            x->right = p;
            x->left = g;
            debug(tree, -103);
            splay(x, tree);
            return;
        }
        if(debug_mode){
            cout << "UNREACHABLE2" << endl;
        }
    }
    if(debug_mode){
        cout << "UNREACHABLE3" << endl;
    }
    return;
}

void print_tree(Node* tree){
    if(tree == NULL){
        cout << "null" << endl;
        return;
    }
    vector<Node*> v;
    Node* temp = new Node;
    v.push_back(tree);
    v.push_back(NULL);
    while(!v.empty()){
        if(v[0] == NULL){
            cout << endl;
            if(v.size() > 1){
                v.push_back(NULL);
            }
        }
        else if(v[0] == temp){
            cout << "x ";
        }
        else{
            cout << v[0]->value << " ";
            if(v[0]->left != NULL){
                v.push_back(v[0]->left);
            }
            else{
                v.push_back(temp);
            }
            if(v[0]->right != NULL){
                v.push_back(v[0]->right);
            }
            else{
                v.push_back(temp);
            }
        }
        v.erase(v.begin());
    }
}

void debug(Node* tree, long i){
    if(debug_mode){
        cout << "---" << i+2 << "---" << endl;
        print_tree(tree);
        cout << "---" << i+2 << "---" << endl;
    }
}