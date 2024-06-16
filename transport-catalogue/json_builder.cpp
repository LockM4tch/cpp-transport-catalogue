#include "json_builder.h"

namespace json{
	//-----------------------BaseContext----------------------------------------------------------
	BaseContext BaseContext::Value(Node::Value value) {
		return BaseContext{ bldr_.InsertValue(value) };
	}
	DictKeyContext BaseContext::Key(std::string key) {
		return bldr_.Key(key);
	}
	DictContext BaseContext::StartDict() {
		return bldr_.StartDict();
	}
	ArrayContext BaseContext::StartArray() {
		return bldr_.StartArray();
	}
	BaseContext BaseContext::EndDict() {
		return bldr_.EndDict();
	}
	BaseContext BaseContext::EndArray() {
		return bldr_.EndArray();
}
	Node BaseContext::Build() {
		return bldr_.Build();
	}

	//-----------------------DictContext----------------------------------------------------------

	//-----------------------DictKeyContext----------------------------------------------------------
	DictContext DictKeyContext::Value(Node::Value value) {
		return DictContext{ bldr_.InsertValue(value) };
	}
	//-----------------------ArrayContext----------------------------------------------------------

	ArrayContext ArrayContext::Value(Node::Value value) {
		return bldr_.InsertValue(value);
	}
	//-----------------------InitialValueContext----------------------------------------------------------
	Node InitialValueContext::Build(){
			return bldr_.Build();
	}

    //-----------------------Builder----------------------------------------------------------
	InitialValueContext Builder::Value(Node::Value value) { //entry point
			IsClosed();
				root_ = Node{ std::move(value) };
				return *this;
		}
		DictContext Builder::StartDict() {  //entry point
			IsClosed();

			nodes_tracker_.emplace_back(new Node{ Dict{} });
			return DictContext{*this};
		}
		ArrayContext Builder::StartArray() {  //entry point
			IsClosed();

			nodes_tracker_.emplace_back(new Node{ Array{} });
			return ArrayContext{ *this };
		}

		Builder& Builder::Key(std::string key) {
			if (!nodes_tracker_.empty()) {
				if (!nodes_tracker_.back()->IsDict()) {
					throw std::logic_error("Dict is not initiallised");
				}

				Node* str = new Node{ std::string(key) };
				nodes_tracker_.emplace_back(str);
			}
			else {
				throw std::logic_error("Dict is not initiallised");
			}
			return *this;
		}
		Builder& Builder::EndDict() {
			if (nodes_tracker_.empty() || !nodes_tracker_.back()->IsDict()) {
				throw std::logic_error("Dict is closed before initialisation");
			}
			else
			{
				auto last = nodes_tracker_.back();
				nodes_tracker_.pop_back();
				InsertValue(last->AsModifiableDict());
			}
			return *this;
		}
		Builder& Builder::EndArray() {
			if (nodes_tracker_.empty() || !nodes_tracker_.back()->IsArray()) {
				throw std::logic_error("Array is closed before initialisation");
			}
			else
			{
				auto last = nodes_tracker_.back();
				nodes_tracker_.pop_back();
				InsertValue(last->AsModifiableArray());
			}
			return *this;
		}

		Node Builder::Build() {
			if (!nodes_tracker_.empty()) {
				throw std::logic_error("Root has no proper ending");
			}
			else if (root_ == nullptr) {
				throw std::logic_error("Root has no proper ending");
			}
			else if (IsBuilt)
			{
				throw std::logic_error("Build is already completed");
			}

			IsBuilt = true;
			return root_;
		};

		Builder& Builder::InsertValue(Node::Value CurrentValue) {
			if (nodes_tracker_.empty() && root_ == nullptr) {
				root_ = Node{ std::move(CurrentValue) };
			}
			else if (root_ != nullptr) {
				throw std::logic_error("root is alredy created");
			}
			else if (nodes_tracker_.empty()) {
				throw std::logic_error("no previous element to use");
			}
			else if (auto last = nodes_tracker_.back(); last->IsString()) {
				nodes_tracker_.pop_back();
				std::string key = last->AsString();
				nodes_tracker_.back()->AsModifiableDict()[key] = std::move(CurrentValue);
			}
			else if (last->IsArray()) {
				last->AsModifiableArray().emplace_back(std::move(CurrentValue));
			}
			else {
				throw std::logic_error("Incorrect value");
			}
			return *this;
		}





}