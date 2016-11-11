using namespace System;
using namespace System::Windows::Forms;

#include "MyForm.h"

[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	// gcnew [1]で付けたプロジェクト名::[2]で付けたForm名()
	Application::Run(gcnew Triangulation::MyForm());
	return 0;
}
