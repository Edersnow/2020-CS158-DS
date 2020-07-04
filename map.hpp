/**
 * implementation method: treap
 * finished by Edersnow(github)
 * go to README.md to see more:)))
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less<Key>
> class map {
friend class iterator;
friend class const_iterator;

const static unsigned long a=233, b=23333;
const static unsigned long mod_num=1226959;

public:
    typedef pair<const Key, T> value_type;

private:
    Compare comparator;
    //___l<r : -1___
    //___l=r :  0___
    //___l>r :  1___
    inline int compare(const Key &left, const Key &right) const{
        if(comparator(left, right))  return -1;
        if(comparator(right, left))  return 1;
        return 0;
    }

    static unsigned long ModMul(unsigned long n1, unsigned long n2){
        unsigned long ans=0;
        while(n2){
            if(n2&1)  ans=(ans+n1)%mod_num;
            n1=(n1+n1)%mod_num;
            n2>>=1;
        }
        return ans;
    }
    static unsigned long random_number_generator(){
        static unsigned long rand_num=2333;
        rand_num=ModMul(rand_num, a)+b;
        return rand_num;
    }

    struct treapnode{
        unsigned long pri;
        value_type data;
        treapnode *parent;
        treapnode *left;
        treapnode *right;

        treapnode(value_type val, treapnode *par=nullptr, treapnode *lef=nullptr, treapnode *rig=nullptr):
            data(val),
            parent(par),
            left(lef),
            right(rig)
            {pri=random_number_generator();}

        treapnode(const treapnode &other):
            data(other.data)
            {pri=other.pri;}
    };
    treapnode *root;
    size_t current_size;

    void copy_tree(treapnode *&cur, treapnode *fat, const treapnode *other){
        if(other==nullptr){
            cur=nullptr;
            return;
        }
        cur=new treapnode (*other);
        cur->parent=fat;
        copy_tree(cur->left, cur, other->left);
        copy_tree(cur->right, cur, other->right);
    }

    void clear_tree(treapnode *&cur){
        if(cur->left)  clear_tree(cur->left);
        if(cur->right)  clear_tree(cur->right);
        delete cur;
    }

    treapnode* find_node(treapnode *cur, const Key &key) const{
        if(cur==nullptr)  return cur;
        switch (compare(cur->data.first, key)){
            case 0:  return cur;
            case 1:  return find_node(cur->left, key);
        }
        return find_node(cur->right, key);
    }

    treapnode*& find_insertion(treapnode *&cur, const Key &key, treapnode *fat, treapnode *&fat_rec){
        if(cur==nullptr){
            fat_rec=fat;
            return cur;
        }
        switch (compare(cur->data.first, key)){
            case 0:  return cur;
            case 1:  return find_insertion(cur->left, key, cur, fat_rec);
        }
        return find_insertion(cur->right, key, cur, fat_rec);
    }

    treapnode*& find_deletion(treapnode *&cur, const Key &key){
        if(cur==nullptr)  return cur;
        switch (compare(cur->data.first, key)){
            case 0:  return cur;
            case 1:  return find_deletion(cur->left, key);
        }
        return find_deletion(cur->right, key);
    }

    inline void left_rotation(treapnode *&cur){
        treapnode *curnode=cur;
        treapnode *rignode=cur->right;
        cur=rignode;
        cur->parent=curnode->parent;
        curnode->right=cur->left;
        cur->left=curnode;
        curnode->parent=cur;
        if(curnode->right)  curnode->right->parent=curnode;
    }

    inline void right_rotation(treapnode *&cur){
        treapnode *curnode=cur;
        treapnode *lefnode=cur->left;
        cur=lefnode;
        cur->parent=curnode->parent;
        curnode->left=cur->right;
        cur->right=curnode;
        curnode->parent=cur;
        if(curnode->left)  curnode->left->parent=curnode;
    }

    treapnode *insert_adjust(treapnode *cur){
        treapnode *tmppar=cur->parent;
        if(tmppar && cur->pri < tmppar->pri){
            if(tmppar->parent && tmppar->parent->right){
                treapnode *&tmp=tmppar->parent->right == tmppar ? tmppar->parent->right : tmppar->parent->left;
                if(tmp->left == cur)  right_rotation(tmp);
                else  left_rotation(tmp);
                return insert_adjust(tmp);
            }
            else if(tmppar->parent){
                treapnode *&tmp=tmppar->parent->left;
                if(tmp->left == cur)  right_rotation(tmp);
                else  left_rotation(tmp);
                return insert_adjust(tmp);
            }
            else{
                treapnode *&tmp=root;
                if(tmp->left == cur)  right_rotation(tmp);
                else  left_rotation(tmp);
                return insert_adjust(tmp);
            }
        }
        return cur;
    }

    treapnode *&delete_adjust(treapnode *&cur){
        if(cur->left && cur->right){
            if(cur->left->pri < cur->right->pri){
                right_rotation(cur);
                return delete_adjust(cur->right);
            }
            else{
                left_rotation(cur);
                return delete_adjust(cur->left);
            }
        }
        return cur;        
    }

public:
    class const_iterator;
    class iterator {
    friend class map;
    friend class const_iterator;

    private:
        treapnode *curnode;
        map *curmap;

        iterator(treapnode *other, map *cur) {curnode=other, curmap=cur;}

    public:
        iterator() {
            curnode=nullptr;
            curmap=nullptr;
        }
        iterator(const iterator &other) {
            curnode=other.curnode;
            curmap=other.curmap;
        }

        iterator operator++(int) {
            iterator tmp=*this;
            ++(*this);
            return tmp;
        }
        iterator & operator++() {
            if(curmap==nullptr || curnode==nullptr)  throw invalid_iterator();
            if(curnode->right){
                curnode=curnode->right;
                while(curnode->left)  curnode=curnode->left;
            }
            else{
                treapnode *tmp=curnode;
                while(tmp->parent && tmp->parent->right == tmp)  tmp=tmp->parent;
                if(tmp->parent==nullptr)  curnode=nullptr;
                else  curnode=tmp->parent;
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp=*this;
            --(*this);
            return tmp;
        }
        iterator & operator--() {
            if(curmap==nullptr || curmap->root==nullptr)  throw invalid_iterator();
            if(curnode==nullptr){
                curnode=curmap->root;
                while(curnode->right)  curnode=curnode->right;
            }
            else if(curnode->left){
                curnode=curnode->left;
                while(curnode->right)  curnode=curnode->right;
            }
            else{
                treapnode *tmp=curnode;
                while(tmp->parent && tmp->parent->left == tmp)  tmp=tmp->parent;
                if(tmp->parent==nullptr)  throw invalid_iterator();
                curnode=tmp->parent;
            }
            return *this;
        }

        value_type & operator*() const {if(curmap==nullptr)  throw invalid_iterator();  return curnode->data;}
        bool operator==(const iterator &rhs) const {return curmap==rhs.curmap && curnode==rhs.curnode;}
        bool operator==(const const_iterator &rhs) const {return curmap==rhs.curmap && curnode==rhs.curnode;}
        bool operator!=(const iterator &rhs) const {return curmap!=rhs.curmap || curnode!=rhs.curnode;}
        bool operator!=(const const_iterator &rhs) const {return curmap!=rhs.curmap || curnode!=rhs.curnode;}
        value_type* operator->() const noexcept {return &(curnode->data);}
    };
    class const_iterator {
    friend class map;
    friend class iterator;
    private:
        const treapnode *curnode;
        const map *curmap;

        const_iterator(const treapnode *other, const map *cur) {curnode=other, curmap=cur;}

    public:
        const_iterator() {
            curnode=nullptr;
            curmap=nullptr;
        }
        const_iterator(const const_iterator &other) {
            curnode=other.curnode;
            curmap=other.curmap;
        }
        const_iterator(const iterator &other) {
            curnode=other.curnode;
            curmap=other.curmap;
        }

        const_iterator operator++(int) {
            const_iterator tmp=*this;
            ++(*this);
            return tmp;
        }
        const_iterator & operator++() {
            if(curmap==nullptr || curnode==nullptr)  throw invalid_iterator();
            if(curnode->right){
                curnode=curnode->right;
                while(curnode->left)  curnode=curnode->left;
            }
            else{
                const treapnode *tmp=curnode;
                while(tmp->parent && tmp->parent->right == tmp)  tmp=tmp->parent;
                if(tmp->parent==nullptr)  curnode=nullptr;
                else  curnode=tmp->parent;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp=*this;
            --(*this);
            return tmp;
        }
        const_iterator & operator--() {
            if(curmap==nullptr || curmap->root==nullptr)  throw invalid_iterator();
            if(curnode==nullptr){
                curnode=curmap->root;
                while(curnode->right)  curnode=curnode->right;
            }
            else if(curnode->left){
                curnode=curnode->left;
                while(curnode->right)  curnode=curnode->right;
            }
            else{
                const treapnode *tmp=curnode;
                while(tmp->parent && tmp->parent->left == tmp)  tmp=tmp->parent;
                if(tmp->parent==nullptr)  throw invalid_iterator();
                curnode=tmp->parent;
            }
            return *this;
        }

        const value_type & operator*() const {if(curmap==nullptr)  throw invalid_iterator();  return curnode->data;}
        bool operator==(const iterator &rhs) const {return curmap==rhs.curmap && curnode==rhs.curnode;}
        bool operator==(const const_iterator &rhs) const {return curmap==rhs.curmap && curnode==rhs.curnode;}
        bool operator!=(const iterator &rhs) const {return curmap!=rhs.curmap || curnode!=rhs.curnode;}
        bool operator!=(const const_iterator &rhs) const {return curmap!=rhs.curmap || curnode!=rhs.curnode;}
        const value_type* operator->() const noexcept {return &(curnode->data);}
    };

    map() {root=nullptr, current_size=0;}
    map(const map &other) {copy_tree(root, nullptr, other.root); current_size=other.current_size;}

    map & operator=(const map &other) {
        if(this==&other)  return *this;
        clear();
        copy_tree(root, nullptr, other.root);
        current_size=other.current_size;
        return *this;
    }

    ~map() {if(root)  clear_tree(root);  root=nullptr;  current_size=0;}

    T & at(const Key &key) {
        treapnode *tmp=find_node(root, key);
        if(tmp==nullptr)  throw index_out_of_bound();
        return tmp->data.second;
    }
    const T & at(const Key &key) const {
        const treapnode *tmp=find_node(root, key);
        if(tmp==nullptr)  throw index_out_of_bound();
        return tmp->data.second;
    }

    T & operator[](const Key &key) {
        treapnode *fatrec;
        treapnode *&tmp=find_insertion(root, key, nullptr, fatrec);
        if(tmp==nullptr){
            treapnode *res;
            tmp=new treapnode (value_type(key, T()), fatrec);
            res=insert_adjust(tmp);
            ++current_size;
            return res->data.second;
        }
        return tmp->data.second;
    }
    const T & operator[](const Key &key) const {
        const treapnode *tmp=find_node(root, key);
        if(tmp==nullptr)  throw index_out_of_bound();
        return tmp->data.second;
    }

    iterator begin() {
        iterator tmp(root, this);
        if(root)  while(tmp.curnode->left)  tmp.curnode=tmp.curnode->left;
        return tmp;
    }
    const_iterator cbegin() const {
        const_iterator tmp(root, this);
        if(root)  while(tmp.curnode->left)  tmp.curnode=tmp.curnode->left;
        return tmp;
    }

    iterator end() {
        return iterator(nullptr, this);
    }
    const_iterator cend() const {
        return const_iterator(nullptr, this);
    }

    bool empty() const {return root==nullptr;}
    size_t size() const {return current_size;}
    void clear() {if(root)  clear_tree(root);  root=nullptr;  current_size=0;}

    pair<iterator, bool> insert(const value_type &value) {
        pair<iterator, bool> res;
        treapnode *fatrec;
        treapnode *&tmp=find_insertion(root, value.first, nullptr, fatrec);
        if(tmp==nullptr){
            tmp=new treapnode (value, fatrec);
            res.first.curnode=insert_adjust(tmp);
            res.first.curmap=this;
            ++current_size;
            res.second=true;
        }
        else{
            res.second=false;
            res.first.curnode=tmp;
            res.first.curmap=this;
        }
        return res;
    }

    //erase function won't change other nodes' location (except the deleted node)
    //so it won't cause iterator failure
    void erase(iterator pos) {
        if(pos.curmap!=this || pos.curnode==nullptr)  throw index_out_of_bound();
        treapnode *&tmp=find_deletion(root, pos.curnode->data.first);
        treapnode *&adj=delete_adjust(tmp);
        if(adj->right==nullptr){
            treapnode *tod=adj;
            adj=adj->left;
            if(adj)  adj->parent=tod->parent;
            delete tod;
        }
        else{
            treapnode *tod=adj;
            adj=adj->right;
            if(adj)  adj->parent=tod->parent;
            delete tod;
        }
        --current_size;
    }

    size_t count(const Key &key) const {if(find_node(root, key))  return 1;  return 0;}

    iterator find(const Key &key) {
        iterator res(find_node(root, key), this);
        return res;
    }
    const_iterator find(const Key &key) const {
        const_iterator res(find_node(root, key), this);
        return res;
    }
};
}

#endif
