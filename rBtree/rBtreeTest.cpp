#include "rBtree.h"
#include <iostream>
int main()
{
    rBtree<int, int> tree; // 使用默认的比较函数
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
            tree.erase(x);
        }
        else if (op == 4)
        {
            std::cout << "traverse result: \n";
            for (auto [key, val, color] : tree.traverse())
            {
                std::cout << key << " " << val << " " << color << std::endl;
            }
        }
        if(!tree.checkTree()){
            std::cout<<"Bug\n"<<std::endl;
        }

    }
    return 0;
}