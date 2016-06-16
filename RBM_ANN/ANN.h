#ifndef _ANN_H_
#define _ANN_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

typedef unsigned uint;
typedef vector<float> vectorF;
typedef vector<vectorF> vectorF2D;
typedef vector<vectorF2D> vectorF3D;
typedef vector<unsigned> vectorU;

/**
 * ������������
 **/
class ANN
{
public:
    typedef struct {
        vectorF data;  //�������ݲ���
        int tag;       //�����ǩ(��or�� or ...)
    } ANNInput;        //�������ݵĽṹ
    typedef struct {
        vectorF3D weight; //Ȩ�ز���,������������ÿ����Ԫ����һ��Ȩֵ
        vectorF2D bias;   //ƫ�ò���,������������ÿ����Ԫ����һ��ƫ��
        float fitValue;   //��Ӧֵ
    } ANNIndividual;      //�������ݻ���������,�������п�ѵ������
    typedef struct {
        vectorF input;    //�����
        vectorF2D hide;   //���ز�
        vectorU output;   //�����
        int tag;          //����Ԥ��õ��ı�ǩ
    } ANNLayer;           //ANN����ṹ

    //@param:��������, ���������, ���ز����
    ANN(const vector<ANNInput>& _inputData, uint outputNums, uint hideLayers = 1);
    //@param:��������ά��, ����������, ���ز����
    ANN(uint inputSize, uint outputNums, uint hideLayers = 1);
    ~ANN() {}
    //���ļ�file����size��ѵ��������,divideToTest����Ƿ񽫲���������Ϊ���Լ�
    void loadTrainSet(const string& file, uint size = 0, bool divideToTest = 1);
    //���ļ�file����size����Լ�����,haveTag����Ƿ��б�ǩ
    void loadTestSet(const string& file, uint size = 0, bool haveTag = 0);
    //��ʼѵ�������� @param: ������� 0.0~1.0 ; ����������
    void train(double permitError, uint maxGens = 100000);
    //����ѵ���õ�����,��ò��Լ�Ԥ����
    vector<int> getTestOut() const;
    //�ԱȲ��Լ������,������ȷ��---ǰ������֪ÿ��������ݵ����
    float compareTestOut() const;
    //��ѵ�����������ȡratio����������Ϊ���Լ�[ѵ��������Щ���ݽ��޳�]
    void randomDivideTrainToTest(double ratio = 0.25);
protected:
    //������ṹ�����ݻ��������г�ʼ��
    void init(uint inputSize, uint outputNums, uint hideLayers = 2);
    //����ĳ���������ݶ�Ӧ��������� (Ԥ���ǩ)
    uint getANNOut(const ANNInput& _input, const ANNIndividual& indiv) const;
    //����������Ӧֵ
    float getFitValue(ANNIndividual& indiv);
    //������첢ѡ����Ÿ��������һ��
    void mutate(ANNIndividual& _annPop);
    //ͨ����˹���췽ʽ��������---mutate��һ��ʵ�ַ�ʽ
    void mutateByGauss(ANNIndividual& tmpPop);
    //�������Ÿ���ĳ�����
    const ANNIndividual& getBestPop() const;
protected:
    mutable ANNLayer layer;       //�񾭽ṹ��
    vector<ANNInput> trainSet;    //������ѵ������
    vector<ANNInput> testSet;     //�������������
    vector<ANNIndividual> annPop; //���������ɵ���Ⱥ

    static const float F_ANN; //������ı������
};

#ifndef _MSC_VER //���ݷ���Ŀ���뻷��
#include "ANN.cpp"
#endif  //_MSC_VER

#endif  //_ANN_H_
