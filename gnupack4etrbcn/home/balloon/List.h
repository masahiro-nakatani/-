
#ifndef _INCLUDE_LISTATE_H_
#define _INCLUDE_LISTATE_H_

/**
 * @file List.h
 * @brief STLは大きいので、軽量Listを自前で用意する。実装は線形の双方向リスト。
 */

namespace nro{

/**
 * nro::List のノード
 * @brief 双方向リスト nro::List のノード
 * @tparam T 要素の型
 */
template <class T>
struct ListItem {
    T m_item; //!< 要素
    ListItem<T>* m_prev; //!< 前のノードへのポインタ
    ListItem<T>* m_next; //!< 後ろのノードへのポインタ
};

/**
 * 手抜きListクラス
 * @brief オブジェクトサイズを小さくするためにSTLは使わない。
 *        実装は ListItem による双方向リスト。
 * @tparam T 要素の型
 */
template <class T>
class List{
public:

    /**
     * nro::List の入出力イテレータ
     * @brief 手抜きListクラスの手抜き入出力双方向イテレータ。 nro::List のみをサポートする。
     * @tparam T 要素の型
     */
    class Iterator{
        friend class List;

    public:

        /**
         * @brief NULLを指すイテレータのコンストラクタ
         */
        Iterator(void){
            m_cur = 0;
        }

        /**
         * @brief 要素を指すイテレータのコンストラクタ
         * @param p イテレータの初期参照先
         */
        Iterator(ListItem<T>* p){
            this->m_cur = p;
        }

        /**
         * @brief イテレータが指す要素を取得する。リストの終端を指している場合、落ちます
         * @return イテレータが指す要素
         */
        T& operator*(void) {
            return m_cur->m_item;
        }

        /**
         * @brief イテレータを1つ先へ進める
         * @return 1つ先へ進めた自分自身
         */
        Iterator& operator++(void) {
            m_cur = m_cur->m_next;
            return *this;
        }

        /**
         * @brief イテレータを1つ前へ戻す
         * @return 1つ前へ戻した自分自身
         */
        Iterator& operator--(void) {
            m_cur = m_cur->m_prev;
            return *this;
        }

        /**
         * @brief イテレータが指す位置を変更する
         * @param iter イテレータが指す位置をiterへ変更する
         * @return 変更後の自分自身
         */
        Iterator& operator=(const Iterator& iter) {
            m_cur = iter.m_cur;
            return *this;
        }

        /**
         * イテレータの指す位置が同一ならtrueを返す
         * @brief イテレータが指している先のアドレスで比較する
         * @param iter 比較するイテレータ
         * @retval true  同じ要素を指している
         * @retval false 異なる要素を指している
         */
        bool operator==(const Iterator& iter){
            return m_cur == iter.m_cur;
        }

        /**
         * イテレータの指す位置が異なっていればtrueを返す
         * @brief イテレータが指している先のアドレスで比較する
         * @param iter 比較するイテレータ
         * @retval true  異なる要素を指している
         * @retval false 同じ要素を指している
         */
        bool operator!=(const Iterator& iter){
            return m_cur != iter.m_cur;
        }

    private:
        ListItem<T>* m_cur; //!< イテレータが指しているコンテナ要素へのポインタ
    }; // end of class Iterator

public:

    /**
     * 空のリストを作成するコンストラクタ
     * @brief 先頭、末尾ともにNULLを指す
     */
    List(void){
        this->m_head.m_prev = 0;
        this->m_head.m_next = &m_tail;
        this->m_tail.m_prev = &m_head;
        this->m_tail.m_next = 0;
        this->m_count = 0;
    }

    /**
     * デストラクタ
     * @brief コンテナ内のすべての要素をdeleteする
     */
    ~List(void){
        ListItem<T>* p = m_head.m_next;
        while(p != &m_tail){
            ListItem<T>* tp = p;
            p = p->m_next;
            delete tp;
        }
        this->m_count = 0;
    }

