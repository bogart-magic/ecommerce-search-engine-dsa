#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <set>

using namespace std;

// Product Structure
struct Product {
    int id;
    string name;
    string category;
    double price;
    int sold;
    int dateAdded; // Lower number = newer
};

// Cart Item Structure
struct CartItem {
    Product product;
    int quantity;
};

// ==========================================
// 1. TRIE DATA STRUCTURE (Prefix Search & Auto-complete)
// ==========================================
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    set<int> productIds; // Stores product IDs matching this prefix
    bool isEndOfWord;
    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    TrieNode* root;

    // Helper to convert string to lowercase
    string toLower(string str) const {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

public:
    Trie() {
        root = new TrieNode();
    }

    // Insert product name and words into Trie
    void insert(const string& text, int productId) {
        string lowerText = toLower(text);
        // Tokenize by space to allow prefix search on individual words (e.g., "Wireless" -> "Wire")
        string word = "";
        for (char c : lowerText) {
            if (c == ' ') {
                if (!word.empty()) {
                    insertWordPrefixes(word, productId);
                    word = "";
                }
            } else {
                word += c;
            }
        }
        if (!word.empty()) {
            insertWordPrefixes(word, productId);
        }
        // Also insert the full string
        insertWordPrefixes(lowerText, productId);
    }

    void insertWordPrefixes(const string& word, int productId) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
            curr->productIds.insert(productId);
        }
        curr->isEndOfWord = true;
    }

    // Search by prefix and return matching product IDs
    set<int> searchPrefix(const string& prefix) const {
        string lowerPrefix = toLower(prefix);
        TrieNode* curr = root;
        for (char c : lowerPrefix) {
            if (curr->children.find(c) == curr->children.end()) {
                return {}; // Prefix not found
            }
            curr = curr->children[c];
        }
        return curr->productIds;
    }
};

// ==========================================
// 3. MERGE SORT ALGORITHM (Product Rankings)
// ==========================================
class Sorter {
private:
    static void merge(vector<Product>& arr, int left, int mid, int right, bool (*comp)(const Product&, const Product&)) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        vector<Product> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (comp(L[i], R[j])) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    static void mergeSortHelper(vector<Product>& arr, int left, int right, bool (*comp)(const Product&, const Product&)) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid, comp);
        mergeSortHelper(arr, mid + 1, right, comp);
        merge(arr, left, mid, right, comp);
    }

public:
    static void sortProducts(vector<Product>& arr, bool (*comp)(const Product&, const Product&)) {
        if (arr.empty()) return;
        mergeSortHelper(arr, 0, arr.size() - 1, comp);
    }
};

// Comparator functions for Merge Sort
bool comparePriceLowToHigh(const Product& a, const Product& b) { return a.price < b.price; }
bool comparePriceHighToLow(const Product& a, const Product& b) { return a.price > b.price; }
bool comparePopularity(const Product& a, const Product& b) { return a.sold > b.sold; }
bool compareNewest(const Product& a, const Product& b) { return a.dateAdded < b.dateAdded; }

// ==========================================
// E-COMMERCE ENGINE SYSTEM
// ==========================================
class ECommerceEngine {
private:
    // 2. HASH TABLE: Fast O(1) lookups for products by ID
    unordered_map<int, Product> productTable;
    Trie nameTrie;
    vector<CartItem> cart;

    void displayProductTable(const vector<Product>& list) const {
        cout << "\n=================================================================\n";
        cout << left << setw(5) << "ID" 
             << setw(35) << "Product Name" 
             << setw(12) << "Category" 
             << setw(10) << "Price" 
             << setw(10) << "Sold" << endl;
        cout << "=================================================================\n";
        
        if (list.empty()) {
            cout << "No products found.\n";
            return;
        }

        for (const auto& p : list) {
            cout << left << setw(5) << p.id 
                 << setw(35) << (p.name.length() > 33 ? p.name.substr(0, 30) + "..." : p.name) 
                 << setw(12) << p.category 
                 << "P" << setw(9) << fixed << setprecision(2) << p.price 
                 << setw(10) << p.sold << endl;
        }
        cout << "=================================================================\n";
    }

public:
    ECommerceEngine() {
        // Load initial seed dataset
        vector<Product> initialProducts = {
            {1, "Wireless Noise-Canceling Headphones", "Electronics", 2499.00, 1250, 1},
            {2, "Smart Fitness Watch Tracker", "Electronics", 1299.00, 3400, 4},
            {3, "Bluetooth Waterproof Portable Speaker", "Electronics", 999.00, 2300, 9},
            {4, "Mechanical Gaming Keyboard RGB", "Electronics", 1850.00, 1540, 10},
            {5, "Minimalist Casual Canvas Backpack", "Fashion", 799.00, 890, 5},
            {6, "Classic Oversized Cotton Hoodie", "Fashion", 899.00, 2100, 3},
            {7, "Vintage Wash Denim Jacket", "Fashion", 1299.00, 740, 15},
            {8, "Breathable Running Sneakers", "Fashion", 1499.00, 2890, 19},
            {9, "Ergonomic Desk LED Lamp with USB", "Home", 550.00, 540, 2},
            {10, "Stainless Steel Insulated Tumbler 32oz", "Home", 450.00, 4120, 6},
            {11, "Aromatherapy Essential Oil Diffuser", "Home", 699.00, 1450, 21},
            {12, "Hydrating Facial Skincare Serum", "Beauty", 399.00, 1890, 7},
            {13, "Matte Liquid Lipstick Makeup Kit", "Beauty", 299.00, 4890, 27},
            {14, "Non-Slip Yoga Exercise Mat 10mm", "Sports", 650.00, 2150, 33},
            {15, "Resistance Workout Bands Set 5pcs", "Sports", 399.00, 4500, 35}
        };

        // Populate Hash Table and Trie
        for (const auto& p : initialProducts) {
            productTable[p.id] = p;       // O(1) Hash Table insertion
            nameTrie.insert(p.name, p.id);  // Trie prefix mapping
            nameTrie.insert(p.category, p.id);
        }
    }

