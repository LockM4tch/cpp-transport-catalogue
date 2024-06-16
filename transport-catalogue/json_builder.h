#include <string>
#include "json.h"

using namespace std::literals;
namespace json {
	class Builder;
	class BaseContext;

	class DictContext;
	class DictKeyContext;
	class DictValueContext;
	class ArrayContext;



	class BaseContext
	{
		friend class Builder;
	public:
		BaseContext(Builder& bldr) : bldr_(bldr) {}
		BaseContext Value(Node::Value value);
		DictKeyContext Key(std::string key);

		DictContext StartDict();
		ArrayContext StartArray();

		BaseContext EndDict();
		BaseContext EndArray();

		Node Build();

		~BaseContext() = default;

	protected:
		Builder& bldr_;
	};

	// вызовом StartDict следует не Key и не EndDict.
	class DictContext : public BaseContext {
		friend class Builder;
	public:
		DictContext(Builder& bldr) : BaseContext(bldr) {}

		BaseContext Value(Node::Value value) = delete;
		DictContext StartDict() = delete;
		ArrayContext StartArray() = delete;
		BaseContext EndArray() = delete;
		Node Build() = delete;

		~DictContext() = default;
	};

	//Непосредственно после Key вызван не Value, не StartDict и не StartArray.
	class DictKeyContext : public BaseContext {
	public:
		DictKeyContext(Builder& bldr) : BaseContext(bldr) {}

		DictContext Value(Node::Value value);

		DictKeyContext Key(std::string key) = delete;
		BaseContext EndDict() = delete;
		BaseContext EndArray() = delete;
		Node Build() = delete;

		~DictKeyContext() = default;
	};

	//За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
	class ArrayContext : public BaseContext {
	public:
		ArrayContext(Builder& bldr) : BaseContext(bldr) {}

		ArrayContext Value(Node::Value value);

		DictKeyContext Key(std::string key) = delete;
		BaseContext EndDict() = delete;
		Node Build() = delete;

		~ArrayContext() = default;
	};
	//Если value был перывм то можно только закрыть билдер
	class InitialValueContext : private BaseContext {
	public:
		InitialValueContext(Builder& bldr) : BaseContext(bldr) {}
		Node Build();
		~InitialValueContext() = default;
	};

	class Builder {
		friend class BaseContext;
		friend class DictContext;
		friend class DictKeyContext;
		friend class ArrayContext;
		friend class InitialValueContext;
	public:
		Builder() = default;

		InitialValueContext Value(Node::Value value); //entry point
		DictContext StartDict(); // entry point
		ArrayContext StartArray(); //entry point

		Node Build();

		~Builder() = default;
	private:
		Builder& Key(std::string key);
		Builder& EndDict();
		Builder& EndArray();
		Builder& InsertValue(Node::Value CurrentValue);
		
		void IsClosed() {
			if (root_ != nullptr) {
				throw std::logic_error("Root is defined");
			}
		}

		bool IsBuilt = false;
		Node root_ = nullptr;
		std::vector<Node*> nodes_tracker_;
	};
}
