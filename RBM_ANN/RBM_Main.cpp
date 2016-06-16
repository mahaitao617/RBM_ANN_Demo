#include "RBM.h"
#include "Math_Util.hpp"
#include <iostream>
using namespace std;

typedef struct {
    string file; //ѵ�����ļ�
    int n_out;   //��������
    int n_train; //ѵ��������
} TrainTxt;

TrainTxt tt__[] = {
    { "./train_test/train01.txt", 2, 250},  //[0]
    { "./train_test/train0123.txt", 4, 500},   //[1]
    { "./train_test/train012345.txt", 6, 750},    //[2]
    { "./train_test/train01234567.txt", 8, 1000},   //[3]
    { "./train_test/train0123456789.txt", 10, 1250},   //[4]
};

int main()
{
    int idx = GetPrivateProfileInt("RBM", "TrainTxtIdx", 0, ".\\set.ini");
    char learnRate[10] = "";
    GetPrivateProfileString("RBM", "LearnRate", "0.1", learnRate, 10, ".\\set.ini");
    TrainTxt& tt = tt__[idx]; ///ͨ���޸�������벻ͬ��ѵ����
    cout << Math_Util::getDateTime() << "\t" << tt.file << "\t" << tt.n_train << endl;
    SetText(FG_HL | FG_G | FG_B);
    try {
        int hideUnits[] = { 196, 49 };
        RBM rbm(784, hideUnits, atof(learnRate));
        rbm.loadTrainSet(tt.file, tt.n_train);
        rbm.train(0.01, 10000);  //��������������,����һ��������ֹͣ
    } catch (const logic_error& err) {
        cout << "\r---error:" << err.what() << endl;
    } catch (...) {
        cout << "\nOops, there are some jokes in the runtime \\(�s-�t)/" << endl;
    }
    cin.get();
    return 0;
}
