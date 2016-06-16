#include "ANN.h"
#include "Math_Util.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cfloat>
#include <stdexcept> //logic_error
using namespace std;

const float ANN::F_ANN = 0.3f; //������ı���ϵ��

//@param:��������, ���������, ���ز����
ANN::ANN(const vector<ANNInput>& _inputData, uint outputNums, uint hideLayers)
{
    trainSet = _inputData;
    if (trainSet.size() > 0)
        init(trainSet[0].data.size(), outputNums, hideLayers);
}

//@param:��������ά��, ���������, ���ز����
ANN::ANN(uint inputSize, uint outputNums, uint hideLayers)
{
    init(inputSize, outputNums, hideLayers);
}

//������ṹ�����ݻ��������г�ʼ��
void ANN::init(uint inputSize, uint outputNums, uint hideLayers)
{
    Math_Util::setSrand();
    if (inputSize == 0 || outputNums == 0 || hideLayers == 0)
        return;
    layer.input.resize(inputSize);    //��ʼ���������Ԫ����
    layer.output.resize(outputNums);  //��ʼ���������Ԫ����
    layer.hide.resize(hideLayers);    //��ʼ�����������
    uint i, j, k, p;
    annPop.resize(2); //�趨��Ⱥ������������
    for (p = 0; p < annPop.size(); ++p) {
        annPop[p].weight.resize(hideLayers + 1); //������������ÿ����Ԫ����һ��Ȩֵ
        annPop[p].bias.resize(hideLayers + 1);   //������������ÿ����Ԫ����һ��ƫ��
        uint hideNums = sqrt(1.0 + layer.output.size() * layer.input.size() / 3); //���ز�ÿ�����Ԫ����
        for (i = 0; i < hideLayers; ++i) {
            layer.hide[i].resize(hideNums);
            annPop[p].weight[i].resize(hideNums);
            for (j = 0; j < hideNums; ++j)
                if (i == 0) //��һ������ÿ����Ԫ��Ȩֵ��������������
                    annPop[p].weight[i][j].resize(layer.input.size());
                else
                    annPop[p].weight[i][j].resize(hideNums);
            annPop[p].bias[i].resize(hideNums, 0);
        }
        //��ʱ i=hideLayers, �����������ÿ����Ԫ��һ��Ȩֵ��ƫ��
        annPop[p].weight[i].resize(outputNums);
        for (j = 0; j < outputNums; ++j)
            annPop[p].weight[i][j].resize(hideNums);
        annPop[p].bias[i].resize(outputNums, 0);
        //��ʼ������Ȩֵ,ƫ���ѳ�ʼ��Ϊ0
        for (i = 0; i < annPop[p].weight.size(); ++i)
            for (j = 0; j < annPop[p].weight[i].size(); ++j)
                for (k = 0; k < annPop[p].weight[i][j].size(); ++k)
                    annPop[p].weight[i][j][k] = Math_Util::randFloat(-1, 1);
    }
}

//���ļ�file����size��ѵ��������,divideToTest����Ƿ񽫲���������Ϊ���Լ�
void ANN::loadTrainSet(const string& file, uint size, bool divideToTest)
{
    if (file == "")
        return;
    ifstream loadFile(file.c_str());
    if (loadFile.is_open()) {
        cout << "reading train set...";
        clock_t t1 = clock();
        trainSet.clear();
        if (size > 0)
            trainSet.reserve(size);
        uint i, j;
        ANNInput input = { vectorF(layer.input.size()), 0 };
        for (i = 0; i < size || size == 0; ++i) {
            for (j = 0; j < input.data.size(); ++j) //��ȡһ������
                if (!(loadFile >> input.data[j]))
                    break;
            //��������û�ж�ȡ����,�򲻼���ѵ����
            if (j < input.data.size() || !(loadFile >> input.tag))
                break;
            trainSet.push_back(input);
        }
        loadFile.close();
        double elapsed_ms = 1.0 * (clock() - t1) / CLOCKS_PER_SEC;
        if (divideToTest) { //���������������Ϊ���Լ�
            testSet.resize(trainSet.size() / 5);
            for (i = 0; i < testSet.size(); ++i) {
                j = rand() % trainSet.size();
                testSet[i] = trainSet[j];
                trainSet.erase(trainSet.begin() + j);
            }
            cout << "\r>>succeed to load " << trainSet.size() << " train sets, "
                 << "and generate " << testSet.size() << " test sets!("
                 << elapsed_ms << " s)" << endl;
        } else {
            cout << "\r>>succeed to load " << trainSet.size() << " train sets!("
                 << elapsed_ms << " s)" << endl;
        }
    } else {
        string msg = "Failed to load data set file '" + file + "', please check"
                     " if it exists or has access to read!\n";
        throw logic_error(msg);
    }
}

