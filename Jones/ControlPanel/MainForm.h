#pragma once

#include "pch.h"
#include "MainForm.h"
#include <windows.h>
#include "bridge.h"

namespace ControlPanel {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	typedef bool (*SetStartProc)(bool);
	typedef bool (*GetStartProc)();
	typedef bool (*SetPauseProc)(bool);
	typedef bool (*GetPauseProc)();
	typedef bool (*SetStopProc)(bool);
	typedef bool (*GetStopProc)();
	typedef bool (*SetExitProc)(bool);
	typedef bool (*GetExitProc)();
	typedef int (*GetProgressDataProc)();
	typedef int (*GetProgressMetaDataProc)();


	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form

	{
	private:
		HMODULE hModule;
		Button^ activeButton = nullptr;

	private: System::Threading::Thread^ updateThread;

	public:
		SetStartProc SetStart = nullptr;
		GetStartProc GetStart = nullptr;
		SetPauseProc SetPause = nullptr;
		GetPauseProc GetPause = nullptr;
		SetStopProc SetStop = nullptr;
		GetStopProc GetStop = nullptr;
		SetExitProc SetExit = nullptr;
		GetExitProc GetExit = nullptr;
		GetProgressDataProc GetProgressData = nullptr;
	private: System::Windows::Forms::Label^ label_communication;

	public:

	public:

	public:

	public:

	public:
		GetProgressMetaDataProc GetProgressMetaData = nullptr;

		MainForm(void)
		{
			InitializeComponent();
			LoadBridgeFunctions();

			exitTimer = gcnew System::Windows::Forms::Timer();
			exitTimer->Interval = 3000; 
			exitTimer->Tick += gcnew System::EventHandler(this, &MainForm::OnExitTimerTick);

		}

	private: System::Windows::Forms::Timer^ exitTimer;

	private:
		void LoadBridgeFunctions()
		{
			hModule = LoadLibrary(L"bridge.dll");
			if (hModule != NULL) {
				SetStart = (SetStartProc)GetProcAddress(hModule, "SetStart");
				GetStart = (GetStartProc)GetProcAddress(hModule, "GetStart");
				SetPause = (SetPauseProc)GetProcAddress(hModule, "SetPause");
				GetPause = (GetPauseProc)GetProcAddress(hModule, "GetPause");
				SetStop= (SetStopProc)GetProcAddress(hModule, "SetStop");
				GetStop= (GetStopProc)GetProcAddress(hModule, "GetStop");
				SetExit = (SetExitProc)GetProcAddress(hModule, "SetExit");
				GetExit = (GetExitProc)GetProcAddress(hModule, "GetExit");
				GetProgressData = (GetProgressDataProc)GetProcAddress(hModule, "GetProgressData");
				GetProgressMetaData = (GetProgressMetaDataProc)GetProcAddress(hModule, "GetProgressMetaData");
			}
			else {
				MessageBox::Show("Failed to load bridge.dll", "Error", MessageBoxButtons::OK);
			}
		}

	private:
		void UpdateProgressInThread() {
			while (true) {
				this->Invoke(gcnew MethodInvoker(this, &MainForm::UpdateProgressBars));
				System::Threading::Thread::Sleep(1000);
			}
		}

	private:
		void UpdateProgressBars() {
			int currentDataProgress = GetProgressData();
			int currentMetaDataProgress = GetProgressMetaData();

			if (this->progressData->InvokeRequired || this->progressMetaData->InvokeRequired) {
				this->Invoke(gcnew MethodInvoker(this, &MainForm::UpdateProgressBars));
			}
			else {

				if (this->progressData->Value == this->progressData->Maximum) {
					this->progressData->Value = 0;
				}
				this->progressData->Value = Math::Min(currentDataProgress, progressData->Maximum);

				if (this->progressMetaData->Value == this->progressMetaData->Maximum) {
					this->progressMetaData->Value = 0;
				}
				this->progressMetaData->Value = Math::Min(currentMetaDataProgress, progressMetaData->Maximum);
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (updateThread != nullptr) {
				if (updateThread->IsAlive) {
					updateThread->Abort();
					updateThread->Join();
				}
			}
			if (hModule != NULL) {
				FreeLibrary(hModule);
			}
			if (components){
				delete components;
			}
		}

	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::ProgressBar^ progressData;
	private: System::Windows::Forms::ProgressBar^ progressMetaData;


	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Button^ buttonExit;
	private: System::Windows::Forms::Button^ buttonStop;
	private: System::Windows::Forms::Button^ buttonPause;
	private: System::Windows::Forms::Button^ buttonStart;

	private: System::Windows::Forms::PictureBox^ lightIndicator;

	private: System::ComponentModel::IContainer^ components;

	protected:

	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->progressData = (gcnew System::Windows::Forms::ProgressBar());
			this->progressMetaData = (gcnew System::Windows::Forms::ProgressBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->buttonStop = (gcnew System::Windows::Forms::Button());
			this->buttonPause = (gcnew System::Windows::Forms::Button());
			this->buttonStart = (gcnew System::Windows::Forms::Button());
			this->lightIndicator = (gcnew System::Windows::Forms::PictureBox());
			this->label_communication = (gcnew System::Windows::Forms::Label());
			this->buttonExit = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->lightIndicator))->BeginInit();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Arial", 26.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::RoyalBlue;
			this->label2->Location = System::Drawing::Point(12, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(190, 40);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Initialization";
			this->label2->Click += gcnew System::EventHandler(this, &MainForm::label2_Click);
			// 
			// progressData
			// 
			this->progressData->BackColor = System::Drawing::Color::Black;
			this->progressData->ForeColor = System::Drawing::Color::RoyalBlue;
			this->progressData->Location = System::Drawing::Point(129, 228);
			this->progressData->Name = L"progressData";
			this->progressData->Size = System::Drawing::Size(368, 23);
			this->progressData->TabIndex = 2;
			this->progressData->Click += gcnew System::EventHandler(this, &MainForm::progressData_Click);
			// 
			// progressMetaData
			// 
			this->progressMetaData->BackColor = System::Drawing::Color::Black;
			this->progressMetaData->ForeColor = System::Drawing::Color::RoyalBlue;
			this->progressMetaData->Location = System::Drawing::Point(129, 283);
			this->progressMetaData->Name = L"progressMetaData";
			this->progressMetaData->Size = System::Drawing::Size(368, 23);
			this->progressMetaData->TabIndex = 3;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::RoyalBlue;
			this->label3->Location = System::Drawing::Point(15, 228);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(55, 24);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Data";
			this->label3->Click += gcnew System::EventHandler(this, &MainForm::label3_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::Color::RoyalBlue;
			this->label4->Location = System::Drawing::Point(15, 283);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(108, 24);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Meta Data";
			// 
			// buttonStop
			// 
			this->buttonStop->BackColor = System::Drawing::Color::OrangeRed;
			this->buttonStop->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonStop->ForeColor = System::Drawing::Color::Black;
			this->buttonStop->Location = System::Drawing::Point(140, 129);
			this->buttonStop->Name = L"buttonStop";
			this->buttonStop->Size = System::Drawing::Size(115, 65);
			this->buttonStop->TabIndex = 6;
			this->buttonStop->Text = L"Stop";
			this->buttonStop->UseVisualStyleBackColor = false;
			this->buttonStop->Click += gcnew System::EventHandler(this, &MainForm::buttonStop_Click);
			this->buttonStop->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonStop_MouseDown);
			this->buttonStop->MouseEnter += gcnew System::EventHandler(this, &MainForm::buttonStop_MouseEnter);
			this->buttonStop->MouseLeave += gcnew System::EventHandler(this, &MainForm::buttonStop_MouseLeave);
			this->buttonStop->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonStop_MouseUp);
			// 
			// buttonPause
			// 
			this->buttonPause->BackColor = System::Drawing::Color::Goldenrod;
			this->buttonPause->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonPause->ForeColor = System::Drawing::Color::Black;
			this->buttonPause->Location = System::Drawing::Point(261, 129);
			this->buttonPause->Name = L"buttonPause";
			this->buttonPause->Size = System::Drawing::Size(115, 65);
			this->buttonPause->TabIndex = 7;
			this->buttonPause->Text = L"Pause";
			this->buttonPause->UseVisualStyleBackColor = false;
			this->buttonPause->Click += gcnew System::EventHandler(this, &MainForm::buttonPause_Click);
			this->buttonPause->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonPause_MouseDown);
			this->buttonPause->MouseEnter += gcnew System::EventHandler(this, &MainForm::buttonPause_MouseEnter);
			this->buttonPause->MouseLeave += gcnew System::EventHandler(this, &MainForm::buttonPause_MouseLeave);
			this->buttonPause->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonPause_MouseUp);
			// 
			// buttonStart
			// 
			this->buttonStart->BackColor = System::Drawing::Color::DarkGreen;
			this->buttonStart->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonStart->ForeColor = System::Drawing::Color::Black;
			this->buttonStart->Location = System::Drawing::Point(382, 129);
			this->buttonStart->Name = L"buttonStart";
			this->buttonStart->Size = System::Drawing::Size(115, 65);
			this->buttonStart->TabIndex = 8;
			this->buttonStart->Text = L"Start";
			this->buttonStart->UseVisualStyleBackColor = false;
			this->buttonStart->Click += gcnew System::EventHandler(this, &MainForm::buttonStart_Click);
			this->buttonStart->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonStart_MouseDown);
			this->buttonStart->MouseEnter += gcnew System::EventHandler(this, &MainForm::buttonStart_MouseEnter);
			this->buttonStart->MouseLeave += gcnew System::EventHandler(this, &MainForm::buttonStart_MouseLeave);
			this->buttonStart->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonStart_MouseUp);
			// 
			// lightIndicator
			// 
			this->lightIndicator->BackColor = System::Drawing::Color::Transparent;
			this->lightIndicator->Location = System::Drawing::Point(167, 81);
			this->lightIndicator->Name = L"lightIndicator";
			this->lightIndicator->Size = System::Drawing::Size(20, 20);
			this->lightIndicator->TabIndex = 9;
			this->lightIndicator->TabStop = false;
			// 
			// label_communication
			// 
			this->label_communication->AutoSize = true;
			this->label_communication->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_communication->ForeColor = System::Drawing::Color::RoyalBlue;
			this->label_communication->Location = System::Drawing::Point(15, 81);
			this->label_communication->Name = L"label_communication";
			this->label_communication->Size = System::Drawing::Size(153, 24);
			this->label_communication->TabIndex = 10;
			this->label_communication->Text = L"Communication";
			this->label_communication->Click += gcnew System::EventHandler(this, &MainForm::label1_Click_2);
			// 
			// buttonExit
			// 
			this->buttonExit->BackColor = System::Drawing::Color::Firebrick;
			this->buttonExit->Font = (gcnew System::Drawing::Font(L"Arial", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->buttonExit->ForeColor = System::Drawing::Color::Black;
			this->buttonExit->Location = System::Drawing::Point(19, 129);
			this->buttonExit->Name = L"buttonExit";
			this->buttonExit->Size = System::Drawing::Size(115, 65);
			this->buttonExit->TabIndex = 11;
			this->buttonExit->Text = L"Exit";
			this->buttonExit->UseVisualStyleBackColor = false;
			this->buttonExit->Click += gcnew System::EventHandler(this, &MainForm::buttonExit_Click);
			this->buttonExit->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonExit_MouseDown);
			this->buttonExit->MouseEnter += gcnew System::EventHandler(this, &MainForm::buttonExit_MouseEnter);
			this->buttonExit->MouseLeave += gcnew System::EventHandler(this, &MainForm::buttonExit_MouseLeave);
			this->buttonExit->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::buttonExit_MouseUp);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 14);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->ClientSize = System::Drawing::Size(784, 361);
			this->Controls->Add(this->buttonExit);
			this->Controls->Add(this->label_communication);
			this->Controls->Add(this->buttonStart);
			this->Controls->Add(this->buttonPause);
			this->Controls->Add(this->buttonStop);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->progressMetaData);
			this->Controls->Add(this->progressData);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->lightIndicator);
			this->Font = (gcnew System::Drawing::Font(L"Arial", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(254)));
			this->ForeColor = System::Drawing::Color::MidnightBlue;
			this->MinimumSize = System::Drawing::Size(800, 400);
			this->Name = L"MainForm";
			this->Text = L"Control Panel";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->lightIndicator))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label1_Click_1(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label2_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void label3_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
		//this->timer1->Start();
		if (this->IsHandleCreated) {
			updateThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &MainForm::UpdateProgressInThread));
			updateThread->Start();
		}
		else {
			MessageBox::Show("Error: Handle not created.");
		}
		// Vérifier si le module du bridge est chargé
		if (this->hModule != NULL) {
			this->lightIndicator->BackColor = System::Drawing::Color::Green;
		}
		else {
			this->lightIndicator->BackColor = System::Drawing::Color::Red;
		}
	}

	private: System::Void buttonExit_Click(System::Object^ sender, System::EventArgs^ e) {
		if (SetExit) {
			bool ExitConfirmation = SetExit(true);
			while (GetExit() == false && ExitConfirmation == false) {
				System::Threading::Thread::Sleep(100);
			}
			exitTimer->Start();
		}
		buttonExit->Enabled = false;
		buttonStop->Enabled = false;
		buttonPause->Enabled = true;
		buttonStart->Enabled = true;
		System::Console::WriteLine("GetStop returned : " + GetStop());
		System::Console::WriteLine("GetExit returned : " + GetExit());
	}

	private: System::Void buttonStop_Click(System::Object^ sender, System::EventArgs^ e) {
		if (SetStop) {
			bool StopConfirmation = SetStop(true);
			while (GetStop() == false && StopConfirmation == false) {
				System::Threading::Thread::Sleep(100);
			}
		}
		buttonExit->Enabled = true;
		buttonStop->Enabled = false;
		buttonPause->Enabled = true;
		buttonStart->Enabled = true;
		System::Console::WriteLine("GetStop returned : " + GetStop());
	}

	private: System::Void buttonPause_Click(System::Object^ sender, System::EventArgs^ e) {
		if (SetPause) {
			bool PauseConfirmation = SetPause(true);
			while (GetPause() == false && PauseConfirmation == false) {
				System::Threading::Thread::Sleep(100);
			}
		}
		buttonExit->Enabled = true;
		buttonStop->Enabled = true;
		buttonPause->Enabled = false;
		buttonStart->Enabled = true;
		System::Console::WriteLine("GetPause returned : " + GetPause());
	}

	private: System::Void buttonStart_Click(System::Object^ sender, System::EventArgs^ e) {
		if (SetStart) {
			bool StartConfirmation = SetStart(true);
			while (GetStart() == false && StartConfirmation == false) {
				System::Threading::Thread::Sleep(100);
			}
		}
		buttonExit->Enabled = true;
		buttonStop->Enabled = true;
		buttonPause->Enabled = true;
		buttonStart->Enabled = false;
		System::Console::WriteLine("GetStart returned : " + GetStart());
	}

	private: System::Void OnExitTimerTick(System::Object^ sender, System::EventArgs^ e) {
		exitTimer->Stop();

		if (GetExit() == true) {
			Application::Exit();
		}
		else {
			
		}
	}

	private: System::Void eventLog1_EntryWritten(System::Object^ sender, System::Diagnostics::EntryWrittenEventArgs^ e) {
	}

    private:
		System::Void buttonExit_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = DarkenColor(button->BackColor, 0.1f);
			}
		}

		System::Void buttonExit_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = OriginalColor(button->Name);
			}
		}

		System::Void buttonExit_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			button->BackColor = DarkenColor(button->BackColor, 0.2f);
		}

		System::Void buttonExit_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			if (activeButton != nullptr) {
				activeButton->BackColor = OriginalColor(activeButton->Name);
			}
			Button^ button = dynamic_cast<Button^>(sender);
			activeButton = button;
			activeButton->BackColor = DarkenColor(button->BackColor, 0.1f);
		}
		
		System::Void buttonStop_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
            Button^ button = dynamic_cast<Button^>(sender);
            if (button != activeButton) {
                button->BackColor = DarkenColor(button->BackColor, 0.1f);
            }
        }

        System::Void buttonStop_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
            Button^ button = dynamic_cast<Button^>(sender);
            if (button != activeButton) {
                button->BackColor = OriginalColor(button->Name);
            }
        }

        System::Void buttonStop_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
            Button^ button = dynamic_cast<Button^>(sender);
            button->BackColor = DarkenColor(button->BackColor, 0.2f);
        }

        System::Void buttonStop_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
            if (activeButton != nullptr) {
                activeButton->BackColor = OriginalColor(activeButton->Name);
            }
            Button^ button = dynamic_cast<Button^>(sender);
            activeButton = button;
            activeButton->BackColor = DarkenColor(button->BackColor, 0.1f);
        }

		System::Void buttonStart_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = DarkenColor(button->BackColor, 0.1f);
			}
		}

		System::Void buttonStart_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = OriginalColor(button->Name);
			}
		}

		System::Void buttonStart_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			button->BackColor = DarkenColor(button->BackColor, 0.2f);
		}

		System::Void buttonStart_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			if (activeButton != nullptr) {
				activeButton->BackColor = OriginalColor(activeButton->Name);
			}
			Button^ button = dynamic_cast<Button^>(sender);
			activeButton = button;
			activeButton->BackColor = DarkenColor(button->BackColor, 0.1f);
		}

		System::Void buttonPause_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = DarkenColor(button->BackColor, 0.1f);
			}
		}

		System::Void buttonPause_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			if (button != activeButton) {
				button->BackColor = OriginalColor(button->Name);
			}
		}

		System::Void buttonPause_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			Button^ button = dynamic_cast<Button^>(sender);
			button->BackColor = DarkenColor(button->BackColor, 0.2f);
		}

		System::Void buttonPause_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			if (activeButton != nullptr) {
				activeButton->BackColor = OriginalColor(activeButton->Name);
			}
			Button^ button = dynamic_cast<Button^>(sender);
			activeButton = button;
			activeButton->BackColor = DarkenColor(button->BackColor, 0.1f);
		}

        System::Drawing::Color DarkenColor(System::Drawing::Color color, float percentage) {
            int r = max(0, (int)(color.R * (1 - percentage)));
            int g = max(0, (int)(color.G * (1 - percentage)));
            int b = max(0, (int)(color.B * (1 - percentage)));
            return System::Drawing::Color::FromArgb(color.A, r, g, b);
        }

        System::Drawing::Color OriginalColor(String^ buttonName) {
            if (buttonName == "buttonExit") return System::Drawing::Color::Firebrick;
			else if (buttonName == "buttonStop") return System::Drawing::Color::OrangeRed;
            else if (buttonName == "buttonPause") return System::Drawing::Color::Goldenrod;
            else if (buttonName == "buttonStart") return System::Drawing::Color::DarkGreen;
            return System::Drawing::SystemColors::Control;
        }

private: System::Void label1_Click_2(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void progressData_Click(System::Object^ sender, System::EventArgs^ e) {
}
};
}