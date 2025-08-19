// wavConcentrator.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<string>
#include<cstring>
#include "AudioFile.h"
#include<vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstdio>
std::vector<float> resampleAudio(const std::vector<float>& input, double oldRate, double newRate);

void prologue()
{
	#ifdef _WIN32
		system("chcp 65001");
	#endif 
}


int main(int argc, char** argv)
{
    prologue();

   int sampleRate = 44100;
    std::string inputfileName;
    std::string outputFileName = "output.wav";

    bool foundInput = false;
    for (int i = 0; i < argc - 1; ++i)
    {
        if (std::strcmp(argv[i], "-f") == 0)
        {
            inputfileName = argv[i + 1];
            foundInput = true;
        }
        else if (std::strcmp(argv[i], "-o") == 0)
        {
            outputFileName = argv[i + 1];
        }
    }

    if (!foundInput)
    {
        std::cout << "Usage: wavConcentrator -f <list.txt> -o <output.wav>" << std::endl;
        return 1;
    }

    std::printf("Input list: %s\n", inputfileName.c_str());
    std::printf("Output file: %s\n", outputFileName.c_str());

    std::ifstream file(inputfileName);
    std::vector<std::string> fileNames;
    std::string line;

    if (!file.is_open())
    {
        std::cout << "X Failed to open file: " << inputfileName << std::endl;
        return 1;
    }

    while (std::getline(file, line))
    {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\n\r");
        fileNames.push_back(line.substr(start, end - start + 1));
    }
    file.close();
    if (fileNames.empty())
    {
        std::printf("what are your input files?\n");
        return -1;
    }
    // Prepare output buffer
    AudioFile<float>::AudioBuffer outputBuffer;
    outputBuffer.resize(2); // Stereo
    AudioFile<float> getSampleRateAudioFile;
    if (!getSampleRateAudioFile.load(fileNames[0]))
    {
        std::printf("failed to get sample rate. Sample rate = %d instead",sampleRate);
    }
    else
    {
        sampleRate = getSampleRateAudioFile.getSampleRate();
        std::printf("Sample rate = %d\n", sampleRate);
    }
    for (const std::string& fname : fileNames)
    {
        std::printf("Loading: %s\n", fname.c_str());

        AudioFile<float> af;

       

        std::vector<float> leftSamples, rightSamples;
        if (!af.load(fname))
        {
            std::printf("X Failed to load: %s\n", fname.c_str());
            continue;
        }
        int originalRate = af.getSampleRate();
        // Optional: Check sample rate
        if (af.getSampleRate() != sampleRate)
        {
            std::printf("! Warning: %s has sample rate %d\n. Resampling.", fname.c_str(), af.getSampleRate());
        }

        if (af.isMono())
        {
            std::printf("  -> Mono file, duplicating to both channels.\n");
            std::vector<float> monoData = af.samples[0];
            std::vector<float> resampled = resampleAudio(monoData, originalRate, sampleRate);
            leftSamples = rightSamples = resampled;
            //std::copy(af.samples[0].begin(), af.samples[0].end(), std::back_inserter(outputBuffer[0]));
            //std::copy(af.samples[0].begin(), af.samples[0].end(), std::back_inserter(outputBuffer[1]));
        }
        else if (af.isStereo())
        {
            std::printf("  -> Stereo file, copying L/R channels.\n");
            std::vector<float> leftResampled = resampleAudio(af.samples[0], originalRate, sampleRate);
            std::vector<float> rightResampled = resampleAudio(af.samples[1], originalRate, sampleRate);
            leftSamples = leftResampled;
            rightSamples = rightResampled;
        }
        else
        {
            std::printf("Unsupported channel format: %d\n", af.getNumChannels());
        }
        outputBuffer[0].insert(outputBuffer[0].end(), leftSamples.begin(), leftSamples.end());
        outputBuffer[1].insert(outputBuffer[1].end(), rightSamples.begin(), rightSamples.end());
        // Optional: Print buffer size in MiB
        size_t numSamples = outputBuffer[0].size();
        double memMiB = (numSamples * sizeof(float) * 2) / (1024.0 * 1024.0);
        std::printf("Current buffer size: %.2f MiB (%zu samples)\n", memMiB, numSamples);
    }

    // Save result
    AudioFile<float> result;
    result.setAudioBuffer(outputBuffer);
    result.setSampleRate(sampleRate);

    if (result.save(outputFileName, AudioFileFormat::Wave))
    {
        std::printf("Successfully saved to %s\n", outputFileName.c_str());
    }
    else
    {
        std::printf("X Failed to save %s\n", outputFileName.c_str());
        return 1;
    }

    return 0;
}


/**
 * @brief 对单通道音频进行重采样（线性插值）
 *
 * @param input 输入音频样本（float 向量）
 * @param oldRate 原始采样率（如 48000）
 * @param newRate 目标采样率（如 44100）
 * @return std::vector<float> 重采样后的音频数据
 */
std::vector<float> resampleAudio(const std::vector<float>& input, double oldRate, double newRate)
{
    if (std::abs(oldRate - newRate) < 1.0)
    {
        return input;
    }

    int inputSize = static_cast<int>(input.size());
    double duration = static_cast<double>(inputSize) / oldRate;
    int outputSize = static_cast<int>(duration * newRate + 0.5);

    std::vector<float> output;
    output.reserve(outputSize);

    for (int i = 0; i < outputSize; ++i)
    {
        double time = static_cast<double>(i) / newRate;
        double inputIndex = time * oldRate;

        int index0 = static_cast<int>(std::floor(inputIndex));
        int index1 = index0 + 1;

        if (index0 < 0) {
            output.push_back(input[0]);
        }
        else if (index0 >= inputSize - 1) {
            output.push_back(input.back());
        }
        else if (index1 >= inputSize) {
            output.push_back(input[index0]);
        }
        else {
            double frac = inputIndex - index0;
            float sample = input[index0] * (1.0f - frac) + input[index1] * frac;
            output.push_back(sample);
        }
    }

    return output;
}