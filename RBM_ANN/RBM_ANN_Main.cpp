#define _CRT_SECURE_NO_WARNINGS
#include "RBM.h"
#include "ANN.h"
#include "Math_Util.hpp"
#include <iostream>
using namespace std;

typedef struct {
    string file; //ѵ�����ļ�
    int n_out;   //��������
    int n_train; //ѵ��������
} TrainTxt;

TrainTxt tt__[] = {
    { "./train_test/train01.txt", 1, 300},  //[0]
    { "./train_test/train0123.txt", 2, 600},   //[1]
    { "./train_test/train012345.txt", 3, 900},    //[2]
    { "./train_test/train01234567.txt", 3, 1200},   //[3]
    { "./train_test/train0123456789.txt", 4, 1500},   //[4]
};

int main()
{
    cout << Math_Util::getDateTime() << endl;
    SetText(FG_HL | FG_G | FG_B);
    try {
        TrainTxt& tt = tt__[2]; //ͨ���޸�������벻ͬ��ѵ����
        int hideUnits[] = { 144, 36 };
        RBM rbm(784, hideUnits);
        cout << "\r>>>��������ѵ�����ݺͲ�������...";
        time_t t_start = clock();
        rbm.loadTrain(tt.file, tt.n_train);
        cout << "\r����ѵ���Ͳ������ݺ�ʱ: " << (clock() - t_start) << "ms" << endl;
        t_start = clock();
        rbm.train(0.05, 10000);  //��������������������һ��������ֹͣ
        cout << "�ݻ�����ʱ: " << (clock() - t_start) / 1000.0 << " s\n\n";
        rbm.saveRBMOutToFile("rbmOut.txt"); //��RBM��������Լ���ǩ���浽�ļ�
        uint rbmOutSize = hideUnits[sizeof(hideUnits) / sizeof(hideUnits[0]) - 1];
        ///��RBM��ȡ�������͵�ANN�н��з���
        ANN ann(rbmOutSize, tt.n_out, 1); //�������ݴ�С,�������Ԫ����,�������
        ann.loadTrainSet("rbmOut.txt"); //��ȡRBM�������ΪANN��ѵ������
        ann.train(0.02, 1000000);
    } catch (const logic_error &err) {
        cout << "\r---error:" << err.what() << endl;
    } catch (...) {
        cout << "\nOops, there are some jokes in the runtime, I am lost in the jungle\\(�s-�t)/" << endl;
    }
    cin.get();
    return 0;
}