//���ļ�file����size����Լ�����, haveTag����Ƿ��б�ǩ
void ANN::loadTestSet(const string& file, uint size, bool haveTag)
{
    if (file == "")
        return;
    ifstream loadFile(file.c_str());
    if (loadFile.is_open()) {
        cout << "reading test set...";
        testSet.clear();
        if (size > 0)
            testSet.reserve(size);
        uint i, j;
        ANNInput input = { vectorF(layer.input.size()), 0 };
        for (i = 0; i < size || size == 0; ++i) {
            for (j = 0; j < input.data.size(); ++j) //��ȡһ������
                if (!(loadFile >> input.data[j]))
                    break;
            //��������û�ж�ȡ����,�򲻼���ѵ����
            if (j < input.data.size() || (haveTag && !(loadFile >> input.tag)))
                break;
            testSet.push_back(input);
        }
        loadFile.close();
        cout << "\r>>succeed to load " << testSet.size() << " test sets!" << endl;
    } else {
        string msg = "Failed to load data set file '" + file + "', please check"
                     " if it exists or has access to read!\n";
        throw logic_error(msg);
    }
}

//��ʼѵ�������� @param: ������� 0.0~1.0 ; ����������
void ANN::train(double permitError, uint maxGens)
{
    if (trainSet.size() == 0) {
        cout << "please load train set first!" << endl;
        return;
    }
    cout << ">>>Evolving...\nGen\ttrain accuracy\ttest accuracy\taverage elapsed(ms)" << endl;
    for (uint p = 0; p < annPop.size(); ++p)
        getFitValue(annPop[p]);  //�����ʼ��ʱ�������Ӧֵ
    clock_t lastStart = clock(), t_start = lastStart;
    float lastFitValue = 1;
    uint lastGen = -1;
    for (uint i = 0; i < maxGens; ++i) {
        for (uint p = 0; p < annPop.size(); ++p)
            mutate(annPop[p]);
        const ANN::ANNIndividual &bestPop = getBestPop();
        if (i % 10 == 0 || clock() - lastStart > 200) {
            double elapsed_ms = 1000.0 * (clock() - lastStart) / CLOCKS_PER_SEC;
            cout << '\r' << setiosflags(ios::left) << setw(8) << i << setw(16)
                 << 1 - bestPop.fitValue << setw(16) << compareTestOut()
                 << elapsed_ms / (i - lastGen) << '\t';
            lastStart = clock();
            lastGen = i;
            if (lastFitValue - bestPop.fitValue > 0.008) {
                cout << endl;
                lastFitValue = bestPop.fitValue;
            }
            if (bestPop.fitValue < permitError)
                break;
        }
        if (checkKeyDown() == 27 &&
                MessageBox(0, "you pressed ESC ,do you want to stop the evolution?",
                           "stop", MB_YESNO | MB_ICONQUESTION) == IDYES)
            break;
    }
    double elapsed_s = 1.0 * (clock() - t_start) / CLOCKS_PER_SEC;
    if (elapsed_s > 60)
        cout << endl << "time elapsed: " << elapsed_s / 60 << " min" << endl;
    else
        cout << endl << "time elapsed: " << elapsed_s << " s" << endl;
}

//����ѵ���õ�����,��ò��Լ�Ԥ����
vector<int> ANN::getTestOut() const
{
    if (testSet.size() == 0)
        return vector<int>();
    vector<int> tagOut(testSet.size());
    const ANNIndividual& bestPop = getBestPop();
    for (int i = 0; i < testSet.size(); ++i) {
        tagOut[i] = getANNOut(testSet[i], bestPop);
    }
    return tagOut;
}

//�ԱȲ��Լ������,������ȷ��. [note:ǰ������֪ÿ��������ݵ����]
float ANN::compareTestOut() const
{
    if (testSet.size() == 0)
        return 0;
    float error = 0;
    const ANN::ANNIndividual &bestPop = getBestPop();
    for (uint i = 0; i < testSet.size(); ++i) {
        getANNOut(testSet[i], bestPop); //��������������ݵ����
        if (layer.tag != testSet[i].tag) //�Ա�����Ԥ��õ���tag��ԭʼ������tag
            error += 1;
    }
    if (testSet.size() > 0)
        error /= testSet.size();
    return 1 - error;
}

//��ѵ�����������ȡratio����������Ϊ���Լ�[ѵ��������Щ���ݽ��޳�]
void ANN::randomDivideTrainToTest(double ratio)
{
    uint testSize = (uint)(trainSet.size() * ratio); //���Լ���С
    if (testSize < 1 || testSize > trainSet.size()) //���������Χ����ȷ��Ĭ�ϻ���1/4
        testSize = (int)(trainSet.size() * 0.2);
    testSet.resize(testSize);
    for (uint i = 0; i < testSize; ++i) {    //��������������������Լ�
        uint j = rand() % trainSet.size();
        testSet[i] = trainSet[j];
        trainSet.erase(trainSet.begin() + j);
    }
}

