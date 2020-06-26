#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
private:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    T **arrays;
    size_t current_length;
    size_t storage_length;
    void doublearray(){
        if(current_length==storage_length-1){
            T **tmp=arrays;
            arrays=new T *[2*storage_length];
            for(size_t i=0;i<storage_length;++i)
                arrays[i]=tmp[i];
            delete [] tmp;
            for(size_t i=storage_length;i<2*storage_length;++i)
                arrays[i]=NULL;
            storage_length*=2;
        }
    }
public:
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
    friend class const_iterator;
    friend class vector<T>;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        T **pointer;
        int position;
        iterator(T **poi,int pos){pointer=poi;position=pos;}
    public:
        iterator(){pointer=NULL;position=0;}
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+(const int &n) const {
            //TODO
            if(pointer==NULL){
                iterator tmps;
                return tmps;
            }
            iterator tem(pointer,position+n);
            return tem;
        }
        iterator operator-(const int &n) const {
            //TODO
            if(pointer==NULL){
                iterator tmps;
                return tmps;
            }
            iterator tem(pointer,position-n);
            return tem;
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            //TODO
            if(pointer==NULL||rhs.pointer==NULL)   throw invalid_iterator();
            if(pointer!=rhs.pointer)               throw invalid_iterator();
            return position-rhs.position;
        }
        iterator& operator+=(const int &n) {
            //TODO
            if(pointer==NULL)  return *this;
            position+=n;
            return *this;
        }
        iterator& operator-=(const int &n) {
            //TODO
            if(pointer==NULL)  return *this;
            position-=n;
            return *this;
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            if(pointer==NULL){
                iterator tmps;
                return tmps;
            }
            iterator tmp=*this;
            ++position;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            if(pointer==NULL)  return *this;
            ++position;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            if(pointer==NULL){
                iterator tmps;
                return tmps;
            }
            iterator tmp=*this;
            --position;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            if(pointer==NULL)  return *this;
            --position;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return **(pointer+position);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {return (pointer==rhs.pointer&&position==rhs.position);}
        bool operator==(const const_iterator &rhs) const {return (pointer==rhs.pointer&&position==rhs.position);}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {return (pointer!=rhs.pointer||position!=rhs.position);}
        bool operator!=(const const_iterator &rhs) const {return (pointer!=rhs.pointer||position!=rhs.position);}
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    friend class iterator;
    friend class vector<T>;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        T *const*pointer;
        int position;
        const_iterator(T *const*poi,int pos){pointer=poi;position=pos;}
    public:
        const_iterator(){pointer=NULL;position=0;}
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator operator+(const int &n) const {
            //TODO
            if(pointer==NULL){
                const_iterator tmps;
                return tmps;
            }
            const_iterator tem(pointer,position+n);
            return tem;
        }
        const_iterator operator-(const int &n) const {
            //TODO
            if(pointer==NULL){
                const_iterator tmps;
                return tmps;
            }
            const_iterator tem(pointer,position-n);
            return tem;
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            //TODO
            if(pointer==NULL||rhs.pointer==NULL)  throw invalid_iterator();
            if(pointer!=rhs.pointer)              throw invalid_iterator();
            return position-rhs.position;
        }
        const_iterator& operator+=(const int &n) {
            //TODO
            if(pointer==NULL)  return *this;
            position+=n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            //TODO
            if(pointer==NULL)  return *this;
            position-=n;
            return *this;
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            if(pointer==NULL){
                const_iterator tmps;
                return tmps;
            }
            const_iterator tmp=*this;
            ++position;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            if(pointer==NULL)  return *this;
            ++position;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            if(pointer==NULL){
                const_iterator tmps;
                return tmps;
            }
            const_iterator tmp=*this;
            --position;
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            if(pointer==NULL)  return *this;
            --position;
            return *this;
        }
        /**
         * TODO *it
         */
        const T& operator*() const{
            return **(pointer+position);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const const_iterator &rhs) const {return (pointer==rhs.pointer&&position==rhs.position);}
        bool operator==(const iterator &rhs) const {return (pointer==rhs.pointer&&position==rhs.position);}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const const_iterator &rhs) const {return (pointer!=rhs.pointer||position!=rhs.position);}
        bool operator!=(const iterator &rhs) const {return (pointer!=rhs.pointer||position!=rhs.position);}
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector() {
        arrays=new T *[10];
        storage_length=10;
        current_length=0;
        for(size_t i=0;i<storage_length;++i)
            arrays[i]=NULL;
    }
    vector(const vector &other) {
        arrays=new T *[other.storage_length];
        storage_length=other.storage_length;
        current_length=other.current_length;
        for(size_t i=0;i<current_length;++i)
            arrays[i]=new T (*(other.arrays[i]));
        for(size_t i=current_length;i<storage_length;++i)
            arrays[i]=NULL;
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        for(size_t i=0;i<storage_length;++i)
            if(arrays[i]!=NULL)  delete arrays[i];
        delete [] arrays;
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if(this==&other)  return *this;
        for(size_t i=0;i<storage_length;++i)
            if(arrays[i]!=NULL)  delete arrays[i];
        delete [] arrays;
        arrays=new T *[other.storage_length];
        storage_length=other.storage_length;
        current_length=other.current_length;
        for(size_t i=0;i<current_length;++i)
            arrays[i]=new T (*(other.arrays[i]));
        for(size_t i=current_length;i<storage_length;++i)
            arrays[i]=NULL;
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T & at(const size_t &pos) {
        if(pos<0||pos>=current_length)  throw index_out_of_bound();
        return *(arrays[pos]);
    }
    const T & at(const size_t &pos) const {
        if(pos<0||pos>=current_length)  throw index_out_of_bound();
        return *(arrays[pos]);
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T & operator[](const size_t &pos) {
        if(pos<0||pos>=current_length)  throw index_out_of_bound();
        return *(arrays[pos]);
    }
    const T & operator[](const size_t &pos) const {
        if(pos<0||pos>=current_length)  throw index_out_of_bound();
        return *(arrays[pos]);
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T & front() const {
        if(current_length==0)  throw container_is_empty();
        return *(arrays[0]);
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T & back() const {
        if(current_length==0)  throw container_is_empty();
        return *(arrays[current_length-1]);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator tmp(arrays,0);
        return tmp;
    }
    const_iterator cbegin() const {
        const_iterator tmp(arrays,0);
        return tmp;
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        iterator tmp(arrays,current_length);
        return tmp;
    }
    const_iterator cend() const {
        const_iterator tmp(arrays,current_length);
        return tmp;
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
        return current_length==0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return current_length;
    }
    /**
     * clears the contents
     */
    void clear() {
        for(size_t i=0;i<storage_length;++i){
            if(arrays[i]!=NULL){
                delete arrays[i];
                arrays[i]=NULL;
            }
        }
        current_length=0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        size_t index=pos-begin();
        if(index>current_length||index<0)  throw index_out_of_bound();
        for(size_t i=current_length;i>index;--i)
            arrays[i]=arrays[i-1];
        arrays[index]=new T (value);
        ++current_length;
        doublearray();
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if(ind>current_length||ind<0)  throw index_out_of_bound();
        iterator tmp(arrays,ind);
        for(size_t i=current_length;i>ind;--i)
            arrays[i]=arrays[i-1];
        arrays[ind]=new T (value);
        ++current_length;
        doublearray();
        return tmp;
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
        size_t index=pos-begin();
        if(index>=current_length||index<0)  throw index_out_of_bound();
        delete arrays[index];
        --current_length;
        for(size_t i=index;i<current_length;++i)
            arrays[i]=arrays[i+1];
        arrays[current_length]=NULL;
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if(ind>=current_length||ind<0)  throw index_out_of_bound();
        iterator tmp(arrays,ind);
        delete arrays[ind];
        --current_length;
        for(size_t i=ind;i<current_length;++i)
            arrays[i]=arrays[i+1];
        arrays[current_length]=NULL;
        return tmp;
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        arrays[current_length++]=new T (value);
        doublearray();
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if(current_length==0)  throw container_is_empty();
        delete arrays[--current_length];
        arrays[current_length]=NULL;
    }
};


}

#endif
