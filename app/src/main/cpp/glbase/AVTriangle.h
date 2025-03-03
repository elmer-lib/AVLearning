//
// Created by 周豪豪 on 2025/3/3.
//

#ifndef AVLEARNING_AVTRIANGLE_H
#define AVLEARNING_AVTRIANGLE_H


class AVTriangle {
public:
    AVTriangle();

    virtual ~AVTriangle();

    int init(void);

    void onDraw(int width, int height);

    void destroy();

private:
    int programHandle;
};

#endif //AVLEARNING_AVTRIANGLE_H