//����ĳ���������ݶ�Ӧ���������
uint ANN::getANNOut(const ANNInput& _input, const ANNIndividual& indiv) const
{
    uint j, k, index;
    ///Ϊ�˼��ټ�������,��ȥ��_input.data��layer.input�ĸ��ƹ���
    //��һ���������������ݺͶ�ӦȨֵ�õ�
    for (j = 0; j < layer.hide[0].size(); ++j) {
        layer.hide[0][j] = 0;
        for (k = 0; k < _input.data.size(); ++k)
            layer.hide[0][j] += _input.data[k] * indiv.weight[0][j][k];
        layer.hide[0][j] += indiv.bias[0][j];
        //layer.hide[0][j] = Math_Util::tansig(layer.hide[0][j]);
    }
    //�����������ǰһ������Ͷ�ӦȨֵ�õ�
    for (index = 1; index < layer.hide.size(); ++index)
        for (j = 0; j < layer.hide[index].size(); ++j) {
            layer.hide[index][j] = 0;
            for (k = 0; k < layer.hide[index - 1].size(); ++k)
                layer.hide[index][j] += layer.hide[index - 1][k] * indiv.weight[index][j][k];
            layer.hide[index][j] += indiv.bias[index][j];
            //layer.hide[index][j] = Math_Util::tansig(layer.hide[index][j]);
        }
    //����������һ������Ͷ�Ӧ��Ȩֵ�õ�
    layer.tag = 0;
    double maxOutput = -DBL_MAX;
    index = layer.hide.size() - 1; //���һ���������
    for (j = 0; j < layer.output.size(); ++j) {
        double sum = 0;
        for (k = 0; k < layer.hide[index].size(); ++k)
            sum += layer.hide[index][k] * indiv.weight[index + 1][j][k];
        sum += indiv.bias[index + 1][j];
        if (sum > maxOutput) { //softmax�ع鷨: ��ǩΪ����������ֵ����Ԫ���
            maxOutput = sum;
            layer.tag = j;
        }
    }
    return layer.tag;
}

//����������Ӧֵ
float ANN::getFitValue(ANNIndividual& indiv)
{
    indiv.fitValue = 0;
    for (uint i = 0; i < trainSet.size(); ++i) {
        getANNOut(trainSet[i], indiv); //��������������ݵ����
        if (layer.tag != trainSet[i].tag) //�Ա�����Ԥ��õ���tag��ԭʼѵ��������tag
            indiv.fitValue += 1;
    }
    if (trainSet.size() > 0)
        indiv.fitValue /= trainSet.size();
    return indiv.fitValue;
}

//������첢ѡ����Ÿ��������һ��
void ANN::mutate(ANNIndividual& _annPop)
{
    ANNIndividual tmpPop = _annPop;
    mutateByGauss(tmpPop);
    getFitValue(tmpPop);
    static float t = 0.03f; //ģ���˻�ĳ�ʼ�¶�
    if (tmpPop.fitValue < _annPop.fitValue)
        _annPop = tmpPop;
    else if (t > 1e-4 && t > Math_Util::randFloat()) //��һ���ĸ��ʽ���һ���ȵ�ǰ��Ҫ��Ľ�
        _annPop = tmpPop;
    t *= 0.999f;
}

//ͨ����˹���췽ʽ��������---mutate��һ��ʵ�ַ�ʽ
void ANN::mutateByGauss(ANNIndividual& tmpPop)
{
    static float gaussRange = 1.2f;    //��˹����ķ���
    uint i, j, k;
    //��Ȩ��ִ�и�˹����
    for (i = 0; i < tmpPop.weight.size(); ++i)
        for (j = 0; j < tmpPop.weight[i].size(); ++j)
            for (k = 0; k < tmpPop.weight[i][j].size(); ++k)
                if (Math_Util::randFloat() <= F_ANN) {
                    tmpPop.weight[i][j][k] += Math_Util::randGauss(0, gaussRange);
                    //makeInRange(tmpPop.weight[i][j][k], Min_weight, Max_weight);
                }
    //��ƫ�ò���ִ�и�˹����
    for (i = 0; i < tmpPop.bias.size(); ++i)
        for (j = 0; j < tmpPop.bias[i].size(); ++j)
            if (Math_Util::randFloat() <= F_ANN) {
                tmpPop.bias[i][j] += Math_Util::randGauss(0, gaussRange);
                //makeInRange(tmpPop.bias[i][j], Min_bias, Max_bias);
            }
    if (gaussRange > 0.3)
        gaussRange *= 0.9999f;
}

//�������Ÿ���ĳ�����
const ANN::ANNIndividual& ANN::getBestPop() const
{
    float bestFitVlue = annPop[0].fitValue;
    uint best = 0;
    for (uint p = 1; p < annPop.size(); ++p)
        if (annPop[p].fitValue < bestFitVlue) {
            bestFitVlue = annPop[p].fitValue;
            best = p;
        }
    return annPop[best];
}
