#include "LogUtils.h"
#include <iostream>

void LogUtils::PrintHRResult(HRESULT res)
{
    if (FAILED(res)) {
        std::string message = std::system_category().message(res);
        std::cout << message << "\n";
    }
    else {
        std::cout << "Operation is successful. Don't worry! \n";
    }
}
    
