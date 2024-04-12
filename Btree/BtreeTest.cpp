#include "Btree.h"
#include <iostream>
int main()
{
    Btree<int, int> tree(5); // 使用默认的比较函数
    while (1)
    {
        int op;
        std::cin >> op;
        if (op == 1)
        {
            int key, val;
            std::cout << "input key and val: ";
            std::cin >> key >> val;
            tree.insert(key, val);
        }
        else if (op == 2)
        {
            int x;
            std::cout << "input search key: ";
            std::cin >> x;
            auto res = tree.search(x);
            if (res.has_value())
                std::cout << res.value() << std::endl;
            else
                std::cout << "Not Found\n";
        }
        else if (op == 3)
        {
            int x;
            std::cout << "input remove item: ";
            std::cin >> x;
            tree.remove(x);
        }
        else if (op == 4)
        {
            std::cout << "traverse result: \n";
            for (auto [key, val] : tree.traverse())
            {
                std::cout << key << " " << val << std::endl;
            }
        }
    }
    return 0;
}
