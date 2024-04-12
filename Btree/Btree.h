#include <iostream>
#include <functional>
#include <list>
#include <optional>
#include <assert.h>
template <typename type, typename val, typename cmp = std::less<type>>
class Btree
{
private:
    int m;
    cmp comp;
    struct BtreeNode
    {
        type *Tkey;
        val *Tval;
        BtreeNode **child;
        BtreeNode *fa;
        bool leaf;
        int siz;
        BtreeNode(int m) : siz(0), fa(nullptr), leaf(false)
        {
            Tkey = static_cast<type *>(calloc(m, sizeof(type)));
            Tval = static_cast<val *>(calloc(m, sizeof(val)));
            child = new BtreeNode *[m];
        };
        ~BtreeNode()
        {
            free(Tkey);
            free(Tval);
            delete[] child;
        }
    };
    BtreeNode *root;
    int findIdx(BtreeNode *node, type _key)
    {
        int i = 0;
        for (i = 0; i < (node->siz); i++)
        {
            if (!comp(node->Tkey[i], _key))
                break;
        }
        return i;
    }
    std::optional<val> search(BtreeNode *node, type _key)
    {
        int i = findIdx(node, _key);
        if (i < node->siz && node->Tkey[i] == _key)
            return node->Tval[i];
        if (node->leaf)
            return std::nullopt;
        return search(node->child[i], _key);
    }
    void insertNotFull(BtreeNode *node, int i, type _key, val _val, BtreeNode *ch)
    {
        for (int j = node->siz; j > i; j--)
        {
            node->child[j] = node->child[j - 1];
            if (j == node->siz)
                continue;
            node->Tkey[j] = node->Tkey[j - 1];
            node->Tval[j] = node->Tval[j - 1];
        }
        node->child[i] = ch;
        node->Tkey[i] = _key;
        node->Tval[i] = _val;
        node->siz++;
    }
    void spilt(BtreeNode *node)
    {
        BtreeNode *newNode = new BtreeNode(m);
        newNode->leaf = node->leaf;
        int spilt_p = m / 2;
        std::copy(node->child + spilt_p + 1, node->child + node->siz + 1, newNode->child);
        std::copy(node->Tkey + spilt_p + 1, node->Tkey + node->siz, newNode->Tkey);
        std::copy(node->Tval + spilt_p + 1, node->Tval + node->siz, newNode->Tval);
        node->siz = spilt_p - 1;
        newNode->siz = m - spilt_p - 1;
        newNode->fa = node->fa;
        for (int i = 0; i < newNode->siz; i++)
        {
            newNode->child[i]->fa = newNode;
        }
        if (node->fa == NULL)
        {
            BtreeNode *newroot = new BtreeNode(m);
            newroot->child[0] = node;
            newroot->child[1] = node;
            newroot->siz = 1;
            newroot->Tkey[0] = node->Tkey[spilt_p];
            newroot->Tval[0] = node->Tval[spilt_p];
            root = newroot;
            node->fa = newroot;
            newNode->fa = newroot;
        }
        else
        {
            int i = findIdx(node->fa, node->Tkey[spilt_p]);
            BtreeNode *fa = node->fa;
            insertNotFull(fa, i, node->Tkey[spilt_p], node->Tval[spilt_p], newNode);
            // 实现非根节点分裂逻辑
            if (fa->siz == m)
                spilt(fa);
        }
    }
    // 将两个节点合并为一个节点
    void merge(BtreeNode *node, int i)
    {
        // 实现合并逻辑
        int siz = node->child[i]->siz;
        node->child[i]->Tkey[siz] = node->Tkey[i];
        node->child[i]->Tval[siz] = node->Tval[i];
        for (int j = i; j < node->siz - 1; j++)
        {
            node->Tkey[j] = node->Tkey[j + 1];
            node->Tval[j] = node->Tval[j + 1];
        }
        BtreeNode *f1 = node->child[i];
        BtreeNode *f2 = node->child[i + 1];
        f1->siz += 1;
        node->siz--;
        if (f1->siz + f2->siz < m)
        {
            std::copy(f2->child, f2->child + f2->siz + 1, f1->child + f1->siz + 1);
            std::copy(f2->Tkey, f2->Tkey + f2->siz, f1->Tkey + f1->siz);
            std::copy(f2->Tval, f2->Tval + f2->siz, f1->Tval + f1->siz);
            f1->siz += f2->siz;
        }
        if (node->siz == 0)
        {
            root = f1;
            delete node;
        }
    }
    int insert(BtreeNode *node, type _key, val _val)
    {
        int i = findIdx(node, _key);
        if (i < node->siz && node->Tkey[i] == _key)
        {
            node->Tval[i] = _val;
            return 1;
        }
        // 实现插入逻辑
        if (node->leaf)
        {
            insertNotFull(node, i, _key, _val, nullptr);
            if (node->siz == m)
                spilt(node);
            return 1; // 这里返回的是插入结果，可以根据实际逻辑修改
        }
        else
            return insert(node->child[i], _key, _val);
    }
    void remove(BtreeNode *node, type key)
    {
        // 实现移除逻辑
        int i = findIdx(node, key);
        if (i < node->siz && node->Tkey[i] == key)
        {
            if (node->leaf)
            {
                for (int j = i; j < node->siz - 1; j++)
                {
                    node->Tkey[j] = node->Tkey[j + 1];
                    node->Tval[j] = node->Tval[j + 1];
                }
                node->siz--;
                return;
            }
            else
            {
                int siz = node->child[i]->siz;
                node->Tkey[i] = node->child[i]->Tkey[siz - 1];
                node->Tval[i] = node->child[i]->Tval[siz - 1];
                key = node->Tkey[i];
            }
        }
        else if (node->leaf)
            return;
        remove(node->child[i], key);
        if (node->child[i]->siz < (m + 1) / 2 - 1)
        {
            if (i + 1 <= node->siz && node->child[i + 1]->siz == (m + 1) / 2 - 1)
            {
                merge(node, i);
                return;
            }
            if (i && node->child[i - 1]->siz == (m + 1) / 2 - 1)
            {
                merge(node, i - 1);
                return;
            }
            merge(node, i);
        }
    }
    void destroy(BtreeNode *node)
    {
        if (!node)
            return;
        for (int i = 0; i <= node->siz; i++)
        {
            destroy(node->child[i]);
        }
        delete node;
    }

public:
    // 初始化几阶的B树
    Btree(int m) : m(m)
    {
        root = new BtreeNode(m);
        root->leaf = true;
        comp = cmp();
    };
    ~Btree()
    {
        destroy(root);
    }
    std::optional<val> search(type key)
    { // 传入比较函数对象
        return search(root, key);
    }
    int insert(type _key, val _val)
    {
        return insert(root, _key, _val);
    }
    std::vector<std::pair<type, val>> traverse()
    {
        std::vector<std::pair<type, val>> p;
        traverse(root, p);
        return p;
    }
    void traverse(BtreeNode *node, std::vector<std::pair<type, val>> &p)
    {
        if (node == nullptr)
            return;
        for (int i = 0; i < node->siz; i++)
        {
            traverse(node->child[i], p);
            p.emplace_back(node->Tkey[i], node->Tval[i]);
        }
        traverse(node->child[node->siz], p);
    }
    void remove(type key)
    {
        remove(root, key);
    }
};
