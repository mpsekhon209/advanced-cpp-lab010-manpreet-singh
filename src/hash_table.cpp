#include "project/hash_table.h"

#include <cmath>
#include <stdexcept>
#include <utility>

HashTable::HashTable(std::size_t capacity)
    : table_(capacity), size_(0) {
    if (capacity == 0) {
        throw std::invalid_argument("Hash table capacity must be greater than zero");
    }
    for (std::size_t i = 0; i < table_.size(); ++i) {
        table_[i] = HashEntry();
    }
}

HashTable::~HashTable() = default;

std::size_t HashTable::hashFunction(const std::string& key) const {
    std::size_t hash = 2166136261u;
    for (unsigned char ch : key) {
        hash ^= ch;
        hash *= 16777619u;
    }
    return hash % table_.size();
}

std::size_t HashTable::probe(const std::string& key) const {
    std::size_t index = hashFunction(key);
    std::size_t original = index;
    std::size_t step = 1;

    while (table_[index].occupied && table_[index].key != key) {
        index = (original + step * step) % table_.size();
        ++step;
    }
    return index;
}

bool HashTable::insert(const std::string& key, int value) {
    if ((size_ + 1) * 2 > table_.size()) {
        rehash();
    }

    const std::size_t original = hashFunction(key);
    std::size_t firstDeleted = table_.size();
    for (std::size_t step = 0; step < table_.size(); ++step) {
        const std::size_t index = (original + step * step) % table_.size();
        HashEntry& entry = table_[index];
        if (entry.occupied && !entry.deleted) {
            if (entry.key == key) {
                return false;
            }
        } else if (entry.deleted) {
            if (firstDeleted == table_.size()) {
                firstDeleted = index;
            }
        } else {
            const std::size_t target = firstDeleted == table_.size() ? index : firstDeleted;
            table_[target] = HashEntry(key, value, true, false);
            ++size_;
            return true;
        }
    }

    if (firstDeleted != table_.size()) {
        table_[firstDeleted] = HashEntry(key, value, true, false);
        ++size_;
        return true;
    }

    rehash();
    return insert(key, value);
}

bool HashTable::remove(const std::string& key) {
    const std::size_t original = hashFunction(key);
    for (std::size_t step = 0; step < table_.size(); ++step) {
        const std::size_t index = (original + step * step) % table_.size();
        HashEntry& entry = table_[index];
        if (!entry.occupied) {
            return false;
        }
        if (!entry.deleted && entry.key == key) {
            entry.deleted = true;
            --size_;
            return true;
        }
    }
    return false;
}

bool HashTable::contains(const std::string& key) const {
    const std::size_t original = hashFunction(key);
    for (std::size_t step = 0; step < table_.size(); ++step) {
        const HashEntry& entry = table_[(original + step * step) % table_.size()];
        if (!entry.occupied) {
            return false;
        }
        if (!entry.deleted && entry.key == key) {
            return true;
        }
    }
    return false;
}

int HashTable::get(const std::string& key) const {
    const std::size_t original = hashFunction(key);
    for (std::size_t step = 0; step < table_.size(); ++step) {
        const HashEntry& entry = table_[(original + step * step) % table_.size()];
        if (!entry.occupied) {
            break;
        }
        if (!entry.deleted && entry.key == key) {
            return entry.value;
        }
    }
    throw std::out_of_range("Key not found in hash table");
}

bool HashTable::update(const std::string& key, int newValue) {
    const std::size_t original = hashFunction(key);
    for (std::size_t step = 0; step < table_.size(); ++step) {
        HashEntry& entry = table_[(original + step * step) % table_.size()];
        if (!entry.occupied) {
            return false;
        }
        if (!entry.deleted && entry.key == key) {
            entry.value = newValue;
            return true;
        }
    }
    return false;
}

std::size_t HashTable::size() const noexcept {
    return size_;
}

std::size_t HashTable::capacity() const noexcept {
    return table_.size();
}

bool HashTable::empty() const noexcept {
    return size_ == 0;
}

double HashTable::loadFactor() const noexcept {
    return static_cast<double>(size_) / static_cast<double>(table_.size());
}

std::vector<std::string> HashTable::keys() const {
    std::vector<std::string> result;
    for (const auto& entry : table_) {
        if (entry.occupied && !entry.deleted) {
            result.push_back(entry.key);
        }
    }
    return result;
}

std::vector<int> HashTable::values() const {
    std::vector<int> result;
    for (const auto& entry : table_) {
        if (entry.occupied && !entry.deleted) {
            result.push_back(entry.value);
        }
    }
    return result;
}

void HashTable::rehash() {
    std::vector<HashEntry> oldTable = std::move(table_);
    table_.assign(nextPrime(oldTable.size() * 2), HashEntry());
    size_ = 0;

    for (const HashEntry& entry : oldTable) {
        if (entry.occupied && !entry.deleted) {
            insert(entry.key, entry.value);
        }
    }
}

bool HashTable::isPrime(std::size_t value) const {
    if (value < 2) {
        return false;
    }
    if (value == 2) {
        return true;
    }
    if (value % 2 == 0) {
        return false;
    }
    for (std::size_t i = 3; i * i <= value; i += 2) {
        if (value % i == 0) {
            return false;
        }
    }
    return true;
}

std::size_t HashTable::nextPrime(std::size_t value) const {
    if (value <= 2) {
        return 2;
    }
    std::size_t candidate = value % 2 == 0 ? value + 1 : value;
    while (!isPrime(candidate)) {
        candidate += 2;
    }
    return candidate;
}
