#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

namespace json {
    using namespace std::literals;



    class Node;
    // ��������� ���������� Dict � Array ��� ���������
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // ��� ������ ������ ������������� ��� ������� �������� JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    using Value = std::variant<std::nullptr_t, Array, Dict, int, double, std::string, bool>;

    class Node {
    public:
        /* ���������� Node, ��������� std::variant */

        Node() = default;
        Node(std::nullptr_t);
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(double value);
        Node(std::string value);
        Node(bool value);

        const Value& GetValue() const {
            return value_;
        }

        bool IsInt() const;
        bool IsDouble() const; //���������� true, ���� � Node �������� int ���� double.
        bool IsPureDouble() const; //���������� true, ���� � Node �������� double.
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;// .���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool operator==(Node node) const;
        bool operator!=(Node node) const;

    private:
        Value value_;
    };

    class Indent {
    public:
        Indent() = default;

        std::string GetIndent();

        void operator()(std::ostream& out);

        void operator++();
        void operator--();

    private:
        int indent_ = 0;
        std::string ind = "    ";
    };


    void PrintNode(const Node& node, std::ostream& out, Indent& indent);

    void PrintValue(const std::string& value, std::ostream& out, Indent& indent);

    void PrintValue(const bool value, std::ostream& out, Indent& indent);

    // ������, ���������� ��� ������ double � int
    template <typename Value>
    void PrintValue(const Value& value, std::ostream& out, Indent& indent);

    void PrintValue(const Dict& value, std::ostream& out, Indent& indent);

    void PrintValue(const Array& value, std::ostream& out, Indent& indent);
    // ���������� ������� PrintValue ��� ������ �������� null
    void PrintValue(std::nullptr_t, std::ostream& out, Indent& indent);

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