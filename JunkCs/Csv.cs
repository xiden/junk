using System;
using System.Collections.Generic;
using System.Text;

namespace Junk {
	/// <summary>
	/// CSV�֌W�̏������܂Ƃ߂��N���X�B
	/// </summary>
	public static class Csv {
		/// <summary>
		/// CSV�`���̕�������t�B�[���h�ɕ�����B
		/// </summary>
		/// <param name="text">�t�B�[���h�ɕ���������������B</param>
		/// <param name="sp">�Z�p���[�^������</param>
		/// <returns>�t�B�[���h�̔z��B</returns>
		public static string[] TextSplit(string text, char sp) {
			if (text == null)
				return new string[0];

			int i; // ��Ɨp�|�C���^
			int start; // �t�B�[���h�̊J�n�ʒu
			int end; // �t�B�[���h�̏I�[�ʒu+1(��؂蕶���܂��͏I�[�̈ʒu)
			int len = text.Length; // ������
			List<string> fields = new List<string>(); // �t�B�[���h�z��

			// �����񂩂�t�B�[���h�����o���Ă���
			i = 0;
			while (i <= len) {
				// �t�B�[���h�̐擪��T��
				while (i < len && (text[i] == ' ' || text[i] == '\t')) {
					i++;
				}

				// �_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
				char sp2 = sp;
				if (i < len && text[i] == '"') {
					sp2 = '"';
					i++;
				}

				// ���̎��_�Ńt�B�[���h�̐擪���m��
				start = i;

				// ��؂蕶����T���t�B�[���h�̏I�[���m�肷��
				end = text.IndexOf(sp2, start);
				if (end < 0)
					end = len;

				// �t�B�[���h�z��Ƀt�B�[���h��ǉ�
				fields.Add(text.Substring(start, end - start));

				// ��؂蕶�����I�[�o�[���[�h����Ă����ꍇ�ɂ͌��̋�؂蕶���܂Ői�߂�
				if (sp != sp2) {
					end = text.IndexOf(sp, end);
					if (end < 0)
						end = len;
				}

				// �t�B�[���h�I�[��������̏I�[�Ɉ�v���Ă���Ȃ�S�Đ؂�o������
				if (end == len)
					break;

				// ���t�B�[���h�̌����J�n�ʒu�ֈړ�
				i = end + 1;
			}

			return fields.ToArray();
		}

