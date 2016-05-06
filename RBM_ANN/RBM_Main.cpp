#include "RBM.h"
#include "Math_Util.hpp"
#include <iostream>
using namespace std;

int main()
{
    cout << Math_Util::getDateTime() << endl;
    SetText(FG_HL | FG_G | FG_B);
    try {
        int hideUnits[] = { 100, 25 };
        RBM rbm(784, hideUnits);

        cout << "\r>>>��������ѵ�����ݺͲ�������...";
        time_t t_start = clock();
        rbm.loadTrain("./train_test/train0123.txt", 400);
        cout << "\r����ѵ���Ͳ������ݺ�ʱ: " << (clock() - t_start) << "ms" << endl;

        t_start = clock();
        rbm.train(0.02, 10000);  //��������������������һ��������ֹͣ
        cout << "\n�ݻ�����ʱ: " << (clock() - t_start) / CLK_TCK << " s" << endl;
    } catch (const logic_error &err) {
        cout << "\r---error:" << err.what() << endl;
    } catch (...) {
        cout << "\nOops, there are some jokes in the runtime \\(�s-�t)/" << endl;
    }
    cin.get();
    return 0;
}
