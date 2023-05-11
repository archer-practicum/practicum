#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Объявляем Sentence<Token> для произвольного типа Token
// синонимом vector<Token>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор предложений — vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>;

template <typename TokenForwardIt>
TokenForwardIt FindSentenceEnd(TokenForwardIt tokens_begin, TokenForwardIt tokens_end) {
    const TokenForwardIt before_sentence_end
        = adjacent_find(tokens_begin, tokens_end, [](const auto& left_token, const auto& right_token) {
              return left_token.IsEndSentencePunctuation() && !right_token.IsEndSentencePunctuation();
          });
    return before_sentence_end == tokens_end ? tokens_end : next(before_sentence_end);
}

// Класс Token имеет метод bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    auto iter_end = FindSentenceEnd(tokens.begin(), tokens.end());
    auto iter_begin = tokens.begin();
    while (1) {
        Sentence<Token> tmp;
        tmp.reserve(iter_end - iter_begin);
        for (auto it = iter_begin; it != iter_end; ++it) tmp.push_back(move(*it));
        res.push_back(move(tmp));
        if (iter_end == tokens.end()) break;
        iter_begin = iter_end;
        iter_end = FindSentenceEnd(iter_end, tokens.end());
    }

    return res;
}

struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

// Тест содержит копирования объектов класса TestToken.
// Для проверки отсутствия копирований в функции SplitIntoSentences
// необходимо написать отдельный тест.
void TestSplitting() {
    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}}));

    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}}));

    assert(SplitIntoSentences(vector<TestToken>(
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}}))
           == vector<Sentence<TestToken>>({
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}},
               {{"Without"s}, {"copies"s}, {"."s, true}},
           }));
}

int main() {
    TestSplitting();
    auto res1 = SplitIntoSentences(vector<TestToken>(
        {
            {"Красиво"s}, {"ты"s}, {"вошла"s}, {"в"s}, {"мою"s}, {"грешную"s}, {"жизнь"s}, {"!"s, true}, 
            {"Устремленно"s}, {"ты"s}, {"ушла"s}, {"из"s}, {"неё"s}, {"."s, true} 
        })
    );
    for (auto container : res1) {
        for (auto el : container) {
            cout << el << " ";
        }
        cout << endl;
    }

}