    /**
     * リストの先頭に要素を挿入する
     * @brief O(1)で挿入する
     * @param i 挿入する要素の値
     */
    void PushFront(T i){
        ListItem<T>* item = new ListItem<T>();
        item->m_item = i;

        // 追加したアイテムの前後を設定
        item->m_prev = &m_head;
        item->m_next = m_head.m_next;

        // アイテム前の次を設定
        item->m_prev->m_next = item;

        // アイテムの次の前を設定
        item->m_next->m_prev = item;

        this->m_count++;
    }

    /**
     * リストの末尾に要素を挿入する
     * @brief O(1)で挿入する
     * @param i 挿入する要素の値
     */
    void PushBack(T i){
        ListItem<T>* item = new ListItem<T>();
        item->m_item = i;

        // 追加したアイテムの前後を設定
        item->m_prev = m_tail.m_prev;
        item->m_next = &m_tail;

        // アイテム前の次を設定
        item->m_prev->m_next = item;

        // アイテムの次の前を設定
        item->m_next->m_prev = item;

        this->m_count++;
    }

    /**
     * リストの先頭の要素の値を返す
     * @return リストの先頭の要素の値
     */
    T Front(void){
        return m_head.m_next->m_item;
    }

    /**
     * リストの末尾の要素の値を返す
     * @return リストの末尾の要素の値
     */
    T Back(void){
        return m_tail.m_prev->m_item;
    }

    /**
     * リストの先頭の要素を削除する
     */
    void PopFront(void){
        if ( m_head.m_next == &m_tail ){
            return;
        }

        ListItem<T>* tp;
        tp = m_head.m_next;
        m_head.m_next = m_head.m_next->m_next;
        m_head.m_next->m_prev = &m_head;
        delete tp;
        this->m_count--;
    }

    /**
     * リストの末尾の要素を削除する
     */
    void PopBack(void){
        if ( m_tail.m_prev == &m_head ){
            return;
        }
        ListItem<T>* tp;
        tp = m_tail.m_prev;
        m_tail.m_prev = m_tail.m_prev->m_prev;
        m_tail.m_prev->m_next = &m_tail;
        delete tp;
        this->m_count--;
    }

    /**
     * イテレータが指す要素をリストから削除する
     * @attention Removeのレシーバと引数のiterは同じリストのイテレータでなければならない
     * @param iter 削除したい要素を指すイテレータ
     */
    void Remove(Iterator& iter){
        if ( (iter.m_cur == &m_head) || (iter.m_cur == &m_tail) ){
            return;
        }
        iter.m_cur->m_prev->m_next = iter.m_cur->m_next;
        iter.m_cur->m_next->m_prev = iter.m_cur->m_prev;
        delete iter.m_cur;
        this->m_count--;
    }

    /**
     * リストの要素をすべて取り除く
     */
    void Clear(void){
        ListItem<T>* p = m_head.m_next;
        while(p != &m_tail){
            ListItem<T>* tp = p;
            p = p->m_next;
            delete tp;
        }
        this->m_count = 0;
    }

    /**
     * リストの先頭を指すイテレータを返す
     * @return リストの先頭を指すイテレータ
     */
    Iterator Begin(void){
        return m_head.m_next;
    }

    /**
     * リストの末尾を指すイテレータを返す
     * @attention リストの末尾は常にNULLです
     * @return リストの先頭を指すイテレータ
     */
    Iterator End(void){
        return &m_tail;
    }

    /**
     * リストの要素数
     * @return リストの要素
     */
    unsigned int Size(void) const{
        return m_count;
    }

    /**
     * リストの先頭から item と最初に一致した位置を指すイテレータを返す
     * @param item 検索する値
     * @return 最初に一致した位置を指すイテレータ
     */
    Iterator Find(T item){
        ListItem<T>* p = m_head.m_next;
        while(p != &m_tail){
            if (p->m_item == item){
                break;
            }
            p = p->m_next;
        }
        return p;
    }

private:
    ListItem<T> m_head; //<! リストの先頭
    ListItem<T> m_tail; //<! リストの末尾

    unsigned int m_count; //<! リストの要素数
}; // end of class List

} // end of namespace nro
#endif
