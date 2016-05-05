#include "ANN.h"
#include "Math_Util.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cfloat>
#include <stdexcept> //logic_error
#include <conio.h>
using namespace std;

const float ANN::F_ANN = 0.3f; //神经网络的变异系数

//@param:输入数据, 输出结点个数, 隐藏层层数
ANN::ANN(const vector<ANNInput>& _inputData, uint outputNums, uint hideLayers)
{
    trainSet = _inputData;
    if (trainSet.size() > 0)
        init(trainSet[0].data.size(), outputNums, hideLayers);
}

//@param:输入数据维数, 输出结点个数, 隐藏层层数
ANN::ANN(uint inputSize, uint outputNums, uint hideLayers)
{
    init(inputSize, outputNums, hideLayers);
}

//对网络结构及可演化参数进行初始化
void ANN::init(uint inputSize, uint outputNums, uint hideLayers)
{
    Math_Util::setSrand();
    if (inputSize == 0 || outputNums == 0 || hideLayers == 0)
        return;
    layer.input.resize(inputSize);    //初始化输入层神经元个数
    layer.output.resize(outputNums);  //初始化输出层神经元个数
    layer.hide.resize(hideLayers);    //初始化隐含层层数
    uint i, j, k, p;
    annPop.resize(2); //设定种群包含两个个体
    for (p = 0; p < annPop.size(); ++p) {
        annPop[p].weight.resize(hideLayers + 1); //隐含层和输出层每个神经元都有一组权值
        annPop[p].bias.resize(hideLayers + 1);   //隐含层和输出层每个神经元都有一个偏置
        uint hideNums = layer.input.size() / 10; //隐藏层每层的神经元个数
        for (i = 0; i < hideLayers; ++i) {
            layer.hide[i].resize(hideNums);
            annPop[p].weight[i].resize(hideNums);
            for (j = 0; j < hideNums; ++j)
                if (i == 0) //第一个隐层每个神经元的权值个数由输入层决定
                    annPop[p].weight[i][j].resize(layer.input.size());
                else
                    annPop[p].weight[i][j].resize(hideNums);
            annPop[p].bias[i].resize(hideNums, 0);
        }
        //此时 i=hideLayers, 即设置输出层每个神经元的一组权值和偏置
        annPop[p].weight[i].resize(outputNums);
        for (j = 0; j < outputNums; ++j)
            annPop[p].weight[i][j].resize(hideNums);
        annPop[p].bias[i].resize(outputNums, 0);
        //初始化所有权值,偏置已初始化为0
        for (i = 0; i < annPop[p].weight.size(); ++i)
            for (j = 0; j < annPop[p].weight[i].size(); ++j)
                for (k = 0; k < annPop[p].weight[i][j].size(); ++k)
                    annPop[p].weight[i][j][k] = Math_Util::randFloat(-1, 1);
    }
}

//从文件file载入size组训练集数据,divideToTest标记是否将部分数据作为测试集
void ANN::loadTrainSet(const string& file, uint size, bool divideToTest)
{
    if (file == "")
        return;
    ifstream loadFile(file.c_str());
    if (loadFile.is_open()) {
        cout << "正在读取训练集数据, 请稍后...";
        time_t t1 = clock();
        trainSet.clear();
        if (size > 0)
            trainSet.reserve(size);
        uint i, j;
        ANNInput input = { vectorF(layer.input.size()), 0 };
        for (i = 0; i < size || size == 0; ++i) {
            for (j = 0; j < input.data.size(); ++j) //读取一组输入
                if (!(loadFile >> input.data[j]))
                    break;
            //该组数据没有读取完整,则不加入训练集
            if (j < input.data.size() || !(loadFile >> input.tag))
                break;
            trainSet.push_back(input);
        }
        loadFile.close();
        if (divideToTest) { //随机将部分数据作为测试集
            testSet.resize(trainSet.size() / 5);
            for (i = 0; i < testSet.size(); ++i) {
                j = rand() % trainSet.size();
                testSet[i] = trainSet[j];
                trainSet.erase(trainSet.begin() + j);
            }
            cout << "\r>>成功载入" << trainSet.size() << "组训练集, 并生成"
                << testSet.size() << "组测试集(" << (clock() - t1) / 1000.0 << "s)\n";
        }
        else {
            cout << "\r>>成功载入" << trainSet.size() << "组训练集(" << (clock() - t1) / 1000.0 << "s)\n";
        }
    }
    else {
        string msg = "载入数据集文件'" + file + "失败，请检查该文件是否存在，或有权限读取\n";
        throw logic_error(msg);
    }
}