		/// <summary>
		/// �w���؂蕶���Ńt�B�[���h��؂蕪���Ď擾����
		/// </summary>
		/// <param name="text">[in] �؂�o������������</param>
		/// <param name="separator">[in] ��؂蕶��</param>
		/// <param name="bundler">[in] ���蕶��</param>
		/// <returns>������̃t�B�[���h�z�񂪕Ԃ�</returns>
		public static List<string> Split(string text, char separator, char bundler) {
			if (string.IsNullOrEmpty(text))
				return new List<string>();

			int p = 0; // ��Ɨp�|�C���^
			int pFieldStart; // �t�B�[���h�̊J�n�ʒu
			int pFieldEnd; // �t�B�[���h�̏I�[�ʒu+1(��؂蕶���܂��͏I�[�̈ʒu)
			int pTextEnd = text.Length; // ������
			StringBuilder field = new StringBuilder(); // �t�B�[���h�����z��
			List<string> fields = new List<string>(); // ������̃t�B�[���h�z��

			// �����񂩂�t�B�[���h�����o���Ă���
			while (p <= pTextEnd) {
				// �t�B�[���h�̐擪��T��
				while (p < pTextEnd && (text[p] == ' ')) {
					p++;
				}

				// �_�u���N�H�[�e�[�V�����Ŋ����Ă��邩���ׂ�
				char sp2 = separator;
				if (bundler != 0 && p < pTextEnd && text[p] == bundler) {
					sp2 = bundler;
					p++;
				}

				// ���̎��_�Ńt�B�[���h�̐擪���m��
				pFieldStart = p;

				// ��؂蕶����T���t�B�[���h�̏I�[���m�肷��
				field.Length = 0;
				pFieldEnd = pFieldStart;
				while (pFieldEnd < pTextEnd) {
					int move = 1;
					if (text[pFieldEnd] == sp2) {
						if (separator != sp2 && pFieldEnd + 1 < text.Length && text[pFieldEnd + 1] == sp2)
							move = 2; // �_�u���N�H�[�e�[�V�������Q����ł���Ȃ炻��͏I�[�ł͂Ȃ�
						else
							break;
					}
					field.Append(text[pFieldEnd]);
					pFieldEnd += move;
				}

				// �t�B�[���h�z��Ƀt�B�[���h��ǉ�
				if (field.Length == 0)
					fields.Add("");
				else
					fields.Add(field.ToString());

				// ��؂蕶�����I�[�o�[���[�h����Ă����ꍇ�ɂ͌��̋�؂蕶���܂Ői�߂�
				if (separator != sp2) {
					while (pFieldEnd < pTextEnd) {
						if (text[pFieldEnd] == separator)
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

			return fields;
		}

		/// <summary>
		/// �w���؂蕶���Ńt�B�[���h��A�����Ď擾����
		/// </summary>
		/// <param name="fields">[in] ���ʂ̕������ꂽ�����񂪒ǉ������z��</param>
		/// <param name="separator">[in] ��؂蕶��</param>
		/// <param name="bundler">[in] ���蕶��</param>
		/// <returns>�A����̕�����</returns>
		public static string Combine(IEnumerable<string> fields, char separator, char bundler) {
			char[] spchars;
			string rep1 = null, rep2 = null;
			StringBuilder text = new StringBuilder();

			if (bundler == 0) {
				spchars = new char[1];
				spchars[0] = separator;
			} else {
				spchars = new char[2];
				spchars[0] = separator;
				spchars[1] = bundler;

				rep1 = new string(new char[] { bundler });
				rep2 = new string(new char[] { bundler, bundler });
			}

			bool firstField = true;
			foreach (var s in fields) {
				if (firstField) {
					firstField = false;
				} else {
					text.Append(separator);
				}

				if (!string.IsNullOrEmpty(s)) {
					if (bundler == 0 || s.IndexOfAny(spchars) < 0) {
						// ���蕶���w�肪�����܂��̓t�B�[���h���ɋ�؂蕶�����܂܂�Ă��Ȃ��ꍇ�ɂ͊���Ȃ�
						text.Append(s);
					} else {
						// ���蕶���w�肪���芎�t�B�[���h���ɋ�؂蕶�����܂܂�Ă���ꍇ�ɂ͊���
						// ���̍ۊ��蕶�����Q�A�ɒu��������
						text.Append(bundler);
						text.Append(s.Replace(rep1, rep2));
						text.Append(bundler);
					}
				}
			}

			return text.ToString();
		}

		/// <summary>
		/// �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
		/// </summary>
		/// <param name="fields">[in] �z��</param>
		/// <param name="index">[in] �z����̃C���f�b�N�X</param>
		/// <param name="def">[in] �f�t�H���g�l</param>
		/// <returns>�w��ʒu�̃t�B�[���h�l</returns>
		public static string GetField(List<string> fields, int index, string def = null) {
			if (index < 0 || fields.Count <= index) {
				if (def != null)
					return def;
				else
					return "";
			}
			return fields[index];
		}

		/// <summary>
		/// �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
		/// </summary>
		/// <param name="fields">[in] �z��</param>
		/// <param name="index">[in] �z����̃C���f�b�N�X</param>
		/// <param name="def">[in] �f�t�H���g�l</param>
		/// <returns>�w��ʒu�̃t�B�[���h�l</returns>
		public static int GetField(List<string> fields, int index, int def) {
			int value;
			if (index < 0 || fields.Count <= index || !int.TryParse(fields[index].Trim(), out value)) {
				return def;
			}
			return value;
		}

		/// <summary>
		/// �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
		/// </summary>
		/// <param name="fields">[in] �z��</param>
		/// <param name="index">[in] �z����̃C���f�b�N�X</param>
		/// <param name="def">[in] �f�t�H���g�l</param>
		/// <returns>�w��ʒu�̃t�B�[���h�l</returns>
		public static long GetField(List<string> fields, int index, long def) {
			long value;
			if (index < 0 || fields.Count <= index || !long.TryParse(fields[index].Trim(), out value)) {
				return def;
			}
			return value;
		}

		/// <summary>
		/// �z�񂩂�w��t�B�[���h���擾����A�z��T�C�Y������Ȃ�������f�t�H���g�l���Ԃ�
		/// </summary>
		/// <param name="fields">[in] �z��</param>
		/// <param name="index">[in] �z����̃C���f�b�N�X</param>
		/// <param name="def">[in] �f�t�H���g�l</param>
		/// <returns>�w��ʒu�̃t�B�[���h�l</returns>
		public static double GetField(List<string> fields, int index, double def) {
			double value;
			if (index < 0 || fields.Count <= index || !double.TryParse(fields[index].Trim(), out value)) {
				return def;
			}
			return value;
		}
	}
}
