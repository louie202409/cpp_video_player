#include <bits/stdc++.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

class AudioPlayer {
private:
    HWAVEOUT hWaveOut;
    WAVEFORMATEX wfx;
    bool initialized;
    double volume;
    bool forceSquare;           // 强制使用方波（若启用）
    int autoSquareThreshold;     // 自动方波阈值（Hz），低于此值用方波，否则正弦波
    bool autoSquareEnabled;      // 是否启用自动方波

public:
    AudioPlayer() : hWaveOut(NULL), initialized(false), volume(1.0),
                    forceSquare(false), autoSquareThreshold(150),
                    autoSquareEnabled(false) {}
    ~AudioPlayer() { close(); }

    bool init(int sampleRate = 44100, int bitsPerSample = 16, int channels = 1) {
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = channels;
        wfx.nSamplesPerSec = sampleRate;
        wfx.wBitsPerSample = bitsPerSample;
        wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        wfx.cbSize = 0;

        MMRESULT res = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
        if (res != MMSYSERR_NOERROR) return false;
        initialized = true;
        return true;
    }

    void close() {
        if (initialized) {
            waveOutClose(hWaveOut);
            initialized = false;
        }
    }

    // 设置全局音量 (0.0 ~ 1.0)
    void setVolume(double vol) { volume = vol; }

    // 强制使用方波（true=方波, false=正弦波），此设置优先于自动阈值
    void setWaveform(bool square) { forceSquare = square; }

    // 启用自动低频方波：频率低于 threshold Hz 时自动使用方波
    void setAutoSquareBelow(int threshold) {
        autoSquareThreshold = threshold;
        autoSquareEnabled = true;
    }

    // 禁用自动方波，恢复为全局 forceSquare 设置
    void disableAutoSquare() { autoSquareEnabled = false; }

    // 播放音符（同步）
    bool playNote(int freq, int durationMs) {
        if (!initialized) return false;
        if (freq <= 0) {
            Sleep(durationMs);
            return true;
        }

        // 决定使用何种波形
        bool useSquare = forceSquare;                      // 强制方波优先
        if (!useSquare && autoSquareEnabled && freq < autoSquareThreshold) {
            useSquare = true;                               // 自动方波触发
        }

        int bytesPerSample = wfx.wBitsPerSample / 8;
        int samples = wfx.nSamplesPerSec * durationMs / 1000;
        if (samples == 0) return true;

        vector<char> buffer(samples * bytesPerSample);

        if (wfx.wBitsPerSample == 16) {
            short* data = reinterpret_cast<short*>(&buffer[0]);
            int maxAmp = (int)(32767 * volume);
            double period = (double)wfx.nSamplesPerSec / freq; // 周期（采样数）

            for (int i = 0; i < samples; ++i) {
                double value;
                if (useSquare) {
                    double phase = fmod(i, period) / period;
                    value = (phase < 0.5) ? 1.0 : -1.0;
                } else {
                    double t = (double)i / wfx.nSamplesPerSec;
                    value = sin(2 * 3.14159265358979323846 * freq * t);
                }
                data[i] = (short)(maxAmp * value);
            }
        } else if (wfx.wBitsPerSample == 8) {
            unsigned char* data = reinterpret_cast<unsigned char*>(&buffer[0]);
            int center = 128;
            int maxAmp = (int)(127 * volume);
            double period = (double)wfx.nSamplesPerSec / freq;

            for (int i = 0; i < samples; ++i) {
                double value;
                if (useSquare) {
                    double phase = fmod(i, period) / period;
                    value = (phase < 0.5) ? 1.0 : -1.0;
                } else {
                    double t = (double)i / wfx.nSamplesPerSec;
                    value = sin(2 * 3.14159265358979323846 * freq * t);
                }
                data[i] = (unsigned char)(center + maxAmp * value);
            }
        } else {
            return false;
        }

        WAVEHDR wh = {0};
        wh.lpData = &buffer[0];
        wh.dwBufferLength = buffer.size();
        wh.dwFlags = 0;

        MMRESULT res = waveOutPrepareHeader(hWaveOut, &wh, sizeof(wh));
        if (res != MMSYSERR_NOERROR) return false;

        res = waveOutWrite(hWaveOut, &wh, sizeof(wh));
        if (res != MMSYSERR_NOERROR) {
            waveOutUnprepareHeader(hWaveOut, &wh, sizeof(wh));
            return false;
        }

        while (!(wh.dwFlags & WHDR_DONE)) Sleep(1);

        waveOutUnprepareHeader(hWaveOut, &wh, sizeof(wh));
        return true;
    }
};

// ---------- 乐谱数据 ----------
int play[5000][2];
int freq[5][11];

int main() {
    // 频率表初始化（浮点计算后取整）
    double tempFreq[4][11];
    tempFreq[3][1]=523.0; tempFreq[3][2]=587.0; tempFreq[3][3]=659.0; tempFreq[3][4]=698.0;
    tempFreq[3][5]=784.0; tempFreq[3][6]=880.0; tempFreq[3][7]=987.0; tempFreq[3][8]=830.0; tempFreq[3][9]=880.0;
    for(int i=2;i>=0;i--) {
        for(int j=1;j<=9;j++) {
            tempFreq[i][j] = tempFreq[i+1][j] / 2.0;
        }
    }
	for(int i=3;i>=0;i--) {
        for(int j=1;j<=9;j++) {
            freq[i][j] = (int)(tempFreq[i][j] + 0.5);
        }
    }
    // 读取乐谱
    ifstream in("notess.txt");
    for(int i=0;i<1984;i++) {
        int a,b;
        in>>a>>b;
        play[i][0]=a+2;
        play[i][1]=b;
    }
    in.close();

    // 创建播放器
    AudioPlayer player;
    if (!player.init(44100, 16, 1)) {
        cout << "音频设备初始化失败！" << endl;
        return 1;
    }

    player.setVolume(1.0);          // 最大音量
    player.setAutoSquareBelow(1000); // 低于 200Hz 自动用方波（可根据需要调整）

    cout<<"Bad apple!!!"<< endl;
    cout<<"Code by deepseek and me. Notes.txt by me."<<endl;
	cout<<"This is 150 bpm. Original is 134 bpm."<<endl;
	cout<<"Original video: https://www.bilibili.com/video/BV1x5411o7Kn/"<<endl;

    // 主旋律
    for (int i = 0; i < 1984; ++i) {
        int noteFreq = freq[play[i][0]][play[i][1]];
        player.playNote(noteFreq, 100);
    }

    return 0;
}
