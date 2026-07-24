#ifndef POOL_H
#define POOL_H

#include <queue>
#include <functional>

template <typename T>
class Pool {
    std::queue<T> queue;
    std::function<T()> factory;

    public:
        Pool(std::function<T()> factory) : factory(std::move(factory)) {}

        int size() const {
            return queue.size();
        }

        T get() {
            if (queue.size() > 0) {
                T obj = queue.front();
                queue.pop();
                return obj;
            } else {
                return factory();
            }
        }

        void free(T object) {
            queue.push(std::move(object));
        }
};

#endif