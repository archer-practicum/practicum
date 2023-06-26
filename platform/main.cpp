#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <future>
#include <execution>
#include <vector>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (const auto&[word, freq] : other.word_frequences) {
            word_frequences[word] += freq;
        }
    }
};

using KeyWords = set<string, less<>>;

vector<string> SplitIntoWords(const string &str) {
    vector<string> res;
    string word = "";
    for (const char c : str) {
        if(c != ' ') {
            word += c;
        } else {
            if (!word.empty()) {
                res.push_back(move(word));
            }
        }
    }
    if (!word.empty()) {
        res.push_back(move(word));
    }
    return res;
}

Stats StringToStats(const vector<string> &&strs, const KeyWords& key_words) {
    Stats res;
    for (const string &str : strs) {
        for (const string word : SplitIntoWords(str)) {
            if (key_words.count(word)) {
                res.word_frequences[word] += 1;
            }        
        }
    }    
    return res;
}

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    string str;
    vector<string> words;
    words.reserve(5000);
    vector<future<Stats>> pats_stats;
    Stats res;

    while (!input.eof()) {
        getline(input, str);
        if (words.size() == 5000) {
            pats_stats.push_back(move(async(StringToStats, move(words), cref(key_words))));
            words.reserve(5000);
        }
        words.push_back(move(str));
    }
    pats_stats.push_back(move(async(StringToStats, move(words), cref(key_words))));
    
    for (auto &el : pats_stats) {
        res += el.get();
    }
    return res;
}

int main() {
    const KeyWords key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    // ss << "10 reasons why yangle is the best IT company\n";
    // ss << "yangle rocks others suck\n";
    // ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }

    for (const string &word : SplitIntoWords("  this new yangle service really rocks  ")) {
        cout << word << endl;
    }

    return 0;
}