//从文件file载入size组测试集数据, haveTag标记是否有标签
void ANN::loadTestSet(const string& file, uint size, bool haveTag)
{
    if (file == "")
        return;
    ifstream loadFile(file.c_str());
    if (loadFile.is_open()) {
        cout << "正在读取测试集数据, 请稍后...";
        testSet.clear();
        if (size > 0)
            testSet.reserve(size);
        uint i, j;
        ANNInput input = { vectorF(layer.input.size()), 0 };
        for (i = 0; i < size || size == 0; ++i) {
            for (j = 0; j < input.data.size(); ++j) //读取一组输入
                if (!(loadFile >> input.data[j]))
                    break;
            //该组数据没有读取完整,则不加入训练集
            if (j < input.data.size() || (haveTag && !(loadFile >> input.tag)))
                break;
            testSet.push_back(input);
        }
        loadFile.close();
        cout << "\r>>成功载入" << testSet.size() << "组测试集\t" << endl;
    }
    else {
        string msg = "载入数据集文件'" + file + "失败，请检查该文件是否存在，或有权限读取\n";
        throw logic_error(msg);
    }
}

//开始训练神经网络 @param: 允许误差 0.0~1.0 ; 最大迭代次数
void ANN::train(double permitError, uint maxGens)
{
    if (trainSet.size() == 0) {
        cout << "请先载入训练数据再进行训练!" << endl;
        return;
    }
    cout << ">>>正在开始演化...\n代数\t训练正确率\t测试正确率\t平均每代耗时:ms" << endl;
    for (uint p = 0; p < annPop.size(); ++p)
        getFitValue(annPop[p]);  //计算初始化时个体的适应值
    time_t lastStart = clock(), t_start = lastStart;
    float lastFitValue = 1;
    uint lastGen = -1;
    for (uint i = 0; i < maxGens; ++i) {
        for (uint p = 0; p < annPop.size(); ++p)
            mutate(annPop[p]);
        const ANN::ANNIndividual &bestPop = getBestPop();
        if (i % 200 == 0 || clock() - lastStart > 200) {
            cout << '\r' << setiosflags(ios::left) << setw(8) << i << setw(16)
                << 1 - bestPop.fitValue << setw(16) << compareTestOut()
                << (clock() - lastStart) / (i - lastGen) << '\t';
            lastStart = clock();
            lastGen = i;
            if (lastFitValue - bestPop.fitValue > 0.008) {
                cout << endl;
                lastFitValue = bestPop.fitValue;
            }
            if (bestPop.fitValue < permitError)
                break;
        }
        if (_kbhit() && 27 == _getch() && MessageBox(0, "你按了ESC键,是否要结束演化过程?"
            , "温馨提示", MB_YESNO) == IDYES)
            break;
    }
    if (clock() - t_start > 60000)
        cout << endl << "演化过程总耗时: " << (clock() - t_start) / 60000.0 << " min" << endl;
    else
        cout << endl << "演化过程总耗时: " << (clock() - t_start) / 1000.0 << " s" << endl;
}

//根据训练好的网络,获得测试集预测结果
void ANN::getTestOut()
{
    if (testSet.size() == 0)
        return;
}

//对比测试集的输出,返回正确率. [note:前提是已知每组测试数据的输出]
float ANN::compareTestOut()
{
    if (testSet.size() == 0)
        return 0;
    float error = 0;
    const ANN::ANNIndividual &bestPop = getBestPop();
    for (uint i = 0; i < testSet.size(); ++i) {
        getANNOut(testSet[i], bestPop); //计算该组输入数据的输出
        if (layer.tag != testSet[i].tag) //对比网络预测得到的tag与原始数据中tag
            error += 1;
    }
    if (testSet.size() > 0)
        error /= testSet.size();
    return 1 - error;
}

//从训练集中随机抽取ratio比例数据作为测试集[训练集中这些数据将剔除]
void ANN::randomDivideTrainToTest(double ratio)
{
    uint testSize = (uint)(trainSet.size() * ratio); //测试集大小
    if (testSize < 1 || testSize > trainSet.size()) //如果参数范围不正确，默认划分1/4
        testSize = (int)(trainSet.size() * 0.2);
    testSet.resize(testSize);
    for (uint i = 0; i < testSize; ++i) {    //随机将部分数据移至测试集
        uint j = rand() % trainSet.size();
        testSet[i] = trainSet[j];
        trainSet.erase(trainSet.begin() + j);
    }
}

