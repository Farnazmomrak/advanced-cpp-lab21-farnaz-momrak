#pragma once

#include <stdexcept>

// Lab 21: Skip Lists; Probabilistic Search Structures
// Implement the functions below. Do not change the public API.

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(std::size_t maxLevel, double probability, unsigned int seed)
    : head_(nullptr), size_(0), maxLevel_(maxLevel), probability_(probability),
      generator_(seed), distribution_(0.0, 1.0) {
    // TODO: Validate arguments and create the sentinel head node.
    if (maxLevel_ == 0) {
        throw std::invalid_argument("maxLevel must be greater than 0");
    }
    if (probability_ <= 0.0 || probability_ >= 1.0) {
        throw std::invalid_argument("probability must be between 0 and 1");
    }
    head_ = new Node(Key(), Value(), maxLevel_);
}

template <typename Key, typename Value>
SkipList<Key, Value>::~SkipList() { clear(); }

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(SkipList&& other) noexcept
    : head_(other.head_), size_(other.size_), maxLevel_(other.maxLevel_),
      probability_(other.probability_), generator_(std::move(other.generator_)),
      distribution_(other.distribution_) {
    other.head_ = nullptr;
    other.size_ = 0;
}

template <typename Key, typename Value>
SkipList<Key, Value>& SkipList<Key, Value>::operator=(SkipList&& other) noexcept {
    if (this != &other) {
        clear();
        head_ = other.head_;
        size_ = other.size_;
        maxLevel_ = other.maxLevel_;
        probability_ = other.probability_;
        generator_ = std::move(other.generator_);
        distribution_ = other.distribution_;
        other.head_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::randomLevel() {
    // TODO: Start at level 0 and promote while distribution_() < probability_,
    // stopping at maxLevel_.
    std::size_t level = 0;
    while (distribution_(generator_) < probability_ && level < maxLevel_) {
        ++level;
    }
    return level;
}

template <typename Key, typename Value>
void SkipList<Key, Value>::clear() {
    // TODO: Delete every data node, then delete the sentinel.
    Node* current = head_;
    while (current) {
        Node* next = current->forward[0];
        delete current;
        current = next;
    }
    head_ = nullptr;
    size_ = 0;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key& key, const Value& value) {
    // TODO: Locate predecessors at every level. Update duplicate values without
    // increasing size_. Otherwise create a random-height node and splice it in.
    if (head_ == nullptr) {
        return false;
    }
    std::vector<Node*> update(levelCount(), nullptr);
        Node* current = head_;
        for (std::size_t i= levelCount(); i-- > 0;) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current && current->key == key) {
        current->value = value;
        return false;
    }
    std::size_t newLevel = randomLevel();
    Node* newNode = new Node(key, value, newLevel);
    for (std::size_t i = 0; i <=newLevel; ++i) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
    ++size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::erase(const Key& key) {
    // TODO: Locate predecessors, unlink the target from every applicable level,
    // delete it, decrement size_, and return true. Return false if absent.
    if (head_ == nullptr) {
        return false;
    }
    std::vector<Node*> update(levelCount(), nullptr);
    Node* current = head_;
    for (std::size_t i = levelCount(); i-- > 0;) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    Node* target = current->forward[0];
    if (target == nullptr || target->key != key) {
        return false;
    }
    for (std::size_t i = 0; i < target->forward.size(); ++i) {
        update[i]->forward[i] = target->forward[i];
    }
    delete target;
    --size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::contains(const Key& key) const { return find(key) != nullptr; }

template <typename Key, typename Value>
Value* SkipList<Key, Value>::find(const Key& key) {
    // TODO: Search from the highest level down; return the value pointer or nullptr.
    if (head_ == nullptr) {
        return nullptr;
    }
    Node* current = head_;
    for (std::size_t i = levelCount(); i-- > 0;) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current && current->key == key) {
        return &current->value;
    }
    return nullptr;
}

template <typename Key, typename Value>
const Value* SkipList<Key, Value>::find(const Key& key) const {
    // TODO: Const version of find().
    if (head_ == nullptr) {
        return nullptr;
    }
    Node* current = head_;
    for (std::size_t i = levelCount(); i-- > 0;) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current && current->key == key) {
        return &current->value;
    }
    return nullptr;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::size() const noexcept { return size_; }

template <typename Key, typename Value>
bool SkipList<Key, Value>::empty() const noexcept { return size_ == 0; }

template <typename Key, typename Value>
std::vector<std::pair<Key, Value>> SkipList<Key, Value>::items() const {
    // TODO: Traverse level 0 and return all entries in ascending key order.
    std::vector<std::pair<Key, Value>> result;
    Node* current = head_ ? head_->forward[0] : nullptr;
    while (current) {
        result.emplace_back(current->key, current->value);
        current = current->forward[0];
    }
    return result;
    
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::levelCount() const noexcept {
    return head_ ? head_->forward.size() : 0;
}
