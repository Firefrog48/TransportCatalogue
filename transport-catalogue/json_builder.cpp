#include "json_builder.h"

namespace JSON {

    Builder::DictItemContext Builder::StartDict() { 
        if (nodes_stack_.empty()) {
            root_ = Dict{};
            nodes_stack_.emplace_back(&root_.value());   
        } else if (nodes_stack_.back()->IsDict()) {
            TryAddToDict("StartDict"s, Dict{});
        } else if (nodes_stack_.back()->IsArray()) {
            TryAddToArray(Dict{});
        } else {
            throw std::logic_error("Unknown error calling \"StartDict\""s);
        }
        return DictItemContext{*this};
    }


    Builder::KeyItemContext Builder::Key(std::string &&key) {
        IsBuilded();
        if (nodes_stack_.empty()) {
            throw std::logic_error("You need to call \"StartDict\" before calling \"Key\""s);
        } else if (!nodes_stack_.back()->IsDict()) {
            throw std::logic_error("You need to call \"StartDict\" before calling \"Key\""s);
        } else if (key_temp_.has_value()) {
            throw std::logic_error("You call \"Key\" after calling \"Key\""s);
        }
        key_temp_ = std::move(key);
        return KeyItemContext{*this};
    }



    Builder& Builder::Value(Node &&node) {  
        if (root_.has_value() && nodes_stack_.empty()) {
            throw std::logic_error("root already have value"s);
        } else if (nodes_stack_.empty()) {
            root_ = std::move(node);
        } else if (nodes_stack_.back()->IsArray()) {
            TryAddToArray(std::move(node), true);
        } else if (nodes_stack_.back()->IsDict()) {
            TryAddToDict("Value"s, std::move(node), true);
        } else {
            throw std::logic_error("Unknown error calling \"Value\""s);
        }
        return *this;
    }

    Builder::ArrayItemContext Builder::StartArray() {  
        if (nodes_stack_.empty()) {
            root_ = Array{};
            nodes_stack_.emplace_back(&root_.value());   
        } else if (nodes_stack_.back()->IsDict()) {
            TryAddToDict("StartArray"s, Array{});
        } else if (nodes_stack_.back()->IsArray()) {
            TryAddToArray(Array{});
        } else {
            throw std::logic_error("Unknown error calling \"StartArray\""s);
        }
        return ArrayItemContext{*this};
    }

    Builder &Builder::EndDict() {
        IsBuilded();   
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("You need to call \"StartDict\" before calling \"EndDict\""s);
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder &Builder::EndArray() {
        IsBuilded(); 
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
            throw std::logic_error("You need to call \"StartArray\" before calling \"EndArray\""s);
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Node &Builder::Build() {
        IsBuilded(); 
        if (!nodes_stack_.empty() && (nodes_stack_.back()->IsArray() || nodes_stack_.back()->IsDict())) {
            throw std::logic_error("You need to call \"EndArray\" or \"EndDict\""s);
        }
        return root_.value();
    }

    void Builder::IsBuilded() const {
        if (!root_.has_value()) {
            throw std::logic_error("You need to make json::Builder"s);
        }
    }

    void Builder::TryAddToDict(std::string&& function_name, Node&& node,[[maybe_unused]] bool call_from_value) {
        if (!key_temp_.has_value()) {
            throw std::logic_error("You need to add key before calling \""s + function_name + '\"');
        }
        std::get<Dict>(nodes_stack_.back()->GetValue())[key_temp_.value()] = std::move(node);
        if (!call_from_value) {
            nodes_stack_.emplace_back(&(std::get<Dict>(nodes_stack_.back()->GetValue())[key_temp_.value()]));
        }
        key_temp_.reset();
    }

    void Builder::TryAddToArray(Node&& node,[[maybe_unused]] bool call_from_value) {
        std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(std::move(node));
        if (!call_from_value) {
            nodes_stack_.emplace_back(&(std::get<Array>(nodes_stack_.back()->GetValue()).back()));
        }

    }

    Builder::KeyItemContext Builder::DictItemContext::Key(std::string &&key) {
        return ref_.Key(std::move(key));
    }

    Builder &Builder::DictItemContext::EndDict() {
        return ref_.EndDict();
    }

    Builder::DictItemContext Builder::KeyItemContext::Value(Node &&node) {
        return DictItemContext{ref_.Value(std::move(node))};
    }

    Builder::ArrayItemContext Builder::KeyItemContext::StartArray() {
        return ref_.StartArray();
    }

    Builder::DictItemContext Builder::KeyItemContext::StartDict() {
        return ref_.StartDict();
    }

    Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node &&node) {
        return ArrayItemContext{ref_.Value(std::move(node))};
    }

    Builder::ArrayItemContext Builder::ArrayItemContext::StartArray() {
        return ref_.StartArray();
    }

    Builder::DictItemContext Builder::ArrayItemContext::StartDict() {
        return ref_.StartDict();
    }

    Builder &Builder::ArrayItemContext::EndArray() {
        return ref_.EndArray();
    }


} // namespace json