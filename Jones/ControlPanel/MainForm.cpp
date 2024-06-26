#include "MainForm.h"
#include <windows.h>
#include "bridge.h"

using namespace System;
using namespace System::Windows::Forms;


void CheckCommunicationStatus() {

    bool isCommunicationOk = true;

    if (isCommunicationOk) {
        System::Console::WriteLine("La communication avec le bridge est établie.");
    }
    else {
        System::Console::WriteLine("Échec de la communication avec le bridge.");
    }
}

void main(array<String^>^ args) {

    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }

    System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(Console::OpenStandardOutput());
    sw->AutoFlush = true;
    Console::SetOut(sw);
    
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    ControlPanel::MainForm form;

    CheckCommunicationStatus();

    Application::Run(% form);
}


