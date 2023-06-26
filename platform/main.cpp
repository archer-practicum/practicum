#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <future>
#include <execution>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (const auto&[word, freq] : other.word_frequences) {
            word_frequences[word] += freq;
        }
    }
};

Stats operator+ (Stats const lhs, Stats const rhs) {
    Stats tmp(lhs);
    tmp += rhs;
    return tmp;
}

using KeyWords = set<string, less<>>;

// Stats StringToStats(const vector<string> &&words, const KeyWords& key_words) {    
//     Stats res;
//     for (const string &word : key_words) {
//         res.word_frequences[word] = 0;
//     }
//     for_each (execution::par, words.begin(), words.end(), 
//         [&](const string &word) { 
//             if (key_words.count(word)) {
//                 res.word_frequences[word] += 1;
//             };
//         }
//     );
//     return res;
// }

Stats StringToStats(const vector<string> &&words, const KeyWords& key_words) {    
    Stats res;
    for (const string word : words) {
        if (key_words.count(word)) {
            res.word_frequences[word] += 1;
        }        
    }
    return res;
}

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    string word;
    vector<string> words;
    words.reserve(5000);
    vector<future<Stats>> pats_stats;
    Stats res;

    while (input >> word) {
        if (words.size() == 5000) {
            pats_stats.push_back(move(async(StringToStats, move(words), cref(key_words))));
            words.reserve(5000);
        }
        words.push_back(move(word));
    }
    pats_stats.push_back(move(async(StringToStats, move(words), cref(key_words))));

    return transform_reduce(//execution::par, 
                    pats_stats.begin(), pats_stats.end(),
                    Stats{},
                    [](auto &el) { return el.get(); },
                    [](auto &l, auto &r){ return l + r; });
    // for (auto &el : pats_stats) {
    //     res += el.get();
    // }

    // vector<Stats> all_stats;
    // for (auto &el : pats_stats) {
    //     all_stats.push_back(move(el.get()));
    // }
    // return reduce(execution::par, all_stats.begin(), all_stats.end(), Stats{}, [](const auto &l, const auto &r) { return l + r; });
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

    return 0;
}