
#ifndef _INCLUDE_LIST_H_
#define _INCLUDE_LIST_H_

//*******************************************************************
// [手抜きListクラス]
//  オブジェクトサイズを小さくするために、stlは使わない
//
//*******************************************************************

namespace jiro{

template <class T> class List;

template <class T>
struct ListItem {
    T item_;
    ListItem<T>* prev_;
    ListItem<T>* next_;
};


template <class T>
class List{
public:
    class Iterator{
        friend class List;
    public:
        Iterator(void){
            cur_ = 0;
        }

        Iterator(ListItem<T>* p){
            this->cur_ = p;
        }

        T& operator*(void) {
            return cur_->item_;
        }

        Iterator& operator++(void) {
            cur_ = cur_->next_;
            return *this;
        }
        Iterator& operator--(void) {
            cur_ = cur_->prev_;
            return *this;
        }

        Iterator& operator=(const Iterator& iter) {
            cur_ = iter.cur_;
            return * this;
        }

        bool operator==(const Iterator& iter){
            return cur_ == iter.cur_;
        }
        bool operator!=(const Iterator& iter){
            return cur_ != iter.cur_;
        }
    private:
        ListItem<T>* cur_;

    };



public:
    List(void){
        this->head_.prev_ = 0;
        this->head_.next_ = &tail_;
        this->tail_.prev_ = &head_;
        this->tail_.next_ = 0;

        this->count_ = 0;
    }

    ~List(void){
        ListItem<T>* p = head_.next_;
        while(p != &tail_){
            ListItem<T>* tp = p;
            p = p->next_;
            delete tp;
        }
        this->count_ = 0;
    }

    void PushFront(T i){
        ListItem<T>* item = new ListItem<T>();
        item->item_ = i;

        // 追加したアイテムの前後を設定
        item->prev_ = &head_;
        item->next_ = head_.next_;

        // アイテム前の次を設定
        item->prev_->next_ = item;

        // アイテムの次の前を設定
        item->next_->prev_ = item;

        this->count_++;

    }

    void PushBack(T i){
        ListItem<T>* item = new ListItem<T>();
        item->item_ = i;

        // 追加したアイテムの前後を設定
        item->prev_ = tail_.prev_;
        item->next_ = &tail_;

        // アイテム前の次を設定
        item->prev_->next_ = item;

        // アイテムの次の前を設定
        item->next_->prev_ = item;

        this->count_++;
    }

    T Front(void){
        return head_.next_->item_;
    }

    T Back(void){
        return tail_.prev_->item_;
    }


    void PopFront(void){
        if ( head_.next_ == &tail_ ){
            return;
        }

        ListItem<T>* tp;
        tp = head_.next_;
        head_.next_ = head_.next_->next_;
        head_.next_->prev_ = &head_;
        delete tp;
        this->count_--;
    }

    void PopBack(void){

        if ( tail_.prev_ == &head_ ){
            return;
        }

        ListItem<T>* tp;
        tp = tail_.prev_;
        tail_.prev_ = tail_.prev_->prev_;
        tail_.prev_->next_ = &tail_;
        delete tp;
        this->count_--;
    }


    void Remove(Iterator& iter){
        if ( (iter.cur_ == &head_) || (iter.cur_ == &tail_) ){
            return;
        }

        iter.cur_->prev_->next_ = iter.cur_->next_;
        iter.cur_->next_->prev_ = iter.cur_->prev_;
        delete iter.cur_;
        this->count_--;
    }

    void Clear(void){
        ListItem<T>* p = head_.next_;
        while(p != &tail_){
            ListItem<T>* tp = p;
            p = p->next_;
            delete tp;
        }
        this->count_ = 0;
    }

    Iterator Begin(void){
        return head_.next_;
    }

    Iterator End(void){
        return &tail_;
    }

    unsigned int Size(void){
        return count_;
    }

    Iterator Find(T item){
        ListItem<T>* p = head_.next_;
        while(p != &tail_){
            if ( p->item_ == item){
                break;
            }
            p = p->next_;
        }
        return p;
    }


private:
    ListItem<T> head_;
    ListItem<T> tail_;

    unsigned int count_;

};


}

#endif
