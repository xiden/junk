#include "stdafx.h"
#include "Csv.h"

//! �w���؂蕶���Ńt�B�[���h��؂蕪���Ď擾����
void Csv::Split(
	const CStringA& text, //!< [in] �؂�o������������
	char separator, //!< [in] ��؂蕶��
	char bundler, //!< [in] ���蕶��
	std::vector<CStringA>& fields //!< [out] ������̃t�B�[���h�z�񂪕Ԃ�
) {
	fields.resize(0);

	if(text.IsEmpty())
		return;

	// �}���`�o�C�g�̂܂܂��ƂP�����������o�C�g�ɂȂ蓾��̂ŏ��������
	// ���C�h�����ɕϊ�����
	CStringW textW(text);

	const wchar_t* p = textW; // ��Ɨp�|�C���^
	const wchar_t* pFieldStart; // �t�B�[���h�̊J�n�ʒu
	const wchar_t* pFieldEnd; // �t�B�[���h�̏I�[�ʒu+1(��؂蕶���܂��͏I�[�̈ʒu)
	const wchar_t* pTextEnd = p + textW.GetLength(); // ������
	std::vector<wchar_t> field; // �t�B�[���h�����z��

	// �����񂩂�t�B�[���h�����o���Ă���
	while (p <= pTextEnd) {
		// �t�B�[���h�̐擪��T��
		while (p < pTextEnd && (*p == L' ')) {
			p++;
		}

		// �_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
		wchar_t sp2 = separator;
		if (bundler != 0 && p < pTextEnd && p[0] == bundler) {
			sp2 = bundler;
			p++;
		}

		// ���̎��_�Ńt�B�[���h�̐擪���m��
		pFieldStart = p;

		// ��؂蕶����T���t�B�[���h�̏I�[���m�肷��
		field.resize(0);
		pFieldEnd = pFieldStart;
		while(pFieldEnd < pTextEnd) {
			int move = 1;
			if (pFieldEnd[0] == sp2) {
				if (separator != sp2 && pFieldEnd[1] == sp2)
					move = 2; // �_�u���N�H�[�e�[�V�������Q����ł���Ȃ炻��͏I�[�ł͂Ȃ�
				else
					break;
			}
			field.push_back(*pFieldEnd);
			pFieldEnd += move;
		}

		// �t�B�[���h�z��Ƀt�B�[���h��ǉ�
		if (field.empty())
			fields.push_back("");
		else
			fields.push_back(CStringA(&field[0], field.size()));

		// ��؂蕶�����I�[�o�[���[�h����Ă����ꍇ�ɂ͌��̋�؂蕶���܂Ői�߂�
		if (separator != sp2) {
			while(pFieldEnd < pTextEnd) {
				if(*pFieldEnd == separator)
					break;
				pFieldEnd++;
			}
		}

		// �t�B�[���h�I�[��������̏I�[�Ɉ�v���Ă���Ȃ�S�Đ؂�o������
		if (pFieldEnd == pTextEnd)
			break;

		// ���t�B�[���h�̌����J�n�ʒu�ֈړ�
		p = pFieldEnd + 1;
	}
}

//! �w���؂蕶���Ńt�B�[���h��؂蕪���Ď擾����
void Csv::Split(
	const CStringW& text, //!< [in] �؂�o������������
	wchar_t separator, //!< [in] ��؂蕶��
	wchar_t bundler, //!< [in] ���蕶��
	std::vector<CStringW>& fields //!< [out] ������̃t�B�[���h�z�񂪕Ԃ�
) {
	fields.resize(0);

	if(text.IsEmpty())
		return;

	const wchar_t* p = text; // ��Ɨp�|�C���^
	const wchar_t* pFieldStart; // �t�B�[���h�̊J�n�ʒu
	const wchar_t* pFieldEnd; // �t�B�[���h�̏I�[�ʒu+1(��؂蕶���܂��͏I�[�̈ʒu)
	const wchar_t* pTextEnd = p + text.GetLength(); // ������
	std::vector<wchar_t> field; // �t�B�[���h�����z��

	// �����񂩂�t�B�[���h�����o���Ă���
	while (p <= pTextEnd) {
		// �t�B�[���h�̐擪��T��
		while (p < pTextEnd && (*p == L' ')) {
			p++;
		}

		// �_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
		wchar_t sp2 = separator;
		if (bundler != 0 && p < pTextEnd && p[0] == bundler) {
			sp2 = bundler;
			p++;
		}

		// ���̎��_�Ńt�B�[���h�̐擪���m��
		pFieldStart = p;

		// ��؂蕶����T���t�B�[���h�̏I�[���m�肷��
		field.resize(0);
		pFieldEnd = pFieldStart;
		while(pFieldEnd < pTextEnd) {
			int move = 1;
			if (pFieldEnd[0] == sp2) {
				if (separator != sp2 && pFieldEnd[1] == sp2)
					move = 2; // �_�u���N�H�[�e�[�V�������Q����ł���Ȃ炻��͏I�[�ł͂Ȃ�
				else
					break;
			}
			field.push_back(*pFieldEnd);
			pFieldEnd += move;
		}

		// �t�B�[���h�z��Ƀt�B�[���h��ǉ�
		if (field.empty())
			fields.push_back(L"");
		else
			fields.push_back(CStringW(&field[0], field.size()));

		// ��؂蕶�����I�[�o�[���[�h����Ă����ꍇ�ɂ͌��̋�؂蕶���܂Ői�߂�
		if (separator != sp2) {
			while(pFieldEnd < pTextEnd) {
				if(*pFieldEnd == separator)
					break;
				pFieldEnd++;
			}
		}

		// �t�B�[���h�I�[��������̏I�[�Ɉ�v���Ă���Ȃ�S�Đ؂�o������
		if (pFieldEnd == pTextEnd)
			break;

		// ���t�B�[���h�̌����J�n�ʒu�ֈړ�
		p = pFieldEnd + 1;
	}
}

