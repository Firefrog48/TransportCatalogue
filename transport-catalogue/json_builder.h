#pragma once

#include "json.h"

#include <optional>

namespace JSON {
    

    using namespace std::literals;
    class Builder {
    public:
        class BaseItemContext;
        class KeyItemContext;
        class DictItemContext;
        class ArrayItemContext;
        
        Builder() = default;
        
        Builder(Builder&) = delete;
        Builder& operator=(Builder&) = delete;

        Builder(Builder&& other) = default;
        Builder& operator=(Builder&& other) = default;
        
        ~Builder() = default;

        DictItemContext StartDict();
        KeyItemContext Key(std::string&& key);
        Builder& Value(Node&& node);
        ArrayItemContext StartArray();
        Builder& EndDict();
        Builder& EndArray();

        Node& Build();
             
    private:
        void IsBuilded() const;
        
        void TryAddToDict(std::string&& function_name, Node&& node,[[maybe_unused]] bool call_from_value = false);
        void TryAddToArray(Node&& node,[[maybe_unused]] bool call_from_value = false);
        
        std::optional<Node> root_;
        std::optional<std::string> key_temp_;
        std::vector<Node*> nodes_stack_;
    };

    class Builder::BaseItemContext {
    public:
        BaseItemContext(Builder& builder) : ref_(builder) {}
    protected:
        Builder& ref_;
    private:
    };

    class Builder::DictItemContext : public Builder::BaseItemContext{
    public:
        KeyItemContext Key(std::string&& key);
        Builder& EndDict();
    };

    class Builder::KeyItemContext : public Builder::BaseItemContext {
    public:
        DictItemContext Value(Node&& node);
        ArrayItemContext StartArray();
        DictItemContext StartDict();
    };

    class Builder::ArrayItemContext : public Builder::BaseItemContext {
    public:
        ArrayItemContext Value(Node&& node);
        ArrayItemContext StartArray();
        DictItemContext StartDict();
        Builder& EndArray();
    };
  

}