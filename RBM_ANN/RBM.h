#ifndef _RBM_H_
#define _RBM_H_

#include <string>
#include <vector>

using std::vector;
using std::string;

typedef unsigned uint;
typedef vector<float> vectorF;
typedef vector<vectorF> vectorF2D;
typedef vector<vectorF2D> vectorF3D;
typedef vector<unsigned> vectorU;

/**
    ���޲�����������,����һ����㲣�������������ԺͲ���
**/
class RBM {
public:
    typedef struct {
        vectorF data;        //�������������������ݲ���
        int tag;             //�����������������ǩ(��or�� or ...)
    } RBMInput;              //������������������
    typedef struct {
        vectorF3D weight;    //Ȩ��
        vectorF2D hbias;     //ƫ�ò�����������ÿ����Ԫ��һ��ƫ��[����ƫ��]
        vectorF2D vbias;     //ƫ�ò�����������ÿ����Ԫ��һ��ƫ��[����ƫ��]
        float fitValue;      //��Ӧֵ,�ø���ѵ����ʵ�����,��ÿһά��ֵ֮��
    } RBMIndividual;         //�����������,�������п�ѵ�������ļ���

    //�����ֱ���[��������ά��, �����ͼ����ָƽ�̺��С],
    //[�ڲ�����ÿ�����Ԫ����], [�����ݻ�������]
    RBM(uint _inputSize, const vectorU& hiddenSizes, double learnRate = .5, uint _popSize = 1)
    {
        init(_inputSize, hiddenSizes, learnRate, _popSize);
    }

    template<int n>
    RBM(uint _inputSize, const int(&hiddenSizes)[n], double learnRate = .5, uint _popSize = 1)
    {
        init(_inputSize, vector<uint>(hiddenSizes, hiddenSizes + n), learnRate, _popSize);
    }
    virtual ~RBM() {}
    //���ļ�file����size��ѵ��������,divideToTest����Ƿ񽫲���������Ϊ���Լ�
    void loadTrainSet(const string& file, uint size = 0, bool divideToTest = 1);
    //���ļ������������
    void loadTestSet(const string& file, uint size = 0, bool haveTag = 0);
    //��ʼѵ��
    void train(double permitError = 0.05, uint maxGens = 100000);
    //��RBM���һ�������������ļ�
    void saveRBMOutToFile(const string& file);
    //�õ�RBM�����һ������
    vector<RBMInput> getRBMOut();
    //��ѵ�����������ȡratio����������Ϊ���Լ�[ѵ��������Щ���ݽ��޳�]
    void randomDivideTrainToTest(double ratio);
protected:
    //��ʼ������ṹ�Ͳ���
    void init(uint _inputSize, const vectorU& hiddenSizes, double learnRate = .5, uint _popSize = 1);
    //ǰ�򴫵�
    void forward(const vectorF& vis, vectorF& hide, const vectorF2D& wt, const vectorF& b);
    //���򴫲�
    void backward(vectorF& vis, const vectorF& hide, const vectorF2D& wt, const vectorF& b);
    //����Ȩֵ��ƫ��
    double adjust_hvh(RBMIndividual& _rbmPop, uint hideIndex);
    //��ȡ����_rbmPop����dataSet�����ϵ�hideindex�����Ӧֵ
    float getFitValue(const RBMIndividual& _rbmPop, const vector<RBMInput>& dataSet, uint hideIndex);
    //��Ԥ���ļ��ж����ݻ�����
    bool loadParam(const string& file);
    //�����������ͼ��ӡ���ļ���
    void printParamToFile(const string& file, bool onlyBest = 0);
    //����õķ��ݽ��������ļ�
    void saveBestReTrain(const string& file);
    //���ҵ�ǰ��ø���
    uint findBestPop();
protected:
    uint inputSize;          //�������ݵĴ�С,��ά��
    //vectorF visible;       //visible��,���Բ�,ά�����������ݵĴ�С[note:��ѵ����ȡ��]
    vectorF2D hidden;        //hidden��,������(�����ж��,�����ּ�һά)
    vector<RBMInput> trainSet;   //ѵ����-���������������ݺͱ�ǩ��Ϣ
    vector<RBMInput> testSet;    //���Լ�-����������������(Ҳ�ɰ�����ǩ��Ϣ)
    vector<RBMIndividual> rbmPop;//���弯��
    float learningRate;          //ѧϰ��
    uint bestPopIndex;           //��ø��������
};

#ifndef _MSC_VER //���ݷ���Ŀ���뻷��
#include "RBM.cpp"
#endif

#endif //_RBM_H_
