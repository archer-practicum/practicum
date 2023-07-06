#include <cassert>

template <typename T>
struct TreeNode {
    T value;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

template <typename T>
void DeleteTree(TreeNode<T>* node) {
    if (!node) {
        return;
    }
    DeleteTree(node->left);
    DeleteTree(node->right);
    delete node;
}

template<typename T>
bool checkSearchNode(const TreeNode<T> *node, const T *min, const T *max) {
    if (!node->left && !node->right) return true;

    if (node->left) {
        if (node->value <= node->left->value) return false;
        if (max) if (node->left->value >= *max) return false;
    }

    if (node->right) {
        if (!(node->value >= node->right->value)) return false;
        if (min) if (node->right->value <= *min) return false;
    }

    return true;
    
}

template <typename T>
bool CheckTreeProperty(const TreeNode<T>* node) {    

    if (node->right) {
        if (checkSearchNode(node->right, &node->value, nullptr)) {
            CheckTreeProperty(node->right);
        } else {
            return false;
        }
    }

    if (node->left) {
        if(checkSearchNode(node->left, nullptr, &node->value)) {
            CheckTreeProperty(node->left);
        } else {
            return false;
        }
    }

    return checkSearchNode(node, nullptr, nullptr);

}
int main() {
    using T = TreeNode<int>;
    T* root1 = new T{6, 
        new T{4, new T{3}, new T{5}}, new T{7}};
    assert(CheckTreeProperty(root1));

    T* root2 = new T{6, 
        new T{4, new T{3}, new T{5}}, new T{7, new T{8}}};
    assert(!CheckTreeProperty(root2));

    T* root3 = new T{5,
            new T{3, new T{1}, new T{7}}, new T{6, new T{2}, new T{8}}};
    assert(!CheckTreeProperty(root3));

    T* root4 = new T{6, 
            new T{4, new T{3}, new T{3}}, new T{9, new T{7}}};
            assert(!CheckTreeProperty(root4));

    DeleteTree(root1);
    DeleteTree(root2);
    DeleteTree(root3);
    DeleteTree(root4);
}