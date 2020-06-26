#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>

namespace sjtu { 

template<class T>
class deque {
public:
    class iterator;
    class const_iterator;

friend class iterator;
friend class const_iterator;

private:
    /**node:
     * save datas
     * each contain a prevnode pointer and a nextnode pointer
     * the datas must be given when a node is constructed(or using malloc())
    **/
    struct DequeNode{
        T nodedata;
        DequeNode *nextnode;
        DequeNode *prevnode;

        DequeNode(const T &data,DequeNode *next=nullptr,DequeNode *prev=nullptr):nodedata(data){
            nextnode=next;
            prevnode=prev;
        }
    };
    /**block:
     * save a list of nodes
     * contain a headnode and a tailnode, which are constructed by malloc()
     * contain a prevblock pointer and a nextblock pointer
     * the blocksize is updated in erase() or insert()
     * provide two construction function:
     *   DequeBlock(DequeBlock *next=nullptr,DequeBlock *prev=nullptr)
     *   DequeBlock(const DequeBlock &other)
    **/
    struct DequeBlock{
        size_t blocksize;
        DequeBlock *nextblock;
        DequeBlock *prevblock;
        DequeNode *headnode;
        DequeNode *tailnode;

        DequeBlock(DequeBlock *next=nullptr,DequeBlock *prev=nullptr){
            blocksize=0;
            nextblock=next;
            prevblock=prev;
            headnode=(DequeNode*)malloc(sizeof(DequeNode));
            tailnode=(DequeNode*)malloc(sizeof(DequeNode));
            headnode->nextnode=tailnode;
            tailnode->prevnode=headnode;
        }
        DequeBlock(const DequeBlock &other){
            blocksize=other.blocksize;
            nextblock=nullptr;
            prevblock=nullptr;
            headnode=(DequeNode*)malloc(sizeof(DequeNode));
            tailnode=(DequeNode*)malloc(sizeof(DequeNode));
            DequeNode *tmp1=headnode;
            DequeNode *tmp2=other.headnode->nextnode;
            size_t i=blocksize;
            //checked
            while(i--){
                tmp1->nextnode=new DequeNode (tmp2->nodedata,nullptr,tmp1);
                tmp1=tmp1->nextnode;
                tmp2=tmp2->nextnode;
            }
            tmp1->nextnode=tailnode;
            tailnode->prevnode=tmp1;
        }
        ~DequeBlock(){
            DequeNode *p,*q;
            p=headnode->nextnode;
            while(p!=tailnode){
                q=p;
                p=p->nextnode;
                delete q;
            }
            free(headnode);
            free(tailnode);
        }
    };
    /**deque:
     * contain a headblock and a tailblock
     * contain the dequesize and the maxblocksize
    **/
    DequeBlock *headblock;
    DequeBlock *tailblock;
    size_t dequesize;
    size_t maxblocksize;

