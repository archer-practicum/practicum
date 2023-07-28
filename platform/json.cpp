#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);
using Number = std::variant<int, double>;

Node LoadArray(istream& input) {
    Array result;
    char c;    
    for (; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (c != ']') throw ParsingError("Array must end with ']'");
    return Node(move(result));
}

Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
std::string LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}


Node LoadDict(istream& input) {
    Dict result;
    char c;
    for (; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    if (c != '}') throw ParsingError("Dict must end with '}'");
    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    switch (c) {
    case '[':
        return LoadArray(input);
    case '{':
        return LoadDict(input);
    case '"':
        return LoadString(input);
    case 'n': {
        input.putback(c);
        static const std::string str_null{"null"s};
        //char str[str_null.size()]; // practicum error compilation: ISO C++ forbids variable length array ‘str’ [-Werror=vla]
        char str[4];
        input.get(str, str_null.size() + 1);
        if (str != str_null) throw ParsingError("expected " + str_null + " current " + str);
        return nullptr;
    }     
    case 't': {
        input.putback(c);
        static const std::string str_true{"true"s};
        char str[4];
        input.get(str, str_true.size() + 1);
        if (str != str_true) throw ParsingError("expected " + str_true + " current " + str);
        return true;
    }
    case 'f': {
        input.putback(c);
        static const std::string str_false{"false"s};
        char str[5];
        input.get(str, str_false.size() + 1);
        if (str != str_false) throw ParsingError("expected " + str_false + " current " + str);
        return false;
    }
    default: {
        input.putback(c);
        const Number &number = LoadNumber(input);
        if (std::holds_alternative<int>(number)) return std::get<int>(number);
        return std::get<double>(number);
    }
    }
}



}  // namespace

const Node::Value & Node::GetValue() const {
    return _value;
}

bool Node::IsInt() const {
    return std::holds_alternative<int>(_value);
}

bool Node::IsDouble() const {
    return std::holds_alternative<int>(_value) || std::holds_alternative<double>(_value);
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(_value);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(_value);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(_value);
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(_value);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(_value);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(_value);
}

int Node::AsInt() const {
    if(!IsInt()) throw std::logic_error("Node does not contain a int value"s);
    return std::get<int>(_value);
}

bool Node::AsBool() const {
    if(!IsBool()) throw std::logic_error("Node does not contain a bool value"s);
    return std::get<bool>(_value);
}

double Node::AsDouble() const {
    if(!IsDouble()) throw std::logic_error("Node does not contain a double value"s);
    if(IsInt()) return std::get<int>(_value);
    return std::get<double>(_value);
}

const std::string& Node::AsString() const {
    if(!IsString()) throw std::logic_error("Node does not contain a std::string value"s);
    return std::get<std::string>(_value);
}

const Array& Node::AsArray() const {
    if(!IsArray()) throw std::logic_error("Node does not contain a Array value"s);
    return std::get<Array>(_value);
}

const Dict& Node::AsMap() const {
    if(!IsMap()) throw std::logic_error("Node does not contain a Map(Dict) value"s);
    return std::get<Dict>(_value);
}

bool Node::operator==(const Node &other) const {
    return _value == other._value;
}

bool Node::operator!=(const Node &other) const {
    return !(*this == other);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document& other) const {
    return root_ == other.root_;
}

bool Document::operator!=(const Document& other) const {
    return !(*this == other);
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    output << doc.GetRoot();
}


namespace print_value_node {
    inline void PrintValue(std::nullptr_t, std::ostream &out) { out << "null"sv; }
    inline void PrintValue(bool value, std::ostream &out) { value ? out << "true"sv : out << "false"sv; }
    inline void PrintValue(int value, std::ostream &out) { out << value; }
    inline void PrintValue(double value, std::ostream &out) { out << value; }

    inline void PrintValue(const std::string &value, std::ostream &out) {
        std::stringstream res;
        res << "\""sv;
        for (const char c : value) {
            switch (c) {
            case '\n':
                res << "\\n"sv;
                break;
            case '\r':
                res << "\\r"sv;
                break;
            case '\"':
                res << "\\\""sv;                
                break;
            case '\t':
                res << "\t"sv;
                break;
            case '\\':
                res << "\\\\"sv;
                break;            
            default:
                res << c;
                break;
            }
        }
        res << "\""sv;
        out << res.str(); 
    }
    
    inline void PrintValue(const Array &value, std::ostream &out) {
        out << "["sv;
        bool first = true;
        for (const auto &el : value) {
            if (first) first = false;
            else out << ", "sv;
            out << el;
        }
        out << "]"sv;
    }

    inline void PrintValue(const Dict &value, std::ostream &out) {
        out << "{"sv;
        bool first = true;
        for (const auto &[key, value]: value) {
            if (first) first = false;
            else out << ", "sv;
            out << "\""sv << key << "\": "sv << value;
        }
        out << "}"sv;
    }
} // namespace print_node


std::ostream & operator<<(std::ostream &out, const Node &node) {
    std::visit([&out](const auto &value) { print_value_node::PrintValue(value, out); }
    , node.GetValue());
    return out;
}

}  // namespace json