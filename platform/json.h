#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <sstream>

namespace json {

using namespace std::string_view_literals;

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    using Value = std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict>;

    const Value & GetValue() const;
    
    Node() = default;
    Node(std::nullptr_t value) : _value(value) {}
    Node(bool value) : _value(value) {}
    Node(int value) : _value(value) {}
    Node(double value) : _value(value) {}
    Node(std::string value) : _value(std::move(value)) {}
    Node(Array value) : _value(std::move(value)) {}
    Node(Dict value) : _value(std::move(value)) {}
   
    bool IsInt() const;
    bool IsDouble() const;     //Возвращает true, если в Node хранится int либо double.
    bool IsPureDouble() const; //Возвращает true, если в Node хранится double.
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;   //Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;

    bool operator==(const Node &other) const;
    bool operator!=(const Node &other) const;

private:
    Value _value = nullptr;
};

std::ostream & operator<<(std::ostream &out, const Node &node);

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& other) const;
    bool operator!=(const Document& other) const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json