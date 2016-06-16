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
    { "./train_test/train01.txt", 2, 250},  //[0]
    { "./train_test/train0123.txt", 4, 500},   //[1]
    { "./train_test/train012345.txt", 6, 750},    //[2]
    { "./train_test/train01234567.txt", 8, 1000},   //[3]
    { "./train_test/train0123456789.txt", 10, 1250},   //[4]
};

void getImgPredictOut(RBM& rbm, ANN& ann)
{
    string file;
    while (getline(cin, file)) {
        system(("saveRGBToFile.exe " + file).c_str());
        char saveToFile[255] = "";
        GetPrivateProfileString("saveRGBToFile", "saveToFile", "", saveToFile, 255, ".\\set.ini");
        streambuf* coutBuf = cout.rdbuf();
        ofstream fout("tmp");
        cout.rdbuf(fout.rdbuf());
        rbm.loadTestSet(saveToFile, 0, 0);
        DeleteFile(saveToFile);
        rbm.saveRBMOutToFile("rbm.out", 0);
        ann.loadTestSet("rbm.out", 0, 0);
        DeleteFile("rbm.out");
        vector<int> tags = ann.getTestOut();
        fout.close();
        cout.rdbuf(coutBuf);
        DeleteFile("tmp");
        for (auto& elem : tags)
            cout << elem << " ";
        cout << endl;
    }
}

int main()
{
    int idx = GetPrivateProfileInt("RBM_ANN", "TrainTxtIdx", 0, ".\\set.ini");
    char learnRate[10] = "";
    GetPrivateProfileString("RBM_ANN", "LearnRate", "0.3", learnRate, 10, ".\\set.ini");
    TrainTxt& tt = tt__[idx]; ///ͨ���޸�������벻ͬ��ѵ����
    cout << Math_Util::getDateTime() << "\t" << tt.file << "\t" << tt.n_train << endl;
    SetText(FG_HL | FG_G | FG_B);
    try {
        int hideUnits[] = { 196, 49 };
        RBM rbm(784, hideUnits, atof(learnRate));
        rbm.loadTrainSet(tt.file, tt.n_train);
        rbm.train(0.05, 10000);  //��������������������һ��������ֹͣ
        rbm.saveRBMOutToFile("rbmOut.txt"); //��RBM��������Լ���ǩ���浽�ļ�
        uint rbmOutSize = hideUnits[sizeof(hideUnits) / sizeof(hideUnits[0]) - 1];
        cout << endl;
        ///��RBM��ȡ�������͵�ANN�н��з���
        ANN ann(rbmOutSize, tt.n_out, 1); //�������ݴ�С,�������Ԫ����,�������
        ann.loadTrainSet("rbmOut.txt"); //��ȡRBM�������ΪANN��ѵ������
        ann.train(0.001, 1000000);
        getImgPredictOut(rbm, ann);
    } catch (const logic_error& err) {
        cout << "\r---error:" << err.what() << endl;
    } catch (...) {
        cout << "\nOops, there are some jokes in the runtime,"
             "I am lost in the jungle\\(�s-�t)/" << endl;
    }
    cin.get();
    return 0;
}
