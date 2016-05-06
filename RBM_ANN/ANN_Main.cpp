#include "ANN.h"
#include <iostream>
using namespace std;

typedef struct {
    string file; //ѵ�����ļ�
    int n_out;   //��������
    int n_train; //ѵ��������
} TrainTxt;

TrainTxt tt__[] = {
    { "./train_test/train01.txt", 2, 300},  //[0]
    { "./train_test/train0123.txt", 4, 600},   //[1]
    { "./train_test/train012345.txt", 6, 900},    //[2]
    { "./train_test/train01234567.txt", 8, 1200},   //[3]
    { "./train_test/train0123456789.txt", 10, 1500},   //[4]
};

int main()
{
    try {
        TrainTxt& tt = tt__[3]; ///ͨ���޸�������벻ͬ��ѵ����
        ANN ann(784, tt.n_out, 1); //��������ά��, ����������, ���ز����
        ann.loadTrainSet(tt.file, tt.n_train);
        ann.train(0.01, 1000000);
    } catch (const logic_error& err) {
        cout << "\r---error:" << err.what() << endl;
    } catch (...) {
        cout << "Oops, I am so sorry to print this!" << endl;
    }
    cin.get();
    return 0;
}
