#include <iostream>

using namespace std;

struct Node {
    Node() : pred(0), suc(0) {}
    Node(Node *p, Node *s) : pred(p), suc(s) {}
    Node *pred; // Predecessor
    Node *suc; // Successor
};

struct DataNode : public Node {
    DataNode(int i) : some_data(i) {}
    const int some_data;
};

// Insert before pos.
void
insert(Node *pos, Node *n) {
    n->pred = pos->pred;
    n->suc = pos;
    pos->pred->suc = n;
    pos->pred = n;
}

// Remove.
void
remove(Node *n) {
    n->pred->suc = n->suc;
    n->suc->pred = n->pred;
    n->pred = n->suc = 0;
}

template <Node *Node::*ptr> struct Traits {
    static const char *const order_type;
};
// Template specializations.
template <> const char *const Traits<&Node::pred>::order_type = "reverse";
template <> const char *const Traits<&Node::suc>::order_type = "forward";

void
print_forward(const Node &sent) {
    for (Node *n = sent.suc; n != &sent; n = n->suc) {
        cout << static_cast<DataNode *>(n)->some_data << endl;
    }
}

template <Node *Node::*ptr>
void
print_list(const Node &sent) {
    cout << "Printing list in " << Traits<ptr>::order_type << " order: " << endl;
    for (Node *n = sent.*ptr; n != &sent; n = n->*ptr) {
        cout << static_cast<DataNode *>(n)->some_data << endl;
    }
}

int
main() {

    Node sentinel;
    sentinel.suc = sentinel.pred = &sentinel;

    // Insert at end.
    insert(&sentinel, new DataNode(1));
    // Insert at end.
    insert(&sentinel, new DataNode(2));
    print_list<&Node::suc>(sentinel);

    // Insert at beginning.
    insert(sentinel.suc, new DataNode(-1));
    insert(sentinel.suc, new DataNode(-2));
    print_list<&Node::suc>(sentinel);

    // Remove first.
    remove(sentinel.suc);

    // Remove last.
    remove(sentinel.pred);

    // Traverse in reverse order.
    print_list<&Node::pred>(sentinel);

    // Should delete memory at the end.
}
