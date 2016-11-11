#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Drawing;

#include <vector>
#include "../../../JunkCpp/Triangulation.h"

void TestTriangulation(std::vector<int>& indices);

namespace Triangulation {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm �̊T�v
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form {
	public:
		MyForm(void) {
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^�[ �R�[�h��ǉ����܂�
			//
		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~MyForm() {
			if (components) {
				delete components;
			}
		}

	private:
		System::Windows::Forms::Button^  button1;

	protected:
		List<PointF>^ _Lines = gcnew List<PointF>();
		List<PointF>^ _Polygon = nullptr;
		List<int>^ _Triangles = nullptr;

		virtual void OnMouseDown(System::Windows::Forms::MouseEventArgs^ e) override {
			if (e->Button == System::Windows::Forms::MouseButtons::Left) {
				_Polygon = nullptr;
				_Triangles = nullptr;

				_Lines->Add(PointF((float)e->X, (float)e->Y));
				this->Invalidate();
			}
		}

		virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override {
			auto g = e->Graphics;
			Pen pen1(Color::FromArgb(0, 0, 0), 1);
			Pen pen2(Color::FromArgb(255, 0, 0), 1);
			Pen penTri(Color::FromArgb(0, 0, 255), 1);
			SolidBrush brsTri(Color::FromArgb(127, 127, 255));

			if (_Polygon != nullptr) {
				auto a = _Polygon->ToArray();
				g->DrawPolygon(%pen1, a);
				for (int i = 0; i < _Triangles->Count; i += 3) {
					auto tri = gcnew array<PointF> { a[_Triangles[i]], a[_Triangles[i + 1]], a[_Triangles[i + 2]] };
					g->FillPolygon(
						%brsTri,
						tri
					);
					g->DrawPolygon(
						%penTri,
						tri
					);
				}
				for each(auto p in _Polygon) {
					g->DrawRectangle(%pen2, Rectangle((int)(p.X - 2), (int)(p.Y - 2), 4, 4));
				}
			} else {
				if (2 <= _Lines->Count) {
					g->DrawLines(%pen1, _Lines->ToArray());
				}
				for each(auto p in _Lines) {
					g->DrawRectangle(%pen2, Rectangle((int)(p.X - 2), (int)(p.Y - 2), 4, 4));
				}
			}
		}

		//virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs^ pevent) override {
		//}

		System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			if (_Lines->Count < 3)
				return;

			std::vector<jk::Vector2f> points;
			std::vector<int> indices;

			// ���_���W _Lines ���� vector �ɓ����
			for each(auto p in _Lines) {
				points.push_back(jk::Vector2f(p.X, p.Y));
			}

			// �O�p�`�������s��
			jk::Triangulation<jk::Vector2f, jk::Vector2f, jk::ExtractVector2FromVectorN<jk::Vector2f>> t;
			t.Do(&points[0], points.size(), indices);

			// ���_���W _Lines ���� _Polygon ���쐬
			_Polygon = _Lines;
			_Lines = gcnew List<PointF>();

			// �O�p�`�C���f�b�N�X�ԍ� vector ���� _Triangles �ɓ����
			_Triangles = gcnew List<int>();
			for (auto i : indices) {
				_Triangles->Add(i);
			}

			this->Invalidate();
		}

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�[�ϐ��ł��B
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i�[ �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�[�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void) {
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(88, 37);
			this->button1->TabIndex = 0;
			this->button1->Text = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(666, 409);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
