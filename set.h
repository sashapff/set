//
// Created by Alexandra Ivanova on 29/06/2019.
//
#include <memory>

#ifndef SET_SET_H
#define SET_SET_H

template<typename T>
class set;

template<typename T>
class node {
public:
    std::unique_ptr<node<T>> left, right;
    node<T> *prev;
    T value;

    node(node *left, node *right, node *parent, T value) :
            left(left), right(right), prev(parent), value(value) {}

    node(node const &other) :
            left(other.left), right(other.right), prev(other.prev), value(other.value) {}

};

template<typename T>
struct iterator {
    typedef T value_type;
    typedef T const &reference;
    typedef std::ptrdiff_t difference_type;
    typedef T const *pointer;
    typedef std::bidirectional_iterator_tag iterator_category;

    template<typename> friend
    class set;

    iterator() = default;

    iterator &operator++() {
        ptr = set<value_type>::next(ptr);
        return *this;
    }

    const iterator operator++(int) {
        iterator result(*this);
        ++*this;
        return result;
    }

    iterator &operator--() {
        if (!ptr) {
            ptr = set<value_type>::get_max(root_ptr);
        } else {
            ptr = set<value_type>::prev(ptr);
        }
        return *this;
    }

    const iterator operator--(int) {
        iterator result(*this);
        --*this;
        return result;
    }

    reference operator*() const {
        return ptr->value;
    }

    pointer operator->() const {
        return &ptr->value;
    }

    bool operator==(iterator const &other) const {
        return ptr == other.ptr && root_ptr == other.root_ptr;
    }

    bool operator!=(iterator const &other) const {
        return ptr != other.ptr || root_ptr != other.root_ptr;
    }

    node<value_type> *ptr = nullptr;
    node<value_type> *root_ptr = nullptr;

private:
    explicit iterator(node<value_type> *ptr, node<value_type> *const root) : ptr(ptr), root_ptr(root) {}
};

template<typename T>
class set {
public:
    typedef T value_type;
    typedef T &reference;
    typedef T *pointer;

    template<typename> friend
    struct::iterator;

    typedef ::iterator<T> iterator;
    using const_iterator = ::iterator<T>;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    set() noexcept : root(nullptr) {}

    set(set const &other) {
        if (root == other.root) {
            return;
        }
        for (auto x = other.begin(); x != other.end(); ++x) {
            insert(*x);
        }
    }

    set &operator=(set const &other) {
        set(other).swap(*this);
        return *this;
    }

    iterator begin() {
        return iterator(get_min(root.get()), root.get());
    }

    const_iterator begin() const {
        return const_iterator(get_min(root.get()), root.get());
    }

    iterator end() {
        return iterator(nullptr, root.get());
    }

