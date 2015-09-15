
#ifndef _INCLUDE_LISTATE_H_
#define _INCLUDE_LISTATE_H_

/**
 * @file List.h
 * @brief STL�͑傫���̂ŁA�y��List�����O�ŗp�ӂ���B�����͐��`�̑o�������X�g�B
 */

namespace nro{

/**
 * nro::List �̃m�[�h
 * @brief �o�������X�g nro::List �̃m�[�h
 * @tparam T �v�f�̌^
 */
template <class T>
struct ListItem {
    T m_item; //!< �v�f
    ListItem<T>* m_prev; //!< �O�̃m�[�h�ւ̃|�C���^
    ListItem<T>* m_next; //!< ���̃m�[�h�ւ̃|�C���^
};

/**
 * �蔲��List�N���X
 * @brief �I�u�W�F�N�g�T�C�Y�����������邽�߂�STL�͎g��Ȃ��B
 *        ������ ListItem �ɂ��o�������X�g�B
 * @tparam T �v�f�̌^
 */
template <class T>
class List{
public:

    /**
     * nro::List �̓��o�̓C�e���[�^
     * @brief �蔲��List�N���X�̎蔲�����o�͑o�����C�e���[�^�B nro::List �݂̂��T�|�[�g����B
     * @tparam T �v�f�̌^
     */
    class Iterator{
        friend class List;

    public:

        /**
         * @brief NULL���w���C�e���[�^�̃R���X�g���N�^
         */
        Iterator(void){
            m_cur = 0;
        }

        /**
         * @brief �v�f���w���C�e���[�^�̃R���X�g���N�^
         * @param p �C�e���[�^�̏����Q�Ɛ�
         */
        Iterator(ListItem<T>* p){
            this->m_cur = p;
        }

        /**
         * @brief �C�e���[�^���w���v�f���擾����B���X�g�̏I�[���w���Ă���ꍇ�A�����܂�
         * @return �C�e���[�^���w���v�f
         */
        T& operator*(void) {
            return m_cur->m_item;
        }

        /**
         * @brief �C�e���[�^��1��֐i�߂�
         * @return 1��֐i�߂��������g
         */
        Iterator& operator++(void) {
            m_cur = m_cur->m_next;
            return *this;
        }

        /**
         * @brief �C�e���[�^��1�O�֖߂�
         * @return 1�O�֖߂����������g
         */
        Iterator& operator--(void) {
            m_cur = m_cur->m_prev;
            return *this;
        }

        /**
         * @brief �C�e���[�^���w���ʒu��ύX����
         * @param iter �C�e���[�^���w���ʒu��iter�֕ύX����
         * @return �ύX��̎������g
         */
        Iterator& operator=(const Iterator& iter) {
            m_cur = iter.m_cur;
            return *this;
        }

        /**
         * �C�e���[�^�̎w���ʒu������Ȃ�true��Ԃ�
         * @brief �C�e���[�^���w���Ă����̃A�h���X�Ŕ�r����
         * @param iter ��r����C�e���[�^
         * @retval true  �����v�f���w���Ă���
         * @retval false �قȂ�v�f���w���Ă���
         */
        bool operator==(const Iterator& iter){
            return m_cur == iter.m_cur;
        }

        /**
         * �C�e���[�^�̎w���ʒu���قȂ��Ă����true��Ԃ�
         * @brief �C�e���[�^���w���Ă����̃A�h���X�Ŕ�r����
         * @param iter ��r����C�e���[�^
         * @retval true  �قȂ�v�f���w���Ă���
         * @retval false �����v�f���w���Ă���
         */
        bool operator!=(const Iterator& iter){
            return m_cur != iter.m_cur;
        }

    private:
        ListItem<T>* m_cur; //!< �C�e���[�^���w���Ă���R���e�i�v�f�ւ̃|�C���^
    }; // end of class Iterator

public:

    /**
     * ��̃��X�g���쐬����R���X�g���N�^
     * @brief �擪�A�����Ƃ���NULL���w��
     */
    List(void){
        this->m_head.m_prev = 0;
        this->m_head.m_next = &m_tail;
        this->m_tail.m_prev = &m_head;
        this->m_tail.m_next = 0;
        this->m_count = 0;
    }

    /**
     * �f�X�g���N�^
     * @brief �R���e�i���̂��ׂĂ̗v�f��delete����
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
     * ���X�g�̐擪�ɗv�f��}������
     * @brief O(1)�ő}������
     * @param i �}������v�f�̒l
     */
    void PushFront(T i){
        ListItem<T>* item = new ListItem<T>();
        item->m_item = i;

        // �ǉ������A�C�e���̑O���ݒ�
        item->m_prev = &m_head;
        item->m_next = m_head.m_next;

        // �A�C�e���O�̎���ݒ�
        item->m_prev->m_next = item;

        // �A�C�e���̎��̑O��ݒ�
        item->m_next->m_prev = item;

        this->m_count++;
    }

    /**
     * ���X�g�̖����ɗv�f��}������
     * @brief O(1)�ő}������
     * @param i �}������v�f�̒l
     */
    void PushBack(T i){
        ListItem<T>* item = new ListItem<T>();
        item->m_item = i;

        // �ǉ������A�C�e���̑O���ݒ�
        item->m_prev = m_tail.m_prev;
        item->m_next = &m_tail;

        // �A�C�e���O�̎���ݒ�
        item->m_prev->m_next = item;

        // �A�C�e���̎��̑O��ݒ�
        item->m_next->m_prev = item;

        this->m_count++;
    }

    /**
     * ���X�g�̐擪�̗v�f�̒l��Ԃ�
     * @return ���X�g�̐擪�̗v�f�̒l
     */
    T Front(void){
        return m_head.m_next->m_item;
    }

    /**
     * ���X�g�̖����̗v�f�̒l��Ԃ�
     * @return ���X�g�̖����̗v�f�̒l
     */
    T Back(void){
        return m_tail.m_prev->m_item;
    }

    /**
     * ���X�g�̐擪�̗v�f���폜����
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
     * ���X�g�̖����̗v�f���폜����
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
     * �C�e���[�^���w���v�f�����X�g����폜����
     * @attention Remove�̃��V�[�o�ƈ�����iter�͓������X�g�̃C�e���[�^�łȂ���΂Ȃ�Ȃ�
     * @param iter �폜�������v�f���w���C�e���[�^
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
     * ���X�g�̗v�f�����ׂĎ�菜��
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
     * ���X�g�̐擪���w���C�e���[�^��Ԃ�
     * @return ���X�g�̐擪���w���C�e���[�^
     */
    Iterator Begin(void){
        return m_head.m_next;
    }

    /**
     * ���X�g�̖������w���C�e���[�^��Ԃ�
     * @attention ���X�g�̖����͏��NULL�ł�
     * @return ���X�g�̐擪���w���C�e���[�^
     */
    Iterator End(void){
        return &m_tail;
    }

    /**
     * ���X�g�̗v�f��
     * @return ���X�g�̗v�f
     */
    unsigned int Size(void) const{
        return m_count;
    }

    /**
     * ���X�g�̐擪���� item �ƍŏ��Ɉ�v�����ʒu���w���C�e���[�^��Ԃ�
     * @param item ��������l
     * @return �ŏ��Ɉ�v�����ʒu���w���C�e���[�^
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
    ListItem<T> m_head; //<! ���X�g�̐擪
    ListItem<T> m_tail; //<! ���X�g�̖���

    unsigned int m_count; //<! ���X�g�̗v�f��
}; // end of class List

} // end of namespace nro
#endif