    void splitblock(iterator &pos){
        DequeNode *p=pos.firsnode,*q=pos.currnode,*r=pos.lastnode;
        DequeBlock *tmp1=pos.currblock,*tmp2=pos.currblock->nextblock,*tmp3;
        size_t alllenght=tmp1->blocksize;
        bool flag=true;
        for(size_t i=0;i<alllenght>>1;++i){
            p=p->nextnode;
            if(p==q)  flag=false;
        }
        tmp1->nextblock=new DequeBlock(tmp2,tmp1);
        tmp3=tmp1->nextblock,
        tmp2->prevblock=tmp3,
        tmp3->headnode->nextnode=p->nextnode,
        p->nextnode->prevnode=tmp3->headnode,
        tmp1->tailnode=tmp3->tailnode,
        tmp1->tailnode->prevnode=p,
        p->nextnode=tmp1->tailnode,
        tmp3->tailnode=r,
        tmp1->blocksize=alllenght>>1,
        tmp3->blocksize=alllenght-(alllenght>>1);
        if(flag){
            pos.currblock=tmp3;
            pos.firsnode=tmp3->headnode;
            pos.lastnode=tmp3->tailnode;
        }
        else  pos.lastnode=tmp1->tailnode;
    }
    void mergeblock(iterator &pos){
        DequeBlock *tmp1,*tmp2=pos.currblock;
        DequeNode *p,*q;
        tmp1=tmp2->nextblock;
        //merge currblock and the next one
        if(tmp1!=tailblock&&(tmp2->blocksize+tmp1->blocksize<maxblocksize||tmp2->blocksize<50)){
            tmp2->nextblock=tmp1->nextblock,
            tmp2->nextblock->prevblock=tmp2,
            p=tmp2->tailnode,
            q=p->prevnode,
            q->nextnode=tmp1->headnode->nextnode,
            q->nextnode->prevnode=q,
            tmp2->tailnode=tmp1->tailnode,
            tmp1->tailnode=p,
            tmp1->tailnode->prevnode=tmp1->headnode,
            tmp1->headnode->nextnode=tmp1->tailnode,
            tmp2->blocksize+=tmp1->blocksize;
            if(pos.currnode==p)  pos.currnode=q->nextnode;
            pos.lastnode=tmp2->tailnode;
            delete tmp1;
        }
        tmp1=tmp2->prevblock;
        if(tmp1!=headblock&&(tmp2->blocksize+tmp1->blocksize<maxblocksize||tmp2->blocksize<50)){
            tmp1->nextblock=tmp2->nextblock,
            tmp1->nextblock->prevblock=tmp1,
            p=tmp1->tailnode,
            q=p->prevnode,
            q->nextnode=tmp2->headnode->nextnode,
            q->nextnode->prevnode=q,
            tmp1->tailnode=tmp2->tailnode,
            tmp2->tailnode=p,
            tmp2->tailnode->prevnode=tmp2->headnode,
            tmp2->headnode->nextnode=tmp2->tailnode,
            tmp1->blocksize+=tmp2->blocksize;
            delete tmp2;
            if(pos.currnode==tmp1->tailnode&&tmp1->nextblock!=tailblock){
                pos.currblock=tmp1->nextblock,
                pos.firsnode=tmp1->nextblock->headnode,
                pos.lastnode=tmp1->nextblock->tailnode,
                pos.currnode=pos.firsnode->nextnode;
            }
            else{
                pos.currblock=tmp1,
                pos.firsnode=tmp1->headnode,
                pos.lastnode=tmp1->tailnode;
            }
        }
    }
    inline void updatesize() {
        if(90000<dequesize)  maxblocksize=sqrt(dequesize);
        else  maxblocksize=300;
    }

public:
    class iterator {
    friend class const_iterator;
    friend class deque<T>;

    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        deque<T> *currdeque;
        DequeBlock *currblock;
        DequeNode *currnode;
        DequeNode *firsnode;
        DequeNode *lastnode;

        size_t currindex() const{
            size_t tmp=0;
            DequeBlock *p=currdeque->headblock->nextblock;
            DequeNode *q=firsnode;
            while(p!=currblock){
                tmp+=p->blocksize,
                p=p->nextblock;
            }
            while(q!=currnode){
                ++tmp,
                q=q->nextnode;
            }
            return tmp;
        }
        iterator(deque<T> *deq,DequeBlock *blo,DequeNode *nod){
            currdeque=deq,
            currblock=blo,
            currnode=nod,
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode;
        }

