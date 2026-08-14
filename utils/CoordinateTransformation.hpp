#include <vector>

typedef struct Transform {
    float xAxisX;
    float xAxisY;

    float yAxisX;
    float yAxisY;

    float originX;
    float originY;

    bool isDefault() const {
        return xAxisX == 1 && xAxisY == 0 && yAxisX == 0 && yAxisY == 1 && originX == 0 && originY == 0;
    }

    static Transform identity() {
        return {
            .xAxisX = 1,
            .xAxisY = 0,

            .yAxisX = 0,
            .yAxisY = 1,

            .originX = 0,
            .originY = 0
        };
    }
} Transform;

// https://www.geeksforgeeks.org/cpp/stack-implementation-in-cpp/
class TransformationStack {
private:
    int top;
    std::vector<Transform> arr;

public:
    TransformationStack(int size) : top(-1), arr(size) {}

    bool push(Transform x) {
        arr[++top] = x;
        return true;
    }

    bool pop() {
        if (top < 0) {
            return false;
        }
        top--;
        return true;
    }

    Transform* peek() {
        if (top < 0) {
            return nullptr;
        }
        return &arr[top];
    }

    bool isEmpty() {
        return top < 0;
    }

    void reset() {
        top = -1;
    }
};