//计算某组输入数据对应的网络输出
uint ANN::getANNOut(const ANNInput& _input, const ANNIndividual& indiv)
{
    uint j, k, index;
    ///为了减少计算消耗,略去由_input.data到layer.input的复制过程
    //第一个隐层由输入数据和对应权值得到
    for (j = 0; j < layer.hide[0].size(); ++j) {
        layer.hide[0][j] = 0;
        for (k = 0; k < _input.data.size(); ++k)
            layer.hide[0][j] += _input.data[k] * indiv.weight[0][j][k];
        layer.hide[0][j] += indiv.bias[0][j];
    }
    //后面的隐层由前一个隐层和对应权值得到
    for (index = 1; index < layer.hide.size(); ++index)
        for (j = 0; j < layer.hide[index].size(); ++j) {
            layer.hide[index][j] = 0;
            for (k = 0; k < layer.hide[index - 1].size(); ++k)
                layer.hide[index][j] += layer.hide[index - 1][k] * indiv.weight[index][j][k];
            layer.hide[index][j] += indiv.bias[index][j];
        }
    //输出层由最后一个隐层和对应的权值得到
    layer.tag = 0;
    double maxOutput = -DBL_MAX;
    index = layer.hide.size() - 1; //最后一个隐层序号
    for (j = 0; j < layer.output.size(); ++j) {
        double sum = 0;
        for (k = 0; k < layer.hide[index].size(); ++k)
            sum += layer.hide[index][k] * indiv.weight[index + 1][j][k];
        sum += indiv.bias[index + 1][j];
        if (sum > maxOutput){ //softmax回归法: 标签为具有最大输出值的神经元序号
            maxOutput = sum;
            layer.tag = j;
        }
    }
    return layer.tag;
}

//计算个体的适应值
float ANN::getFitValue(ANNIndividual& indiv)
{
    indiv.fitValue = 0;
    for (uint i = 0; i < trainSet.size(); ++i) {
        getANNOut(trainSet[i], indiv); //计算该组输入数据的输出
        if (layer.tag != trainSet[i].tag) //对比网络预测得到的tag与原始训练数据中tag
            indiv.fitValue += 1;
    }
    if (trainSet.size() > 0)
        indiv.fitValue /= trainSet.size();
    return indiv.fitValue;
}

//个体变异并选择更优个体进入下一代
void ANN::mutate(ANNIndividual& _annPop)
{
    ANNIndividual tmpPop = _annPop;
    mutateByGauss(tmpPop);
    getFitValue(tmpPop);
    static float t = 0.03f; //模拟退火的初始温度
    if (tmpPop.fitValue < _annPop.fitValue)
        _annPop = tmpPop;
    else if (t > 1e-4 && t > Math_Util::randFloat()) //以一定的概率接受一个比当前解要差的解
        _annPop = tmpPop;
    t *= 0.999f;
}

//通过高斯变异方式调整参数---mutate的一种实现方式
void ANN::mutateByGauss(ANNIndividual& tmpPop)
{
    static float gaussRange = 1.2f;    //高斯变异的方差
    uint i, j, k;
    //对权重执行高斯变异
    for (i = 0; i < tmpPop.weight.size(); ++i)
        for (j = 0; j < tmpPop.weight[i].size(); ++j)
            for (k = 0; k < tmpPop.weight[i][j].size(); ++k)
                if (Math_Util::randFloat() <= F_ANN) {
                    tmpPop.weight[i][j][k] += Math_Util::randGauss(0, gaussRange);
                    //makeInRange(tmpPop.weight[i][j][k], Min_weight, Max_weight);
                }
    //对偏置参数执行高斯变异
    for (i = 0; i < tmpPop.bias.size(); ++i)
        for (j = 0; j < tmpPop.bias[i].size(); ++j)
            if (Math_Util::randFloat() <= F_ANN) {
                tmpPop.bias[i][j] += Math_Util::randGauss(0, gaussRange);
                //makeInRange(tmpPop.bias[i][j], Min_bias, Max_bias);
            }
    if (gaussRange > 0.3)
        gaussRange *= 0.9999f;
}

//返回最优个体的常引用
const ANN::ANNIndividual& ANN::getBestPop()
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
