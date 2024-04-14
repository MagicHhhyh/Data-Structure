#include <iostream>
#include <functional>
#include <list>
#include <optional>
#include <unistd.h>
#include <memory>
#include <assert.h>

#define rb_RED 0
#define rb_BLACK 1
template <typename T, typename V, typename cmp = std::less<T>>
class rBtree
{
private:
    cmp comp;
    struct rb_node
    {
        T key;
        V val;
        unsigned long parent_color;
        rb_node *L, *R;
        rb_node(T key, V val) : key(key), val(val), parent_color(0), L(nullptr), R(nullptr){};
    } __attribute__((aligned(sizeof(long))));
    void setColor(rb_node *p, int color)
    {
        if (!p)
            return;
        p->parent_color &= ~1;
        p->parent_color |= color;
    }
    void setParent(rb_node *p, rb_node *parent)
    {
        if (!p)
            return;
        p->parent_color &= 1;
        p->parent_color |= (unsigned long)parent;
    }
    rb_node *rb_parent(rb_node *p)
    {
        if (!p)
            return nullptr;
        return (rb_node *)((p)->parent_color & ~1);
    }
    unsigned long rb_color(rb_node *p)
    {
        if (!p)
            return rb_BLACK;
        return p->parent_color & 1;
    }
    // 根节点
    rb_node *root;
    // 内部实现
    // 左旋
    void turn_Left(rb_node *p)
    {
        if (!p)
            return;
        // p节点左旋，右节点补上
        assert(p->R);
        // 父节点
        rb_node *parent = rb_parent(p);
        // T1,T2,T3,对应根节点的左儿子，T2，对应右儿子的两个儿子节点
        rb_node *L = p, *R = p->R;
        rb_node *T1 = L->L, *T2 = R->L, *T3 = R->R;
        // L是根节点，R是右儿子
        /*
        L              R
       / \            / \
      T1  R   ->     L  T3
         / \        / \
        T2  T3     T1  T2
      */
        setParent(R, parent);
        L->R = T2;
        setParent(T2, L);
        R->L = L;
        setParent(L, R);
        if (L == root)
        {
            root = R;
            return;
        }
        if (parent->L == L)
            parent->L = R;
        else
            parent->R = R;
    };
    // 右旋
    void turn_right(rb_node *p)
    {
        if (!p)
            return;
        // 右旋，左旋的镜像操作
        assert(p->L);
        // 父节点
        rb_node *parent = rb_parent(p);
        rb_node *L = p->L, *R = p;
        rb_node *T1 = L->L, *T2 = L->R, *T3 = R->R;
        /*
       操作相反
         L              R
        / \            / \
       T1  R   <-     L  T3
          / \        / \
         T2  T3     T1  T2
       */
        setParent(L, parent);
        R->L = T2;
        setParent(T2, R);
        L->R = R;
        setParent(R, L);
        if (R == root)
        {
            root = L;
            return;
        }
        if (parent->L == R)
            parent->L = L;
        else
            parent->R = L;
    };
    // fixed 某个节点合法
    void fixedInsert(rb_node *p)
    {

        if (p == root)
        {
            setColor(p, rb_BLACK);
            return;
        }
        rb_node *parent = rb_parent(p);
        if (!parent or rb_color(parent) == rb_BLACK)
            return;
        if (rb_parent(parent)->R && rb_parent(parent)->L && rb_color(rb_parent(parent)->L) == rb_RED && rb_color(rb_parent(parent)->R) == rb_RED)
        {
            setColor(rb_parent(parent)->L, rb_BLACK);
            setColor(rb_parent(parent)->R, rb_BLACK);
            setColor(rb_parent(parent), rb_RED);
            fixedInsert(rb_parent(parent));
            return;
        }
        // 父节点红，叔叔节点黑或不存在
        // 方向不一致旋转
        if (rb_parent(parent)->L == parent)
        {
            if (parent->R == p)
            {
                turn_Left(rb_parent(p));
                p = parent;
            }
        }
        else if (rb_parent(parent)->R == parent)
        {
            if (parent->L == p)
            {
                turn_right(rb_parent(p));
                p = parent;
            }
        }
        parent = rb_parent(p);
        // 方向一致处理
        setColor(parent, rb_BLACK);
        setColor(rb_parent(parent), rb_RED);
        if (rb_parent(p)->L == p)
        {
            turn_right(rb_parent(parent));
        }
        else
            turn_Left(rb_parent(parent));
    }
    // 寻找节点
    rb_node *findNode(T key)
    {
        rb_node *p = root;
        rb_node *parent = nullptr;
        // 找到或者到空节点了
        while (p && p->key != key)
        {
            // key>p->key
            parent = p;
            if (comp(p->key, key))
            {
                p = p->R;
            }
            else
            {
                p = p->L;
            }
        }
        return p ? p : parent;
    }
    // 旋转fixed
    void fixedRemove(rb_node *p)
    {
        // p节点不平衡
        rb_node *node = nullptr, *tmp1, *tmp2, *sibling;
        while (1)
        {
            sibling = p->R;
            if (sibling != node)
            {
                if (rb_color(sibling) == rb_RED)
                {
                    // 变色左旋
                    setColor(p, rb_RED);
                    setColor(sibling, rb_BLACK);
                    turn_Left(p);
                }
                // 兄弟节点为黑色
                sibling = p->R;
                if (rb_color(sibling) == rb_BLACK)
                {
                    tmp1 = sibling->L;
                    tmp2 = sibling->R;
                    // 父节点为红色
                    if (rb_color(p) == rb_RED && rb_color(tmp1) == rb_BLACK && rb_color(tmp2) == rb_BLACK)
                    {
                        setColor(p, rb_BLACK);
                        setColor(sibling, rb_RED);
                        break;
                    }
                    else
                    {
                        // 左儿子为红
                        if (rb_color(tmp1) == rb_RED)
                        {
                            setColor(tmp1, rb_BLACK);
                            setColor(sibling, rb_RED);
                            turn_right(sibling);
                            // 更新兄弟节点
                            sibling = rb_parent(sibling);
                        }
                        // 右儿子为红
                        tmp2 = sibling->R;
                        if (rb_color(tmp2) == rb_RED)
                        {
                            // 兄弟变为父色，父和右儿变黑，左旋p
                            setColor(sibling, rb_color(p));
                            setColor(tmp2, rb_BLACK);
                            setColor(p, rb_BLACK);
                            turn_Left(p);
                            break;
                        }
                        node = p;
                        p = rb_parent(p);
                        if (p)
                            continue;
                        break;
                    }
                }
            }
            else
            {
                sibling = p->L;
                if (rb_color(sibling) == rb_RED)
                {
                    // 变色左旋
                    setColor(p, rb_RED);
                    setColor(sibling, rb_BLACK);
                    turn_right(p);
                }
                // 兄弟节点为黑色
                sibling = p->L;
                if (rb_color(sibling) == rb_BLACK)
                {
                    tmp1 = sibling->L;
                    tmp2 = sibling->R;
                    // 父节点为红色
                    if (rb_color(p) == rb_RED && rb_color(tmp1) == rb_BLACK && rb_color(tmp2) == rb_BLACK)
                    {
                        setColor(p, rb_BLACK);
                        setColor(sibling, rb_RED);
                        break;
                    }
                    // 都是黑色的
                    else
                    {
                        // 右儿子为红
                        if (rb_color(tmp2) == rb_RED)
                        {
                            setColor(tmp2, rb_BLACK);
                            setColor(sibling, rb_RED);
                            turn_Left(sibling);
                            // 更新兄弟节点
                            sibling = rb_parent(sibling);
                        }
                        tmp1 = sibling->L;
                        if (rb_color(tmp1) == rb_RED)
                        {
                            // 兄弟变为父色，父和左儿变黑，左旋p
                            setColor(sibling, rb_color(p));
                            setColor(tmp1, rb_BLACK);
                            setColor(p, rb_BLACK);
                            turn_right(p);
                            break;
                        }
                        node = p;
                        p = rb_parent(p);
                        if (p)
                            continue;
                        break;
                    }
                }
            }
        }
    };
    // 设置子节点
    void make_child(rb_node *parent, rb_node *ignore, rb_node *child)
    {
        if (parent)
        {
            if (parent->L == ignore)
            {
                parent->L = child;
            }
            else if (parent->R == ignore)
            {
                parent->R = child;
            }
        }
        else
            root = child;
        if (child)
            setParent(child, parent);
    }
    rb_node *removeNode(T key)
    {
        // 找到删除节点
        rb_node *p = findNode(key);
        // 没找到需要删除的节点
        if (!p || p->key != key)
        {
            // 无需调整
            return nullptr;
        }
        // 找到后继节点
        rb_node *suc = p;
        // 需要调整的节点
        rb_node *fixedNode = nullptr;
        // 叶子节点直接删除
        // 只有右儿
        if (p->L == nullptr)
        {
            suc = p->R;
            unsigned long pc = p->parent_color;
            make_child(rb_parent(p), p, suc);
            if (suc)
                suc->parent_color = pc;
            else if ((pc & 1) == rb_BLACK)
                fixedNode = rb_parent(p);
        }
        // 只有左儿
        else if (p->R == nullptr)
        {
            suc = p->L;
            unsigned long pc = p->parent_color;
            make_child(rb_parent(p), p, suc);
            suc->parent_color = pc;
        }
        else
        {
            suc = p->R;
            rb_node *child2, *parent;
            while (suc->L)
                suc = suc->L;
            rb_node *gg = rb_parent(suc);
            if (rb_parent(suc) != p)
            {
                // 替换操作需要把子节点顶上之前的位置
                child2 = suc->R;
                parent = rb_parent(suc);
                make_child(rb_parent(suc), suc, suc->R);
                suc->R = p->R;
                fixedNode = rb_parent(suc);
                setParent(p->R, suc);
            }
            else
            {
                parent = suc;
                child2 = suc->R;
            }
            unsigned long pc = p->parent_color;
            suc->L = p->L;
            setParent(p->L, suc);
            unsigned pc2 = suc->parent_color;
            make_child(rb_parent(p), p, suc);
            suc->parent_color = pc;
            if (child2)
            {
                setParent(child2, parent);
                setColor(child2, rb_BLACK);
                fixedNode = nullptr;
            }
            else
            {
                fixedNode = ((pc2 & 1) == rb_BLACK ? parent : nullptr);
            }
        }
        if (root == p)
            root = suc;
        delete p;
        return fixedNode;
    }
    // 遍历红黑树的key and val
    void traverse(rb_node *node, std::vector<std::tuple<T, V, std::string>> &p)
    {
        if (node == nullptr)
            return;
        traverse(node->L, p);
        p.emplace_back(node->key, node->val, rb_color(node) == rb_RED ? "RED" : "BLACK");
        traverse(node->R, p);
    }
    void destroy(rb_node *node)
    {
        if (!node)
            return;
        destroy(node->L);
        destroy(node->R);
        delete node;
    }
    int dfsCheck(rb_node *p)
    {
        if (!p)
            return 0;
        if (p == root && rb_color(p) == rb_RED)
        {
            return -1;
        }
        int a = dfsCheck(p->L), b = dfsCheck(p->R);
        if (a == -1 || b == -1)
            return -1;
        if (rb_color(p) == rb_RED && (rb_color(p->L) == rb_RED || rb_color(p->R) == rb_RED))
            return -1;
        if (p->L&&rb_parent(p->L) != p || p->R&&rb_parent(p->R) != p)
            return -1;
        if (a != b)
            return -1;
        return a + rb_color(p);
    }

public:
    // 构造函数
    rBtree() : root(nullptr), comp(cmp()){};
    // 析构函数
    ~rBtree()
    {
        destroy(root);
    }
    bool checkTree()
    {
        int res = dfsCheck(root);
        return res != -1;
    } // 添加节点
    int insert(T key, V val)
    {
        rb_node *parent = findNode(key);
        // 已存在节点
        if (parent && parent->key == key)
        {
            parent->val = val;
            return 1;
        }
        // 新增节点处理

        rb_node *p = new rb_node(key, val);
        setColor(p, rb_RED);
        setParent(p, parent);
        // 树为空
        if (!parent)
        {
            root = p;
        }
        else if (comp(parent->key, key))
        {
            parent->R = p;
        }
        else
        {
            parent->L = p;
        }
        // 完成染色旋转等操作
        fixedInsert(p);
        return 1;
    }
    // 删除节点
    int erase(T key)
    {
        rb_node *fixedNode = removeNode(key);
        if (fixedNode)
            fixedRemove(fixedNode);
        return 1;
    }
    // 修改节点
    void modify(T key, V val)
    {
        // 发现相同键值直接修改
        insert(key, val);
    }
    // 查找节点
    std::optional<V> search(T key)
    {
        rb_node *res = findNode(key);
        if (res && res->key == key)
        {
            return res->val;
        }
        else
        {
            return std::nullopt;
        }
    }
    // 遍历红黑树的key and val
    std::vector<std::tuple<T, V, std::string>> traverse()
    {
        std::vector<std::tuple<T, V, std::string>> res;
        traverse(root, res);
        return res;
    }
};