    public:
        iterator(){currdeque=nullptr;}
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, the behaviour is **undefined**.
         * as well as operator-
         */
        iterator operator+(const int &n) const {
            //TODO
            iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            return tmp+=n;
        }
        iterator operator-(const int &n) const {
            //TODO
            iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            return tmp-=n;
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const iterator &rhs) const {
            //TODO
            if(currdeque==nullptr||rhs.currdeque==nullptr)  throw invalid_iterator();
            if(currdeque!=rhs.currdeque)  throw invalid_iterator();
            size_t tmp1=currindex();
            size_t tmp2=rhs.currindex();
            return tmp1-tmp2;
        }
        iterator& operator+=(const int &n) {
            //TODO
            if(currdeque==nullptr)  return *this;
            if(n==0)  return *this;
            if(n<0)   return *this-=(-n);
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            size_t cnt=0;
            iterator def=*this;
            while(cnt<n&&currnode->nextnode!=lastnode)  ++cnt,currnode=currnode->nextnode;
            if(n==cnt)  return *this;
            while(cnt<n&&currblock->nextblock!=currdeque->tailblock)  currblock=currblock->nextblock,cnt+=currblock->blocksize;
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode,
            currnode=lastnode->prevnode;
            if(cnt==n-1)  ++cnt,currnode=currnode->nextnode;
            if(cnt<n){
                *this=def;
                throw index_out_of_bound();
            }
            while(cnt>n)  currnode=currnode->prevnode,--cnt;
            return *this;
        }
        iterator& operator-=(const int &n) {
            //TODO
            if(currdeque==nullptr)  return *this;
            if(n==0)  return *this;
            if(n<0)   return *this+=(-n);
            size_t cnt=0;
            iterator def=*this;
            while(cnt<n&&currnode->prevnode!=firsnode)  ++cnt,currnode=currnode->prevnode;
            if(n==cnt)  return *this;
            while(cnt<n&&currblock->prevblock!=currdeque->headblock)  currblock=currblock->prevblock,cnt+=currblock->blocksize;
            if(cnt<n){
                *this=def;
                throw index_out_of_bound();
            }
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode,
            currnode=firsnode->nextnode;
            while(cnt>n)  currnode=currnode->nextnode,--cnt;
            return *this;
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            if(currnode->nextnode==lastnode&&currblock->nextblock!=currdeque->tailblock){
                currblock=currblock->nextblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=firsnode->nextnode;
            }
            else  currnode=currnode->nextnode;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++() {
            if(currdeque==nullptr)  return *this;
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            if(currnode->nextnode==lastnode&&currblock->nextblock!=currdeque->tailblock){
                currblock=currblock->nextblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=firsnode->nextnode;
            }
            else  currnode=currnode->nextnode;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            if(currnode->prevnode==firsnode){
                if(currblock->prevblock==currdeque->headblock)  throw index_out_of_bound();
                currblock=currblock->prevblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=lastnode->prevnode;
            }
            else  currnode=currnode->prevnode;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            if(currdeque==nullptr)  return *this;
            if(currnode->prevnode==firsnode){
                if(currblock->prevblock==currdeque->headblock)  throw index_out_of_bound();
                currblock=currblock->prevblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=lastnode->prevnode;
            }
            else  currnode=currnode->prevnode;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const {
            if(currdeque==nullptr)  throw invalid_iterator();
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            return currnode->nodedata;
        }
        /**
         * TODO it->field
         */
        T* operator->() const noexcept {
            return &(currnode->nodedata);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {return currnode==rhs.currnode;}
        bool operator==(const const_iterator &rhs) const {return currnode==rhs.currnode;}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {return currnode!=rhs.currnode;}
        bool operator!=(const const_iterator &rhs) const {return currnode!=rhs.currnode;}
    };
    class const_iterator {
    friend class iterator;
    friend class deque<T>;
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
    private:
        // data members.
        const deque<T> *currdeque;
        const DequeBlock *currblock;
        const DequeNode *firsnode;
        const DequeNode *lastnode;
        const DequeNode *currnode;

        size_t currindex() const{
            size_t tmp=0;
            const DequeBlock *p=currdeque->headblock->nextblock;
            const DequeNode *q=firsnode;
            while(p!=currblock){
                tmp+=p->blocksize,
                p=p->nextblock;
            }
            while(q!=currnode){
                ++tmp;
                q=q->nextnode;
            }
            return tmp;
        }
        const_iterator(const deque<T> *deq,const DequeBlock *blo,const DequeNode *nod){
            currdeque=deq,
            currblock=blo,
            currnode=nod,
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode;
        }

    public:
        const_iterator() {
            // TODO
            currdeque=nullptr;
        }
        const_iterator(const const_iterator &other) {
            // TODO
            currdeque=other.currdeque,
            currblock=other.currblock,
            firsnode=other.firsnode,
            lastnode=other.lastnode,
            currnode=other.currnode;
        }
        const_iterator(const iterator &other) {
            // TODO
            currdeque=other.currdeque,
            currblock=other.currblock,
            firsnode=other.firsnode,
            lastnode=other.lastnode,
            currnode=other.currnode;
        }
        // And other methods in iterator.
        // And other methods in iterator.
        // And other methods in iterator.
        const_iterator operator+(const int &n) const {
            //TODO
            const_iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            return tmp+=n;
        }
        const_iterator operator-(const int &n) const {
            //TODO
            const_iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            return tmp-=n;
        }
        // return th distance between two iterator,
        // if these two iterators points to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            //TODO
            if(currdeque==nullptr||rhs.currdeque==nullptr)  throw invalid_iterator();
            if(currdeque!=rhs.currdeque)  throw invalid_iterator();
            size_t tmp1=currindex();
            size_t tmp2=rhs.currindex();
            return tmp1-tmp2;
        }
        const_iterator& operator+=(const int &n) {
            //TODO
            if(currdeque==nullptr)  return *this;
            if(n==0)  return *this;
            if(n<0)   return *this-=(-n);
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            size_t cnt=0;
            const_iterator def=*this;
            while(cnt<n&&currnode->nextnode!=lastnode)  ++cnt,currnode=currnode->nextnode;
            if(n==cnt)  return *this;
            while(cnt<n&&currblock->nextblock!=currdeque->tailblock)  currblock=currblock->nextblock,cnt+=currblock->blocksize;
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode,
            currnode=lastnode->prevnode;
            if(cnt==n-1)  ++cnt,currnode=currnode->nextnode;
            if(cnt<n){
                *this=def;
                throw index_out_of_bound();
            }
            while(cnt>n)  currnode=currnode->prevnode,--cnt;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            //TODO
            if(currdeque==nullptr)  return *this;
            if(n==0)  return *this;
            if(n<0)   return *this+=(-n);
            size_t cnt=0;
            const_iterator def=*this;
            while(cnt<n&&currnode->prevnode!=firsnode)  ++cnt,currnode=currnode->prevnode;
            if(n==cnt)  return *this;
            while(cnt<n&&currblock->prevblock!=currdeque->headblock)  currblock=currblock->prevblock,cnt+=currblock->blocksize;
            if(cnt<n){
                *this=def;
                throw index_out_of_bound();
            }
            firsnode=currblock->headnode,
            lastnode=currblock->tailnode,
            currnode=firsnode->nextnode;
            while(cnt>n)  currnode=currnode->nextnode,--cnt;
            return *this;
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            if(currnode->nextnode==lastnode&&currblock->nextblock!=currdeque->tailblock){
                currblock=currblock->nextblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=firsnode->nextnode;
            }
            else  currnode=currnode->nextnode;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            if(currdeque==nullptr)  return *this;
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            if(currnode->nextnode==lastnode&&currblock->nextblock!=currdeque->tailblock){
                currblock=currblock->nextblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=firsnode->nextnode;
            }
            else  currnode=currnode->nextnode;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator tmp=*this;
            if(currdeque==nullptr)  return tmp;
            if(currnode->prevnode==firsnode){
                if(currblock->prevblock==currdeque->headblock)  throw index_out_of_bound();
                currblock=currblock->prevblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=lastnode->prevnode;
            }
            else  currnode=currnode->prevnode;
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            if(currdeque==nullptr)  return *this;
            if(currnode->prevnode==firsnode){
                if(currblock->prevblock==currdeque->headblock)  throw index_out_of_bound();
                currblock=currblock->prevblock,
                firsnode=currblock->headnode,
                lastnode=currblock->tailnode,
                currnode=lastnode->prevnode;
            }
            else  currnode=currnode->prevnode;
            return *this;
        }
        /**
         * TODO *it
         */
        const T& operator*() const {
            if(currdeque==nullptr)  throw invalid_iterator();
            if(currnode==currdeque->tailblock->prevblock->tailnode)  throw index_out_of_bound();
            return currnode->nodedata;
        }
        /**
         * TODO it->field
         */
        const T* operator->() const noexcept {
            return &(currnode->nodedata);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {return currnode==rhs.currnode;}
        bool operator==(const const_iterator &rhs) const {return currnode==rhs.currnode;}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {return currnode!=rhs.currnode;}
        bool operator!=(const const_iterator &rhs) const {return currnode!=rhs.currnode;}
    };
    /**
     * TODO Constructors
     */
    //ensure that each block is not empty(except dequesize==0)
    deque() {
        headblock=new DequeBlock ();
        tailblock=new DequeBlock ();
        headblock->nextblock=new DequeBlock (tailblock,headblock);
        tailblock->prevblock=headblock->nextblock,
        dequesize=0,
        maxblocksize=300;
    }
    deque(const deque &other) {
        dequesize=other.dequesize,
        maxblocksize=other.maxblocksize;
        headblock=new DequeBlock ();
        tailblock=new DequeBlock ();
        DequeBlock *tmp1=headblock;
        DequeBlock *tmp2=other.headblock->nextblock;
        while(tmp2!=other.tailblock){
            tmp1->nextblock=new DequeBlock (*tmp2);
            tmp1->nextblock->prevblock=tmp1,
            tmp1=tmp1->nextblock,
            tmp2=tmp2->nextblock;
        }
        tmp1->nextblock=tailblock,
        tailblock->prevblock=tmp1;
    }
    /**
     * TODO Deconstructor
     */
    ~deque() {
        DequeBlock *p=headblock->nextblock,*q;
        while(p!=tailblock){
            q=p,
            p=p->nextblock;
            delete q;
        }
        delete headblock;
        delete tailblock;
    }
    /**
     * TODO assignment operator
     */
    deque &operator=(const deque &other) {
        if(this==&other)  return *this;
        DequeBlock *p=headblock->nextblock,*q;
        while(p!=tailblock){
            q=p,
            p=p->nextblock;
            delete q;
        }
        delete headblock;
        delete tailblock;
        dequesize=other.dequesize,
        maxblocksize=other.maxblocksize;
        headblock=new DequeBlock ();
        tailblock=new DequeBlock ();
        DequeBlock *tmp1=headblock;
        DequeBlock *tmp2=other.headblock->nextblock;
        while(tmp2!=other.tailblock){
            tmp1->nextblock=new DequeBlock (*tmp2);
            tmp1->nextblock->prevblock=tmp1,
            tmp1=tmp1->nextblock,
            tmp2=tmp2->nextblock;
        }
        tmp1->nextblock=tailblock,
        tailblock->prevblock=tmp1;
        return *this;
    }
    /**
     * access specified element with bounds checking
     * throw index_out_of_bound if out of bound.
     */
    T & at(const size_t &pos) {
        size_t cnt=0;
        DequeBlock *tmpb=headblock;
        DequeNode *tmpn;
        while(cnt<=pos&&tmpb->nextblock!=tailblock)  tmpb=tmpb->nextblock,cnt+=tmpb->blocksize;
        if(cnt<=pos)  throw index_out_of_bound();
        tmpn=tmpb->tailnode->prevnode;
        while(cnt>pos+1)  tmpn=tmpn->prevnode,--cnt;
        return tmpn->nodedata;
    }
    const T & at(const size_t &pos) const {
        size_t cnt=0;
        const DequeBlock *tmpb=headblock;
        const DequeNode *tmpn;
        while(cnt<=pos&&tmpb->nextblock!=tailblock)  tmpb=tmpb->nextblock,cnt+=tmpb->blocksize;
        if(cnt<=pos)  throw index_out_of_bound();
        tmpn=tmpb->tailnode->prevnode;
        while(cnt>pos+1)  tmpn=tmpn->prevnode,--cnt;
        return tmpn->nodedata;
    }
    T & operator[](const size_t &pos) {
        size_t cnt=0;
        DequeBlock *tmpb=headblock;
        DequeNode *tmpn;
        while(cnt<=pos&&tmpb->nextblock!=tailblock)  tmpb=tmpb->nextblock,cnt+=tmpb->blocksize;
        if(cnt<=pos)  throw index_out_of_bound();
        tmpn=tmpb->tailnode->prevnode;
        while(cnt>pos+1)  tmpn=tmpn->prevnode,--cnt;
        return tmpn->nodedata;
    }
    const T & operator[](const size_t &pos) const {
        size_t cnt=0;
        const DequeBlock *tmpb=headblock;
        const DequeNode *tmpn;
        while(cnt<=pos&&tmpb->nextblock!=tailblock)  tmpb=tmpb->nextblock,cnt+=tmpb->blocksize;
        if(cnt<=pos)  throw index_out_of_bound();
        tmpn=tmpb->tailnode->prevnode;
        while(cnt>pos+1)  tmpn=tmpn->prevnode,--cnt;
        return tmpn->nodedata;
    }
    /**
     * access the first element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if(dequesize==0)  throw container_is_empty();
        return headblock->nextblock->headnode->nextnode->nodedata;
    }
    /**
     * access the last element
     * throw container_is_empty when the container is empty.
     */
    const T & back() const {
        if(dequesize==0)  throw container_is_empty();
        return tailblock->prevblock->tailnode->prevnode->nodedata;
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        iterator tmp(this,headblock->nextblock,headblock->nextblock->headnode->nextnode);
        return tmp;
    }
    const_iterator cbegin() const {
        const_iterator tmp(this,headblock->nextblock,headblock->nextblock->headnode->nextnode);
        return tmp;
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        iterator tmp(this,tailblock->prevblock,tailblock->prevblock->tailnode);
        return tmp;
    }
    const_iterator cend() const {
        const_iterator tmp(this,tailblock->prevblock,tailblock->prevblock->tailnode);
        return tmp;
    }
    /**
     * checks whether the container is empty.
     */
    bool empty() const {
        return dequesize==0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return dequesize;
    }
    /**
     * clears the contents
     */
    void clear() {
        DequeBlock *p=headblock->nextblock,*q;
        while(p!=tailblock){
            q=p,
            p=p->nextblock;
            delete q;
        }
        delete headblock;
        delete tailblock;
        headblock=new DequeBlock ();
        tailblock=new DequeBlock ();
        headblock->nextblock=new DequeBlock (tailblock,headblock);
        tailblock->prevblock=headblock->nextblock,
        dequesize=0,
        maxblocksize=300;
        //checked
    }
    /**
     * inserts elements at the specified locat on in the container.
     * inserts value before pos
     * returns an iterator pointing to the inserted value
     *     throw if the iterator is invalid or it point to a wrong place.
     */
    iterator insert(iterator pos, const T &value) {
        if(pos.currdeque!=this)  throw invalid_iterator();
        DequeNode *tmp=pos.currnode->prevnode;
        tmp->nextnode=new DequeNode (value,pos.currnode,tmp);
        pos.currnode->prevnode=tmp->nextnode,
        pos.currnode=tmp->nextnode,
        ++pos.currblock->blocksize,
        ++dequesize;
        updatesize();
        if(pos.currblock->blocksize>2*maxblocksize)  splitblock(pos);
        return pos;
    }
    /**
     * removes specified element at pos.
     * removes the element at pos.
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid or it points to a wrong place.
     */
    iterator erase(iterator pos) {
        if(pos.currdeque!=this)  throw invalid_iterator();
        if(pos.currnode==tailblock->prevblock->tailnode)  throw index_out_of_bound();
        DequeNode *tmp=pos.currnode;
        pos.currnode=pos.currnode->nextnode,
        pos.currnode->prevnode=tmp->prevnode,
        tmp->prevnode->nextnode=pos.currnode;
        //linked
        delete tmp;
        --pos.currblock->blocksize,
        --dequesize;
        updatesize();
        mergeblock(pos);
        if(pos.currnode==pos.lastnode&&pos.currblock->nextblock!=tailblock){
            pos.currblock=pos.currblock->nextblock,
            pos.firsnode=pos.currblock->headnode,
            pos.lastnode=pos.currblock->tailnode,
            pos.currnode=pos.firsnode->nextnode;
        }
        return pos;
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        insert(end(),value);
    }
    /**
     * removes the last element
     *     throw when the container is empty.
     */
    void pop_back() {
        if(dequesize==0)  throw container_is_empty();
        iterator p=end();
        --p;
        erase(p);
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        insert(begin(),value);
    }
    /**
     * removes the first element.
     *     throw when the container is empty.
     */
    void pop_front() {
        if(dequesize==0)  throw container_is_empty();
        erase(begin());
    }
};

}

#endif
