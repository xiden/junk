using System;
using System.Runtime.InteropServices;

namespace Junk
{
	/// <summary>
	/// �V�X�e���C���[�W���X�g�ւ̃A�N�Z�X���s���N���X�B
	/// </summary>
    internal static class SystemImageList
    {
        #region �t�B�[���h
		private static Boolean m_bInitialized = false;
        private static IntPtr m_himlSmall = IntPtr.Zero;
		private static IntPtr m_himlLarge = IntPtr.Zero;
        #endregion

		#region �v���p�e�B
		/// <summary>
		/// ���V�X�e���C���[�W���X�g�n���h���̎擾�B
		/// </summary>
		public static IntPtr SmallImageList
		{
			get
			{
				if (!m_bInitialized)
					Initialize();
				return m_himlSmall;
			}
		}

		/// <summary>
		/// ��V�X�e���C���[�W���X�g�n���h���̎擾�B
		/// </summary>
		public static IntPtr LargeImageList
		{
			get
			{
				if (!m_bInitialized)
					Initialize();
				return m_himlLarge;
			}
		}
		#endregion

		#region �������\�b�h
		/// <summary>
		///	�ϐ��Ȃǂ�����������B
		/// </summary>
		private static void Initialize()
		{
			m_bInitialized = true;

			ShellAPI.SHFILEINFO shInfo = new ShellAPI.SHFILEINFO();
			ShellAPI.SHGFI dwAttribs =
				ShellAPI.SHGFI.SHGFI_USEFILEATTRIBUTES |
				ShellAPI.SHGFI.SHGFI_SMALLICON |
				ShellAPI.SHGFI.SHGFI_SYSICONINDEX;
			m_himlSmall = ShellAPI.SHGetFileInfoW(".txt", ShellAPI.FILE_ATTRIBUTE_NORMAL, out shInfo, (uint)Marshal.SizeOf(shInfo), dwAttribs);
			if (m_himlSmall.Equals(IntPtr.Zero))
				throw new Exception("���V�X�e���C���[�W���X�g���擾�ł��܂���ł����B");

			dwAttribs =
				ShellAPI.SHGFI.SHGFI_USEFILEATTRIBUTES |
				ShellAPI.SHGFI.SHGFI_LARGEICON |
				ShellAPI.SHGFI.SHGFI_SYSICONINDEX;
			m_himlLarge = ShellAPI.SHGetFileInfoW(".txt", ShellAPI.FILE_ATTRIBUTE_NORMAL, out shInfo, (uint)Marshal.SizeOf(shInfo), dwAttribs);
			if (m_himlLarge.Equals(IntPtr.Zero))
				throw new Exception("��V�X�e���C���[�W���X�g���擾�ł��܂���ł����B");
		}
		#endregion
    }
}
