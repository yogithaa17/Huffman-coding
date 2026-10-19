
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <array>

using namespace std;

class TreeNode {
private:
    char ch;
    int freq;
    TreeNode* left;
    TreeNode* right;

public:
    TreeNode(char character, int frequency,
             TreeNode* l = nullptr,
             TreeNode* r = nullptr)
        : ch(character), freq(frequency),
          left(l), right(r) {}

    char getChar() const {
        return ch;
    }

    int getFreq() const {
        return freq;
    }

    TreeNode* getLeft() const {
        return left;
    }

    TreeNode* getRight() const {
        return right;
    }

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

class Coder {
public:
    char ch;
    string code;

    Coder(char character, const string& huffmanCode)
        : ch(character), code(huffmanCode) {}
};

class Compare {
public:
    bool operator()(const TreeNode* a,
                    const TreeNode* b) const {
        return a->getFreq() > b->getFreq();
    }
};

// Free all dynamically allocated tree nodes.
void freeTree(TreeNode* root) {
    if (root == nullptr)
        return;

    freeTree(root->getLeft());
    freeTree(root->getRight());
    delete root;
}

// Build the Huffman tree.
TreeNode* buildTree(const string& str) {
    if (str.empty())
        return nullptr;

    array<int, 256> frequency{};

    for (unsigned char ch : str) {
        frequency[ch]++;
    }

    priority_queue<
        TreeNode*,
        vector<TreeNode*>,
        Compare
    > minHeap;

    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            minHeap.push(
                new TreeNode(static_cast<char>(i),
                             frequency[i])
            );
        }
    }

    while (minHeap.size() > 1) {
        TreeNode* left = minHeap.top();
        minHeap.pop();

        TreeNode* right = minHeap.top();
        minHeap.pop();

        TreeNode* parent = new TreeNode(
            '\0',
            left->getFreq() + right->getFreq(),
            left,
            right
        );

        minHeap.push(parent);
    }

    return minHeap.top();
}

// Generate Huffman codes for every leaf.
void encoding(TreeNode* root, string code,
              vector<Coder>& codes) {
    if (root == nullptr)
        return;

    if (root->isLeaf()) {
        // Give a single-character tree the code "0".
        if (code.empty())
            code = "0";

        codes.emplace_back(root->getChar(), code);
        return;
    }

    encoding(root->getLeft(), code + "0", codes);
    encoding(root->getRight(), code + "1", codes);
}

// Decode a bit string safely.
// Returns false if the bit string is invalid.
bool decoding(TreeNode* root,
              const string& encoded,
              string& result) {
    result.clear();

    if (root == nullptr)
        return encoded.empty();

    // Special case: only one distinct character.
    if (root->isLeaf()) {
        for (char bit : encoded) {
            if (bit != '0')
                return false;

            result += root->getChar();
        }
        return true;
    }

    TreeNode* temp = root;

    for (char bit : encoded) {
        if (bit == '0') {
            temp = temp->getLeft();
        }
        else if (bit == '1') {
            temp = temp->getRight();
        }
        else {
            result.clear();
            return false;
        }

        // Defensive check before accessing the node.
        if (temp == nullptr) {
            result.clear();
            return false;
        }

        if (temp->isLeaf()) {
            result += temp->getChar();
            temp = root;
        }
    }

    // A valid encoding must finish at the root.
    // Otherwise, the last code is incomplete.
    if (temp != root) {
        result.clear();
        return false;
    }

    return true;
}

int main() {
    string str;

    cout << "Enter text: ";
    getline(cin, str);

    TreeNode* root = buildTree(str);

    if (root == nullptr) {
        cout << "Input is empty.\n";
        return 0;
    }

    vector<Coder> codes;
    encoding(root, "", codes);

    // Store each character's code for quick lookup.
    array<string, 256> huffmanCodes{};

    for (const Coder& item : codes) {
        huffmanCodes[
            static_cast<unsigned char>(item.ch)
        ] = item.code;
    }

    cout << "\nHuffman Codes:\n";

    for (const Coder& item : codes) {
        cout << "'" << item.ch << "' : "
             << item.code << '\n';
    }

    // Encode the original text.
    string encoded;

    for (unsigned char ch : str) {
        encoded += huffmanCodes[ch];
    }

    cout << "\nEncoded string: " << encoded << '\n';

    // Decode the encoded text.
    string decoded;

    if (decoding(root, encoded, decoded)) {
        cout << "Decoded string: " << decoded << '\n';

        cout << "Original and decoded match: "
             << (decoded == str ? "Yes" : "No")
             << '\n';
    }
    else {
        cout << "Invalid encoded string.\n";
    }

    freeTree(root);

    return 0;
}
