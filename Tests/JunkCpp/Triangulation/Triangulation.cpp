using namespace System;
using namespace System::Windows::Forms;

#include "MyForm.h"

[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	// gcnew [1]�ŕt�����v���W�F�N�g��::[2]�ŕt����Form��()
	Application::Run(gcnew Triangulation::MyForm());
	return 0;
}
