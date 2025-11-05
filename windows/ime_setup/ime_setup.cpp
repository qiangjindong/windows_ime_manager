#include "ime_setup.h"

#include <iostream>
#include <vector>

std::vector<HKL> loadedLayouts;
HKL previousLayout = NULL;

void saveInitialKeyboardLayout()
{
    previousLayout = GetKeyboardLayout(0);
}

bool imeSetup(const std::string &methodName)
{

    std::map<std::string, std::string> languageNames = {
        {"japaneseHiraganaIme", "Japanese"},
        {"japaneseFullWidthKatakanaIme", "Japanese"},
        {"japaneseHalfWidthKatakanaIme", "Japanese"},
        {"japaneseHalfWidthAlphanumericIme", "Japanese"},
        {"englishIme", "English"},
        {"arabicIme", "Arabic"},
        {"malayalamIme", "Malayalam"},
        {"hindiIme", "Hindi"},
        {"spanishIme", "Spanish"},
        {"frenchIme", "French"},
        {"russianIme", "Russian"},
        {"chineseIme", "Chinese"},
        {"dutchIme", "Dutch"},
        {"germanIme", "German"},
        {"italianIme", "Italian"},
    };

    std::map<std::string, std::string> languageCodes = {
        {"English", "00000409"},
        {"Japanese", "00000411"},
        {"Malayalam", "0000044C"},
        {"Hindi", "00000439"},
        {"Arabic", "00000401"},
        {"Spanish", "0000040A"},
        {"French", "0000040C"},
        {"Russian", "00000419"},
        {"Chinese", "00000804"},
        {"Dutch", "00000413"},
        {"German", "00000407"},
        {"Italian", "00000410"},
    };

    std::string languageName = languageNames[methodName];
    std::string languageCode = languageCodes[languageName];

    if (previousLayout == NULL)
        saveInitialKeyboardLayout();

    std::wstring wstr = std::wstring(languageCode.begin(), languageCode.end());
    LPCWSTR lwstr = wstr.c_str();
    HKL languageIME = LoadKeyboardLayout(lwstr, KLF_ACTIVATE);
    if (languageIME == NULL)
    {
        std::cout << "Failed to load " << languageName << " IME" << std::endl;
        return false;
    }

    loadedLayouts.push_back(languageIME);

    if (!ActivateKeyboardLayout(languageIME, 0))
    {
        std::cout << "Failed to activate " << languageName << " IME" << std::endl;
        return false;
    }

    HWND hwnd = GetForegroundWindow();
    HIMC himc = ImmGetContext(hwnd);

    DWORD conversionMode = 0, sentenceMode = IME_SMODE_NONE;
    if (methodName == "japaneseHiraganaIme")
    {
        conversionMode = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE;
    }
    else if (methodName == "japaneseFullWidthKatakanaIme")
    {
        conversionMode = IME_CMODE_NATIVE | IME_CMODE_KATAKANA | IME_CMODE_FULLSHAPE;
    }
    else if (methodName == "japaneseHalfWidthKatakanaIme")
    {
        conversionMode = IME_CMODE_NATIVE | IME_CMODE_KATAKANA;
    }
    else if (methodName == "japaneseHalfWidthAlphanumericIme")
    {
        conversionMode = IME_CMODE_ALPHANUMERIC;
    }
    else
    {
        ImmGetConversionStatus(himc, &conversionMode, &sentenceMode);
        if (methodName == "englishIme")
        {
            conversionMode = IME_CMODE_ALPHANUMERIC;
        }
    }

    ImmSetConversionStatus(himc, conversionMode, sentenceMode);
    ImmSetOpenStatus(himc, true);
    ImmReleaseContext(hwnd, himc);
    return true;
}

void restoreKeyboardLayout()
{
    if (!previousLayout)
      return;

    ActivateKeyboardLayout(previousLayout, 0);

    for (HKL layout : loadedLayouts)
        UnloadKeyboardLayout(layout);
      
    loadedLayouts.clear();
}