    void showAllProducts() {
        vector<Product> list;
        for (const auto& pair : productTable) {
            list.push_back(pair.second);
        }
        // Default sort by ID
        sort(list.begin(), list.end(), [](const Product& a, const Product& b) { return a.id < b.id; });
        displayProductTable(list);
    }

    // Trie-based Prefix Search & Auto-complete
    void searchByPrefix() {
        cin.ignore();
        string query;
        cout << "\nEnter search prefix / keyword (Trie search): ";
        getline(cin, query);

        set<int> matchingIds = nameTrie.searchPrefix(query);
        vector<Product> results;
        for (int id : matchingIds) {
            if (productTable.find(id) != productTable.end()) {
                results.push_back(productTable[id]);
            }
        }

        cout << "\n--- Trie Search Results for \"" << query << "\" ---\n";
        displayProductTable(results);
    }

    // Hash Table O(1) Lookup by ID
    void lookupById() {
        int id;
        cout << "\nEnter Product ID for O(1) Hash Table lookup: ";
        cin >> id;

        if (productTable.find(id) != productTable.end()) {
            const Product& p = productTable[id];
            cout << "\n[Found via Hash Table O(1)]\n";
            cout << "ID: " << p.id << "\nName: " << p.name << "\nCategory: " << p.category 
                 << "\nPrice: P" << p.price << "\nSold: " << p.sold << endl;
        } else {
            cout << "Product with ID " << id << " not found in Hash Table.\n";
        }
    }

    // Merge Sort Ranking
    void rankAndSortProducts() {
        vector<Product> list;
        for (const auto& pair : productTable) {
            list.push_back(pair.second);
        }

        int choice;
        cout << "\n--- Merge Sort Ranking Options ---\n";
        cout << "1. Price: Low to High\n";
        cout << "2. Price: High to Low\n";
        cout << "3. Popularity (Most Sold)\n";
        cout << "4. Newest Arrivals\n";
        cout << "Select sorting choice (1-4): ";
        cin >> choice;

        if (choice == 1) {
            Sorter::sortProducts(list, comparePriceLowToHigh);
        } else if (choice == 2) {
            Sorter::sortProducts(list, comparePriceHighToLow);
        } else if (choice == 3) {
            Sorter::sortProducts(list, comparePopularity);
        } else if (choice == 4) {
            Sorter::sortProducts(list, compareNewest);
        } else {
            cout << "Invalid choice.\n";
            return;
        }

        displayProductTable(list);
    }

    void run() {
        int choice;
        do {
            cout << "\n=== E-Commerce Search Engine (DSA Core) ===" << endl;
            cout << "1. View All Products" << endl;
            cout << "2. Trie Prefix Search / Auto-Complete" << endl;
            cout << "3. Hash Table O(1) Product ID Lookup" << endl;
            cout << "4. Merge Sort Product Rankings" << endl;
            cout << "5. Exit" << endl;
            cout << "Enter your choice (1-5): ";
            cin >> choice;

            switch (choice) {
                case 1: showAllProducts(); break;
                case 2: searchByPrefix(); break;
                case 3: lookupById(); break;
                case 4: rankAndSortProducts(); break;
                case 5: cout << "Exiting Search Engine. Goodbye!\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 5);
    }
};

int main() {
    ECommerceEngine engine;
    engine.run();
    return 0;
}
