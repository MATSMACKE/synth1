#include <iostream>
#include <map>
#include <Windows.h>
#include "soundMachine.h"
#include <cstdlib>

using namespace std;

double volume = 0.05;

atomic<double> frequency = 0.0;

void displayKeyboard() {
    // Display a keyboard
    wcout << endl <<
        "|   |   |   |   |   | |   |   |   |   | |   | |   |   |" << endl <<
        "|   | W |   |   | R | | T |   |   | U | | I | | O |   |" << endl <<
        "|   |___|   |   |___| |___|   |   |___| |___| |___|   |" << endl <<
        "|     |     |     |     |     |     |     |     |     |" << endl <<
        "|  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |" << endl <<
        "|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl << "X -> Octave Up" << endl << "Z -> Octave Down" << endl << endl;
}

double ProduceSquareWave(double dTime) {
    double sineWave = sin(frequency * 2 * 3.14159265 * dTime);

    if (sineWave > 0) {
        return volume;
    }
    else {
        return -1* volume;
    }
}

double ProduceSineWave(double dTime) {
    return volume * sin(frequency * 2 * 3.14159265 * dTime);
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

int main() {
    //Get sound hardware
    vector<wstring> devices = soundMachine<short>::Enumerate();

    int deviceNumber = 0;

    int selectedDevice;

    //Display sound hardware
    for (auto d : devices) {
        wcout << deviceNumber << ") Found output device: " << d << "\n";
        ++deviceNumber;
    }

    //Ask user to select sound drivr
    wcout << "\nSelect driver: ";
    cin >> selectedDevice;

    //Create soundMachine
    soundMachine<short> sound(devices[selectedDevice], 44100, 1, 8, 512);

    //Set volume
    cout << "Volume (between 0 and 1): ";
    cin >> volume;

    //Choose waveform
    cout << "Waveform (s = sin, q = square): ";
    char waveform;
    cin >> waveform;

    //Link noise function to soundMachine
    switch (waveform) {
        case 's':
            sound.SetUserFunction(ProduceSineWave);
            break;
        case 'q':
            sound.SetUserFunction(ProduceSquareWave);
            break;
        default:
            cout << waveform << "is not a waveform.";
    }

    bool running = true;

    double frequencyMultiplier = 1;

    bool XKeyState = false;
    bool ZKeyState = false;

    system("CLS");

    displayKeyboard();

    char keysArray[16] = {'A', 'W', 'S', 'D', 'R', 'F', 'T', 'G', 'H', 'U', 'J', 'I', 'K', 'O', 'L'};

    double keyToFreq[16] = {440.0, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77};

    int currentOctave = 4;

    ShowConsoleCursor(false);

    while (running) {/*
        if (GetAsyncKeyState('A') & 0x8000) {
            frequency = 440.0 * frequencyMultiplier;
        }
        else {
            frequency = 0.0;
        }*/
        bool keyPressed = false;
        for (int i = 0; i < 16; i++) {
            if ((GetAsyncKeyState(keysArray[i]) & 0x8000) && keyPressed == false) {
                frequency = keyToFreq[i] * frequencyMultiplier;
                keyPressed = true;
            }
        }
        if (keyPressed == false) {
            frequency = 0;
        }

        if (GetAsyncKeyState('X') & 0x8000) {
            if (XKeyState == false) {
                frequencyMultiplier *= 2.0;
                currentOctave += 1;
            }
            XKeyState = true;
        }
        else {
            XKeyState = false;
        }
        if (GetAsyncKeyState('Z') & 0x8000) {
            if (ZKeyState == false) {
                frequencyMultiplier /= 2.0;
                currentOctave -= 1;
            }
            ZKeyState = true;
        }
        else {
            ZKeyState = false;
        }
        if (GetAsyncKeyState('Q') & 0x8000) {
            running = false;
        }

        //Give Octave Feedback
        cout << "\rCurrent octave is: " << currentOctave;

        Sleep(2);
    }

    system("pause");

    return 0;
}