    const_iterator end() const {
        return const_iterator(nullptr, root.get());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    std::pair<iterator, bool> insert(value_type const &value) {
        auto result = insert_private(value);
        return std::make_pair(iterator(result.first, root.get()), result.second);
    }

    iterator erase(const_iterator pos) {
        auto result = erase_private(pos.ptr);
        return iterator(result, root.get());
    }

    const_iterator find(value_type const &pos) const {
        return const_iterator(find_private(pos, root.get()), root.get());
    }
    const_iterator lower_bound(value_type const& pos) const {
        return const_iterator(lb_private(pos, root.get()), root.get());
    }
    const_iterator upper_bound(value_type const& pos) const {
        return const_iterator(ub_private(pos, root.get()), root.get());
    }

    void clear() {
        root.reset(nullptr);
    }

    bool empty() {
        return !root.get();
    }

private:
    typedef std::unique_ptr<node<value_type>> uniq_n;

    uniq_n root;

    std::pair<node<value_type> *, bool> insert_private(value_type const &value) {
        if (!root.get()) {
            root.reset(new node<value_type>(nullptr, nullptr, nullptr, value));
            return std::make_pair(root.get(), true);
        }
        if (root.get()->value == value) {
            return std::make_pair(root.get(), false);
        }
        node<value_type> *current = root.get();
        while (true) {
            if (current->value == value) {
                return std::make_pair(current, false);
            } else if (current->value > value) {
                if (current->left) {
                    current = current->left.get();
                } else {
                    current->left.reset(new node<value_type>(nullptr, nullptr, current, value));
                    return std::make_pair(current->left.get(), true);
                }
            } else {
                if (current->right) {
                    current = current->right.get();
                } else {
                    current->right.reset(new node<value_type>(nullptr, nullptr, current, value));
                    return std::make_pair(current->right.get(), true);
                }
            }
        }
    }

    node<value_type> *erase_private(node<value_type> *n) {
        if (!n) return nullptr;
        auto rightmin = get_min(n->right.get());
        if (rightmin) {
            node<value_type> *buff = rightmin;
            if (buff->prev->left.get() == buff) {
                buff = buff->prev->left.release();
                buff->prev->left.reset(buff->right.release());
                if (buff->prev->left != nullptr) {
                    buff->prev->left->prev = buff->prev;
                }
            } else {
                buff = buff->prev->right.release();
                buff->prev->right.reset(buff->right.release());
                if (buff->prev->right != nullptr) {
                    buff->prev->right->prev = buff->prev;
                }
            }
            buff = change_node(n, buff);
            return buff;
        }
        node<value_type> *buff = next(n);
        if (n->prev == nullptr) {
            root.reset(n->left.get());
        } else {
            if (n->prev->left.get() == n) {
                if (n->left == nullptr) {
                    n->prev->left.reset(nullptr);
                } else {
                    n->left->prev = n->prev;
                    n->prev->left.reset(n->left.release());
                }
            } else {
                if (n->left.get() == nullptr) {
                    n->prev->right = nullptr;
                } else {
                    n->left->prev = n->prev;
                    n->prev->right.reset(n->left.release());
                }
            }
        }
        return buff;
    }

    node<value_type> *change_node(node<value_type> *from, node<value_type> *to) {
        if (from->left != nullptr) {
            from->left->prev = to;
        }
        if (from->right != nullptr) {
            from->right->prev = to;
        }
        to->left.reset(from->left.release());
        to->right.reset(from->right.release());
        if (from->prev) {
            auto buff = from->prev;
            if (from->prev->left.get() == from) {
                from->prev->left.reset(to);
            } else {
                from->prev->right.reset(to);
            }
            to->prev = buff;
        } else {
            to->prev = nullptr;
            root.reset(to);
        }
        return to;
    }

    node<value_type> *find_private(value_type const &value, node<value_type> *n) const {
        if (!n) return nullptr;
        if (n->value == value) {
            return n;
        }
        if (n->value > value) {
            return find_private(value, n->left.get());
        }
        return find_private(value, n->right.get());
    }

    node<value_type> *lb_private(value_type const &value, node<value_type> *n) const {
        if (!n) return nullptr;
        if (n->value == value) {
            auto result = lb_private(value, n->left.get());
            if (result) return result;
            return n;
        }
        if (n->value > value) {
            auto result = lb_private(value, n->left.get());
            if (result) return result;
            return n;
        }
        return find_private(value, n->right.get());
    }

    node<value_type> *ub_private(value_type const &value, node<value_type> *n) const {
        if (!n) return nullptr;
        if (n->value == value) {
            return ub_private(value, n->right.get());
        }
        if (n->value > value) {
            auto result = ub_private(value, n->left.get());
            if (result) return result;
            return n->value > value ? n : nullptr;
        }
        auto result = ub_private(value, n->right.get());
        if (result) return result;
        return n->value > value ? n : nullptr;
    }

    static node<value_type> *get_min(node<value_type> *n) {
        if (!n) return nullptr;
        while (n->left) {
            n = n->left.get();
        }
        return n;
    }

    static node<value_type> *get_max(node<value_type> *n) {
        if (!n) return nullptr;
        while (n->right) {
            n = n->right.get();
        }
        return n;
    }

    static node<value_type> *next(node<value_type> *n) {
        if (n->right) {
            n = n->right.get();
            return get_min(n);
        }
        node<value_type> *last = n;
        n = n->prev;
        while (n && (n->right && n->right.get() == last)) {
            last = n;
            n = n->prev;
        }
        if (!n) {
            return nullptr;
        }
        return n;
    }

    static node<value_type> *prev(node<value_type> *n) {
        assert(n);
        if (n->left) {
            n = n->left.get();
            return get_max(n);
        }
        node<value_type> *last = n;
        n = n->prev;
        while (n && (n->left && n->left.get() == last)) {
            last = n;
            n = n->prev;
        }
        if (!n) {
            return nullptr;
        }
        return n;
    }

    void swap(set &other) {
        std::swap(root, other.root);
    }

    friend void swap(set<T> &a, set<T> &b) {
        a.swap(b);
    }

};


#endif //SET_SET_H
