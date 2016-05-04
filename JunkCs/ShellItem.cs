using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Jk
{
	/// <summary>
	/// �V�F���A�C�e���̎�ޗ񋓒l�B
	/// </summary>
	public enum ShellItemType
	{
		/// <summary>
		/// �V�F���A�C�e���̓R���g���[���p�l����S�~���Ȃǂ̓���t�H���_�B
		/// </summary>
		SpecialFolder,

		/// <summary>
		/// �V�F���A�C�e���̓h���C�u�B
		/// </summary>
		Drive,

		/// <summary>
		/// �V�F���A�C�e���͒ʏ�̃t�H���_�B
		/// </summary>
		Folder,

		/// <summary>
		/// �V�F���A�C�e���̓R���g���[���p�l�����̍��ڂ̂悤�ȓ���A�C�e���B
		/// </summary>
		SpecialItem,

		/// <summary>
		/// �V�F���A�C�e���͒ʏ�̃t�@�C���B
		/// </summary>
		File
	}

	/// <summary>
	///	�t�H���_�A�t�@�C���Ȃǂ��w�������N���X�B
	/// </summary>
	public class ShellItem
	{
		#region �N���X�Ȃ�
		/// <summary>
		/// �t�@�C���V�X�e���̏��B
		/// </summary>
		public class FileInfo
		{
			internal DateTime m_CreationTime;
			internal DateTime m_LastAccessTime;
			internal DateTime m_LastWriteTime;
			internal long m_FileSize;

			/// <summary>
			/// �t�@�C���쐬���ԁB
			/// </summary>
			public DateTime CreationTime
			{
				get { return m_CreationTime; }
				set { m_CreationTime = value; }
			}

			/// <summary>
			/// �t�@�C���ŏI�A�N�Z�X���ԁB
			/// </summary>
			public DateTime LastAccessTime
			{
				get { return m_LastAccessTime; }
				set { m_LastAccessTime = value; }
			}

			/// <summary>
			/// �t�@�C���ŏI�X�V���ԁB
			/// </summary>
			public DateTime LastWriteTime
			{
				get { return m_LastWriteTime; }
				set { m_LastWriteTime = value; }
			}

			/// <summary>
			/// �t�@�C���T�C�Y�B
			/// </summary>
			public long FileSize
			{
				get { return m_FileSize; }
				set { m_FileSize = value; }
			}
		}
		#endregion

		#region �t�B�[���h
		[Flags]
		enum FlagsEnum
		{
			IsFolder = 1 << 0,
			IsFileSystem = 1 << 1,
			HasSubFolder = 1 << 2,

			//HasSubFolderInitialized = 1 << 16,
		}

		/// <summary>
		/// InitAtr �ł̏��������[�h�B
		/// </summary>
		enum InitMode
		{
			/// <summary>
			/// �f�X�N�g�b�v�A�C�e���p�̏������B
			/// </summary>
			Desktop,

			/// <summary>
			/// �p�X����쐬���p�̏������B
			/// </summary>
			Path,

			/// <summary>
			/// ShellItem �̎q�Ƃ��č쐬���p�̏������B
			/// </summary>
			Child,
		}

		private static ShellItem m_DesktopShellItem = null;

		private ShellAPI.IShellFolder m_ShellFolder = null;
		private ShellAPI.IShellFolder m_ParentShellFolder = null;
		private IntPtr m_pIDL = IntPtr.Zero;         // ���̃A�C�e���̐�΃p�XIDL
		private IntPtr m_pRelativeIDL = IntPtr.Zero; // �ЂƂ�̐e����̑��΃p�XIDL
		private string m_DisplayName = "";
		private string m_Path = "";
		private string m_FileName = "";
		private string m_TypeName = "";
		private Int32 m_iIconIndex = -1;
		private Int32 m_iSelectedIconIndex = -1;
		private ShellItemType m_ItemType = ShellItemType.SpecialFolder;
		private FlagsEnum m_Flags = 0;
		#endregion

		#region �R���X�g���N�^
		/// <summary>
		/// �R���X�g���N�^�B�w�肳�ꂽ�p�X���w�������悤�ɏ���������B
		/// </summary>
		/// <param name="path">�t���p�X���B</param>
		public ShellItem(string path)
		{
			//	�w�肳�ꂽ�p�X���w������ITEMIDL���擾����
			m_pIDL = ShellAPI.ILCreateFromPathW(path);
			if (m_pIDL == IntPtr.Zero)
				throw new Exception(string.Format("{0} �͗L���ȃp�X�ł͂���܂���B", path));

			IntPtr parentIdl = IntPtr.Zero;
			try
			{
				//	�w�肳�ꂽ�p�X�̐e��IDL���擾
				parentIdl = ShellAPI.ILClone(m_pIDL);
				ShellAPI.ILRemoveLastID(parentIdl);

				//	�e����̑���IDL������
				IntPtr relativeIdl = ShellAPI.ILFindLastID(m_pIDL);

				//	����IDL���擾�i����������R�ꂳ���Ȃ����߂ɑ����Ƀ����o�ϐ��֊i�[�j
				m_pRelativeIDL = ShellAPI.ILClone(relativeIdl);
				//	�w�肳�ꂽ�p�X�̐e��IShellFolder���擾
				m_ParentShellFolder = ShellAPI.BindToIShellFolder(DesktopShellItem.ShellFolder, parentIdl);

				//	�����Ȃǂ��낢�돉��������
				InitAtr(InitMode.Path, m_pRelativeIDL, m_ParentShellFolder);
			}
			finally
			{
				if (parentIdl != IntPtr.Zero)
					Marshal.FreeCoTaskMem(parentIdl);
			}
		}

		/// <summary>
		///	�R���X�g���N�^�B�f�X�N�g�b�v���w�������悤�ɏ���������B
		/// </summary>
		private ShellItem()
		{
			//	�f�X�N�g�b�v��IDL���擾����
			int hRes = ShellAPI.SHGetSpecialFolderLocation(IntPtr.Zero, ShellAPI.CSIDL.CSIDL_DESKTOP, ref m_pIDL);
			if (hRes != 0)
				Marshal.ThrowExceptionForHR(hRes);

			//	�f�X�N�g�b�v�� IShellFolder �C���^�[�t�F�[�X���擾����
			hRes = ShellAPI.SHGetDesktopFolder(out m_ShellFolder);
			if (hRes != 0)
				Marshal.ThrowExceptionForHR(hRes);

			//	�����Ȃǂ��낢�돉��������
			InitAtr(InitMode.Desktop, IntPtr.Zero, null);
		}

		/// <summary>
		/// �R���X�g���N�^�B�w�肳�ꂽ ShellItem �̎q�Ƃ��ď���������B
		/// �������r���ŗ�O������������ pIDL �����L���Ă͂Ȃ�Ȃ��B
		/// </summary>
		/// <param name="pIDL">�q�A�C�e����ITEMIDL�B</param>
		/// <param name="shParent">�e�V�F���A�C�e���I�u�W�F�N�g�B</param>
		private ShellItem(IntPtr pIDL, ShellItem shParent)
		{
			//	ITEMIDL��A��
			m_pIDL = ShellAPI.ILCombine(shParent.PIDL, pIDL);
			//	�����Ȃǂ��낢�돉��������
			InitAtr(InitMode.Child, pIDL, shParent.ShellFolder);

			//	�e IShellFolder �Ɛe����̑���IDL���擾
			m_ParentShellFolder = shParent.ShellFolder;
			m_pRelativeIDL = pIDL;
		}
		#endregion

		#region �f�X�g���N�^
		/// <summary>
		///	�f�X�g���N�^�B
		/// </summary>
		~ShellItem()
		{
			if (m_pIDL != IntPtr.Zero) Marshal.FreeCoTaskMem(m_pIDL);
			if (m_pRelativeIDL != IntPtr.Zero) Marshal.FreeCoTaskMem(m_pRelativeIDL);
			GC.SuppressFinalize(this);
		}
		#endregion

		#region �v���p�e�B
		/// <summary>
		/// �f�X�N�g�b�v���w�������V�F���A�C�e���I�u�W�F�N�g���擾����B
		/// </summary>
		public static ShellItem DesktopShellItem
		{
			get
			{
				if (m_DesktopShellItem == null)
					m_DesktopShellItem = new ShellItem();
				return m_DesktopShellItem;
			}
		}

		/// <summary>
		///	�V�F���A�C�e���̕\�����̂̎擾�B
		/// </summary>
		public string DisplayName
		{
			get { return m_DisplayName; }
		}

		/// <summary>
		///	�V�F���A�C�e���̎�ޖ��̂̎擾�B
		/// </summary>
		public string TypeName
		{
			get { return m_TypeName; }
		}

		/// <summary>
		///	���̃V�F���A�C�e���̃A�C�R���̃V�X�e���C���[�W���X�g���ł̃C���f�b�N�X�ԍ��̎擾�B
		/// </summary>
		public Int32 IconIndex
		{
			get { return m_iIconIndex; }
		}

		/// <summary>
		///	���̃V�F���A�C�e���̑I����Ԃ̃A�C�R���̃V�X�e���C���[�W���X�g���ł̃C���f�b�N�X�ԍ��̎擾�B
		/// </summary>
		public Int32 SelectedIconIndex
		{
			get { return m_iSelectedIconIndex; }
		}

		/// <summary>
		///	���̃V�F���A�C�e���� IShellFolder �C���^�[�t�F�[�X���擾����B
		/// </summary>
		internal ShellAPI.IShellFolder ShellFolder
		{
			get { return m_ShellFolder; }
		}

		/// <summary>
		/// ���̃A�C�e����ITEMIDL���擾����B
		/// </summary>
		public IntPtr PIDL
		{
			get { return m_pIDL; }
		}

		/// <summary>
		///	���̃V�F���A�C�e�����t�H���_���ǂ����̃t���O�̎擾�B�ʏ�̃t�@�C���t�H���_�A�V�X�e���t�H���_�̏ꍇ�� true ���擾����܂��B
		/// </summary>
		public bool IsFolder
		{
			get { return GetFlags(FlagsEnum.IsFolder); }
			private set { SetFlags(FlagsEnum.IsFolder, value); }
		}

		/// <summary>
		///	���̃V�F���A�C�e�����t�@�C���V�X�e�����ǂ����̃t���O�̎擾�B
		/// </summary>
		public bool IsFileSystem
		{
			get { return GetFlags(FlagsEnum.IsFileSystem); }
			private set { SetFlags(FlagsEnum.IsFileSystem, value); }
		}

		/// <summary>
		///	���̃V�F���A�C�e���̎�ނ̎擾�B
		/// </summary>
		public ShellItemType ItemType
		{
			get { return m_ItemType; }
		}

		/// <summary>
		///	���̃V�F���A�C�e���Ɏq�t�H���_�����邩�ǂ����̃t���O�̎擾�B
		/// </summary>
		public bool HasSubFolder
		{
			get
			{
				//if (!GetFlags(FlagsEnum.HasSubFolderInitialized))
				//{
				//    //	���̃A�C�e���̃v���p�e�B���擾����
				//    ShellAPI.SFGAOF uFlags = ShellAPI.SFGAOF.SFGAO_HASSUBFOLDER;
				//    m_Parent.ShellFolder.GetAttributesOf(1, out m_pRelativeIDL, out uFlags);
				//    this.HasSubFolder = Convert.ToBoolean(uFlags & ShellAPI.SFGAOF.SFGAO_HASSUBFOLDER);
				//}
				return GetFlags(FlagsEnum.HasSubFolder);
			}
			private set
			{
				SetFlags(FlagsEnum.HasSubFolder, value);
				//SetFlags(FlagsEnum.HasSubFolderInitialized, true);
			}
		}

		/// <summary>
		///	���̃V�F���A�C�e���̃p�X�����擾�B
		/// </summary>
		public string Path
		{
			get { return m_Path; }
		}

		/// <summary>
		///	�t�@�C�����̎擾�B
		/// </summary>
		public string FileName
		{
			get { return m_FileName; }
		}
		#endregion

		#region ���\�b�h

		/// <summary>
		/// ���̃A�C�e���̌��݂̃t�@�C�������擾����B
		/// �t�@�C���V�X�e���ȊO��f�X�N�g�b�v����t�@�C�������擾���悤�Ƃ����O���o�܂��B
		/// </summary>
		/// <returns>�t�@�C�����B</returns>
		public FileInfo GetFileInfo()
		{
			if (m_ParentShellFolder == null || m_pRelativeIDL == IntPtr.Zero)
			{
				throw new Exception(string.Format("���� ShellItem(\"{0}\") ����t�@�C�������擾���邱�Ƃ͂ł��܂���B", m_Path));
			}

			//	������ IDL ����t�@�C�������擾
			ShellAPI.WIN32_FIND_DATA wfd = new ShellAPI.WIN32_FIND_DATA();
			int hr = ShellAPI.SHGetDataFromIDListW(m_ParentShellFolder, m_pRelativeIDL, ShellAPI.SHGDFIL_FINDDATA, out wfd, Marshal.SizeOf(wfd));
			if (hr < 0)
			{
				//	�f�X�N�g�b�v�̃t�@�C���͉��̂� SHGetDataFromIDListW �����s����̂� FindFirstFileW ���g��
				IntPtr hFind = ShellAPI.FindFirstFileW(m_Path, out wfd);
				if (hFind != new IntPtr(-1))
					ShellAPI.FindClose(hFind);
			}

			//	FileInfo �̒��g��ݒ肷��
			FileInfo fi = new FileInfo();
			ShellAPI.FILETIME ft = new ShellAPI.FILETIME();
			ShellAPI.SYSTEMTIME st = new ShellAPI.SYSTEMTIME();

			ShellAPI.FileTimeToLocalFileTime(ref wfd.ftCreationTime, out ft);
			ShellAPI.FileTimeToSystemTime(ref ft, out st);
			fi.m_CreationTime = new DateTime((int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds);

			ShellAPI.FileTimeToLocalFileTime(ref wfd.ftLastAccessTime, out ft);
			ShellAPI.FileTimeToSystemTime(ref ft, out st);
			fi.m_LastAccessTime = new DateTime((int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds);

			ShellAPI.FileTimeToLocalFileTime(ref wfd.ftLastWriteTime, out ft);
			ShellAPI.FileTimeToSystemTime(ref ft, out st);
			fi.m_LastWriteTime = new DateTime((int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour, (int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds);

			fi.m_FileSize = ((long)wfd.nFileSizeHigh << 32) | (long)wfd.nFileSizeLow;

			return fi;
		}

		/// <summary>
		/// �T�u�t�H���_�z����擾����B
		/// </summary>
		/// <returns>ShellItem �̃��X�g�B</returns>
		public List<ShellItem> GetSubFolders()
		{
			//	Make sure we have a folder.
			if (this.IsFolder == false)
				throw new Exception("�t�H���_�ȊO����͎q�t�H���_�͎擾�ł��܂���B");

			List<ShellItem> arrChildren = new List<ShellItem>();

			//	IEnumIDList �C���^�[�t�F�[�X�|�C���^���擾����
			ShellAPI.IEnumIDList pEnum = null;
			uint hRes = m_ShellFolder.EnumObjects(
				IntPtr.Zero,
				ShellAPI.SHCONTF.SHCONTF_FOLDERS | ShellAPI.SHCONTF.SHCONTF_INCLUDEHIDDEN,
				out pEnum);
			if (hRes != 0)
				Marshal.ThrowExceptionForHR((int)hRes);

			//	�q��IDL��񋓂���
			Int32 iGot;
			IntPtr child = IntPtr.Zero;
			try
			{
				while (true)
				{
					if (pEnum.Next(1, out child, out iGot) != 0)
						break;
					arrChildren.Add(new ShellItem(child, this)); // child �͍쐬���� ShellItem �ɏ��L�����
					child = IntPtr.Zero;
				}
			}
			finally
			{
				if (child != IntPtr.Zero)
					Marshal.FreeCoTaskMem(child); // ��O���N���� child �̏��L�����ړ����Ă��Ȃ������玩���ŉ��
				Marshal.ReleaseComObject(pEnum); // pEnum �͊O���Ɍ��J���Ă��Ȃ��̂ő����� Release
			}

			return arrChildren;
		}

		/// <summary>
		/// �T�u�A�C�e���z����擾����B
		/// </summary>
		/// <returns>ShellItem �̃��X�g�B</returns>
		public List<ShellItem> GetSubItems()
		{
			//	Make sure we have a folder.
			if (this.IsFolder == false)
				throw new Exception("�t�H���_�ȊO����͎q�A�C�e���͎擾�ł��܂���B");

			List<ShellItem> arrChildren = new List<ShellItem>();

			//	IEnumIDList �C���^�[�t�F�[�X�|�C���^���擾����
			ShellAPI.IEnumIDList pEnum = null;
			uint hRes = m_ShellFolder.EnumObjects(
				IntPtr.Zero,
				ShellAPI.SHCONTF.SHCONTF_FOLDERS | ShellAPI.SHCONTF.SHCONTF_NONFOLDERS | ShellAPI.SHCONTF.SHCONTF_INCLUDEHIDDEN,
				out pEnum);
			if (hRes != 0)
				Marshal.ThrowExceptionForHR((int)hRes);

			//	�q��IDL��񋓂���
			Int32 iGot;
			IntPtr child = IntPtr.Zero;
			try
			{
				while (true)
				{
					if (pEnum.Next(1, out child, out iGot) != 0)
						break;
					arrChildren.Add(new ShellItem(child, this)); // child �͍쐬���� ShellItem �ɏ��L�����
					child = IntPtr.Zero;
				}
			}
			finally
			{
				if (child != IntPtr.Zero)
					Marshal.FreeCoTaskMem(child); // ��O���N���� child �̏��L�����ړ����Ă��Ȃ������玩���ŉ��
				Marshal.ReleaseComObject(pEnum); // pEnum �͊O���Ɍ��J���Ă��Ȃ��̂ő����� Release
			}

			return arrChildren;
		}

		/// <summary>
		/// �w�肳�ꂽ�V�F���A�C�e���������Ɠ������̂��w���V�F���A�C�e�����ǂ������ׂ�B
		/// </summary>
		/// <param name="shellItem">�������ǂ������ׂ����V�F���A�C�e���B</param>
		/// <returns>����:true�A��������Ȃ�:false�B</returns>
		public bool IsSameShellItem(ShellItem shellItem)
		{
			return Convert.ToBoolean(ShellAPI.ILIsEqual(m_pIDL, shellItem.PIDL));
		}

		/// <summary>
		/// �w�肳�ꂽ�V�F���A�C�e���������̎q�A�C�e���Ȃ̂����ׂ�B
		/// </summary>
		/// <param name="shellItem">�q���ǂ������ׂ����V�F���A�C�e���B</param>
		/// <param name="immediate">�����̒��ڂ̎q�����ǂ����𒲂ׂ����ꍇ�� true ���Z�b�g����B</param>
		/// <returns>�����̎q:true�A�����̎q����Ȃ�:false�B</returns>
		public bool IsChild(ShellItem shellItem, bool immediate)
		{
			return Convert.ToBoolean(ShellAPI.ILIsParent(m_pIDL, shellItem.PIDL, immediate ? 1 : 0));
		}
		#endregion

		#region �������\�b�h
		/// <summary>
		///	����������������B
		/// </summary>
		private void InitAtr(InitMode mode, IntPtr pIDL, ShellAPI.IShellFolder parentSf)
		{
			//	��{�I�ȑ���������������
			ShellAPI.SFGAOF uFlags = 0;
			switch (mode)
			{
				case InitMode.Desktop:
					{
						this.IsFolder = true;
						this.IsFileSystem = false;
						this.HasSubFolder = true;
					}
					break;
				case InitMode.Path:
				case InitMode.Child:
					{
						uFlags =
							ShellAPI.SFGAOF.SFGAO_FOLDER |
							ShellAPI.SFGAOF.SFGAO_FILESYSTEM |
							ShellAPI.SFGAOF.SFGAO_FILESYSANCESTOR;
						parentSf.GetAttributesOf(1, out pIDL, out uFlags);
						this.IsFolder = Convert.ToBoolean(uFlags & ShellAPI.SFGAOF.SFGAO_FOLDER);
						this.IsFileSystem = Convert.ToBoolean(uFlags & ShellAPI.SFGAOF.SFGAO_FILESYSTEM);

						if (this.IsFolder)
						{
							//	GetAttributesOf �� SFGAO_HASSUBFOLDER �𒲂ׂ�Ƃ��Ȃ�x���Ȃ�A
							//	�������� this.HasSubFolder �̓q���g�Ƃ��Ďg�p���邾���Ȃ̂Ńt�H���_�̏ꍇ�͖ⓚ���p��
							//	this.HasSubFolder �� true ���Z�b�g����
							this.HasSubFolder = true;

							//	���̃I�u�W�F�N�g�p�� IShellFolder �C���^�[�t�F�[�X�擾
							m_ShellFolder = ShellAPI.BindToIShellFolder(parentSf, pIDL);
						}
					}
					break;
			}

			//	�p�X���擾
			m_Path = ShellAPI.SHGetPathFromIDList(m_pIDL);

			//	�A�C�R���C���f�b�N�X�ԍ��Ƃ��̑����擾
			ShellAPI.SHFILEINFO shInfo = new ShellAPI.SHFILEINFO();
			ShellAPI.SHGetFileInfoW(
				m_pIDL,
				0,
				out shInfo,
				(uint)Marshal.SizeOf(shInfo),
				ShellAPI.SHGFI.SHGFI_SMALLICON |
				ShellAPI.SHGFI.SHGFI_SYSICONINDEX |
				ShellAPI.SHGFI.SHGFI_PIDL |
				ShellAPI.SHGFI.SHGFI_DISPLAYNAME |
				ShellAPI.SHGFI.SHGFI_TYPENAME
			);
			m_DisplayName = shInfo.szDisplayName;
			m_TypeName = shInfo.szTypeName;
			m_iIconIndex = shInfo.iIcon;

			//	�I�����̃A�C�R���C���f�b�N�X�ԍ��擾
			ShellAPI.SHGetFileInfoW(
				m_pIDL,
				0,
				out shInfo,
				(uint)Marshal.SizeOf(shInfo),
				ShellAPI.SHGFI.SHGFI_PIDL |
				ShellAPI.SHGFI.SHGFI_SMALLICON |
				ShellAPI.SHGFI.SHGFI_SYSICONINDEX |
				ShellAPI.SHGFI.SHGFI_OPENICON
			);
			m_iSelectedIconIndex = shInfo.iIcon;

			//	���낢�뒲�ׂăA�C�e�����(m_ItemType)���m�肷��
			if (this.IsFileSystem)
			{
				bool bValidated = false;
				m_FileName = "";

				try
				{
					m_FileName = System.IO.Path.GetFileName(m_Path);

					if (this.IsFolder)
					{
						if (System.IO.Path.GetPathRoot(m_Path) == m_Path)
						{
							//	�p�X���ƃ��[�g�p�X�������Ȃ�h���C�u�̂͂�
							m_ItemType = ShellItemType.Drive;
							bValidated = true;
						}
						else if (Convert.ToBoolean(uFlags & ShellAPI.SFGAOF.SFGAO_FILESYSANCESTOR))
						{
							//	�t�@�C���V�X�e���A�C�e�����܂ނ��Ƃ��ł���t�H���_���t�@�C���t�H���_�i���Ǝv���j
							m_ItemType = ShellItemType.Folder;
							bValidated = true;
						}
					}
				}
				catch
				{
					//	�V�X�e���̃A�C�e���̏ꍇ�܂Ƃ��ȃp�X���ł͂Ȃ��āA��O���o���Ⴄ�̂�
					//	�����ŗ�O�L���b�`
				}

				if (!bValidated)
				{
					m_ItemType = ShellItemType.File;
					bValidated = true;
				}
			}
			else
			{
				if (this.IsFolder)
				{
					m_ItemType = ShellItemType.SpecialFolder;
				}
				else
				{
					m_ItemType = ShellItemType.SpecialItem;
				}
			}
		}

		/// <summary>
		/// �t���O�̒l�擾�B
		/// </summary>
		private bool GetFlags(FlagsEnum type)
		{
			return (m_Flags & type) == type;
		}

		/// <summary>
		/// �t���O�Z�b�g�B
		/// </summary>
		private void SetFlags(FlagsEnum type, bool b)
		{
			if (b)
				m_Flags |= type;
			else
				m_Flags &= ~type;
		}
		#endregion
	}
}
