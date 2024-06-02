#include "json.h"

using namespace std;

namespace json {
    //load
    namespace { 

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            //[1,1.23,\"Hello\"]
            Array result;
            char c;
            for (c = ' '; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("Failed to create an array"s);
            }
            return Node(move(result));
        }

        Node LoadBool(istream& input) {
            char c;
            string s;
            while (input >> c && c != ',' && c != ' ' && c != '}')
            {
                s += c;
            }
            if (c == '}') { input.putback(c);}
            if (s == "rue") { return Node{ true }; }
            if (s == "alse") { return Node{ false }; }
            throw ParsingError("bool is to be expected"s);
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        Node LoadString(std::istream& input) {
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
                }
                else if (ch == '\\') {
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
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }
            return Node(move(s));
        }

        Node LoadDict(istream& input) {
            Dict result;
            char c;
            for (c = ' '; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (c != '}' ){
                throw ParsingError("Failed to create a Map"s);
            }

            return Node(move(result));
        }

        using Number = std::variant<int, double>;

        Node LoadNumber(std::istream& input) {
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
            }
            else {
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
                        return Node(std::move(std::stoi(parsed_num)));
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::move(std::stod(parsed_num)));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadNull(istream& input) {
            char c;
            string s;
            while (input >> c && c != ',' && c != ' ')
            {
                s += c;
            }
            if (s == "ull") { return Node{}; }
            throw ParsingError("null is to be expected"s);
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            switch (c)
            {
            case ('['):
                return LoadArray(input);
            case ('{'):
                return LoadDict(input);
            case ('"'):
                return LoadString(input);
            case ('n'):
                return LoadNull(input);
            case ('t'):
                return LoadBool(input);
            case ('f'):
                return LoadBool(input);
            case ('}'):
                throw ParsingError("Map is closed before initiation"s);
            case(']'):
                throw ParsingError("Array is closed before initiation"s);
            default:
                input.putback(c);
                return LoadNumber(input);
                break;
            }
        }

    }  // namespace

    Node::Node(std::nullptr_t)
        : value_(nullptr) {
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble()const { //Возвращает true, если в Node хранится int либо double.
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);
    }
    bool Node::IsPureDouble()  const { //Возвращает true, если в Node хранится double.
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsNull() const {
        return std::holds_alternative<nullptr_t>(value_);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap()const {
        return std::holds_alternative<Dict>(value_);
    }

    const Array& Node::AsArray() const {
        if (const Array* array_value = std::get_if<Array>(&value_)) {
            return *array_value;
        }
        throw std::logic_error("Invalid variant type: not an array");
    }
    const Dict& Node::AsMap() const {
        if (const Dict* dict_value = std::get_if<Dict>(&value_)) {
            return *dict_value;
        }
        throw std::logic_error("Invalid variant type: not a map");
    }
    int Node::AsInt() const {
        if (const int* int_value = std::get_if<int>(&value_)) {
            return *int_value;
        }
        throw std::logic_error("Invalid variant type: not an int");
    }
    const std::string& Node::AsString() const {
        if (const std::string* str_value = std::get_if<std::string>(&value_)) {
            return *str_value;
        }
        throw std::logic_error("Invalid variant type: not a string");
    }
    double Node::AsDouble() const {// .Возвращает значение типа double, если внутри хранится double либо int.В последнем случае возвращается приведённое в double значение.
        if (const double* double_value = std::get_if<double>(&value_)) {
            return *double_value;
        }
        if (const int* double_value = std::get_if<int>(&value_)) {
            return *double_value;
        }
        throw std::logic_error("Invalid variant type: not a double or int");
    }
    [[nodiscard]] bool Node::AsBool() const {
        if (const bool* bool_value = std::get_if<bool>(&value_)) {
            return *bool_value;
        }
        throw std::logic_error("Invalid variant type: not an int");
    }

    bool Node::operator==(Node other) const {
        return GetValue() == other.GetValue();
    }
    bool Node::operator!=(Node other) const {
        return !(*this == other);
    }

    //-------------------Print Indent-------------------------------------

        std::string Indent::GetIndent() {
            std::string s;
            
            for (size_t i = 0; i < indent_; i++)
            {
                s.append(ind);
            }
            return s;
        }

        void Indent::operator()(std::ostream& out) {
            out << GetIndent();
        }

        void Indent::operator++() {
            ++indent_;
        }
        void Indent::operator--() {
            --indent_;
        }
    //-------------------Print-------------------------------------

    void PrintNode(const Node& node, std::ostream& out, Indent& indent) {
        std::visit(
            [&out, &indent](const auto& value) { PrintValue(value, out, indent); },
            node.GetValue());
    }

    void PrintValue(const std::string& value, std::ostream& out, Indent& /*indent*/) {
        out << "\"";
        for (char c : value) {
            switch (c)
            {
            case('\\'):
                out << "\\\\";
                break;
            case('\r'):
                out << "\\r";
                break;
            case('\n'):
                out << "\\n";
                break;
            case('\t'):
                out << "\\t";
                break;
            case('\"'):
                out << "\\\"";
                break;
            default:
                out << c;
                break;
            }
        }
        out << "\"";
    }

    void PrintValue(const bool value, std::ostream& out, Indent& /*indent*/) {
        out << (value ? "true" : "false");
    }

    template <typename Value>
    void PrintValue(const Value& value, std::ostream& out, Indent& /*indent*/) {
        out << value;
    }

    void PrintValue(const Dict& value, std::ostream& out, Indent& indent) {
        //{ \"key1\": \"value1\", \"key2\": 42 }
        out << "{\n";
        ++indent;
        bool first = true;
        for (auto& v : value) {
            if (!first) { out << "," << "\n"; }
            else {
                first = false;
            }
            indent(out);
            out << "\"";
            out << v.first;
            out << "\"";
            out << " : ";
            PrintNode(v.second, out, indent);
        }
        --indent;
        out << "\n";
        indent(out);
        out << "}";
    }

    void PrintValue(const Array& value, std::ostream& out, Indent& indent) {
        //"[1,1.23,\"Hello\"]"s
        out << "[\n";
        ++indent;

        bool first = true;
        for (auto& v : value) {
            if (!first) { out << ",\n"; }
            else {
                first = false;
            }
            indent(out);
            PrintNode(v, out, indent);
        }
        --indent;
        out << "\n";
        indent(out);
        out << "]";
    }

    void PrintValue(std::nullptr_t, std::ostream& out, Indent& /*indent*/) {
        out << "null"sv;
    }

    //-------------------Document-------------------------------------


    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document& other) const {
        return other.root_ == root_;
    }
    bool Document::operator!=(const Document& other) const {
        return !(other == *this);
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        Indent indent;
        PrintNode(doc.GetRoot(), output, indent);
    }
}  // namespace json