//! �w���؂蕶���Ńt�B�[���h��A�����Ď擾����
void Csv::Combine(
	const std::vector<CStringA>& fields, //!< [in] ���ʂ̕������ꂽ�����񂪒ǉ������z��
	wchar_t separator, //!< [in] ��؂蕶��
	wchar_t bundler, //!< [in] ���蕶��
	CStringA& text //!< [out] �A����̕�����
) {
	char spchars[3], rep1[2], rep2[3];
	CStringA s2;

	if (bundler == 0) {
		spchars[0] = (char)separator;
		spchars[1] = 0;
	} else {
		spchars[0] = (char)separator;
		spchars[1] = (char)bundler;
		spchars[2] = 0;

		rep1[0] = (char)bundler;
		rep1[1] = 0;
		rep2[0] = (char)bundler;
		rep2[1] = (char)bundler;
		rep2[2] = 0;
	}

	text.Empty();
	for (int i = 0; i < (int)fields.size(); i++) {
		if (i != 0)
			text += separator;

		const CStringA& s = fields[i];
		if (!s.IsEmpty()) {
			if (bundler == 0 || s.FindOneOf(spchars) < 0) {
				// ���蕶���w�肪�����܂��̓t�B�[���h���ɋ�؂蕶�����܂܂�Ă��Ȃ��ꍇ�ɂ͊���Ȃ�
				text += s;
			} else {
				// ���蕶���w�肪���芎�t�B�[���h���ɋ�؂蕶�����܂܂�Ă���ꍇ�ɂ͊���
				// ���̍ۊ��蕶�����Q�A�ɒu��������
				s2 = s;
				s2.Replace(rep1, rep2);
				text += bundler;
				text += s2;
				text += bundler;
			}
		}
	}
}

//! �w���؂蕶���Ńt�B�[���h��A�����Ď擾����
void Csv::Combine(
	const std::vector<CStringW>& fields, //!< [in] ���ʂ̕������ꂽ�����񂪒ǉ������z��
	wchar_t separator, //!< [in] ��؂蕶��
	wchar_t bundler, //!< [in] ���蕶��
	CStringW& text //!< [out] �A����̕�����
) {
	wchar_t spchars[3], rep1[2], rep2[3];
	CStringW s2;

	if (bundler == 0) {
		spchars[0] = separator;
		spchars[1] = 0;
	} else {
		spchars[0] = separator;
		spchars[1] = bundler;
		spchars[2] = 0;

		rep1[0] = bundler;
		rep1[1] = 0;
		rep2[0] = bundler;
		rep2[1] = bundler;
		rep2[2] = 0;
	}

	text.Empty();
	for (int i = 0; i < (int)fields.size(); i++) {
		if (i != 0)
			text += separator;

		const CStringW& s = fields[i];
		if (!s.IsEmpty()) {
			if (bundler == 0 || s.FindOneOf(spchars) < 0) {
				// ���蕶���w�肪�����܂��̓t�B�[���h���ɋ�؂蕶�����܂܂�Ă��Ȃ��ꍇ�ɂ͊���Ȃ�
				text += s;
			} else {
				// ���蕶���w�肪���芎�t�B�[���h���ɋ�؂蕶�����܂܂�Ă���ꍇ�ɂ͊���
				// ���̍ۊ��蕶�����Q�A�ɒu��������
				s2 = s;
				s2.Replace(rep1, rep2);
				text += bundler;
				text += s2;
				text += bundler;
			}
		}
	}
}


//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
CString Csv::GetField(
	const std::vector<CStringW>& fields, //!< [in] �z��
	int iIndex, //!< �z����̃C���f�b�N�X
	LPCTSTR pszDef //!< �f�t�H���g�l
	) {
	if (iIndex < 0 || (int)fields.size() <= iIndex) {
		if (pszDef != NULL)
			return pszDef;
		else
			return CString();
	}
	return CString(fields[iIndex]);
}

//! �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
CString Csv::GetField(
	const std::vector<CStringA>& fields, //!< [in] �z��
	int iIndex, //!< �z����̃C���f�b�N�X
	LPCTSTR pszDef //!< �f�t�H���g�l
) {
	if (iIndex < 0 || (int)fields.size() <= iIndex) {
		if (pszDef != NULL)
			return pszDef;
		else
			return CString();
	}
	return CString(fields[iIndex]);
}
