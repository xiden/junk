﻿namespace PolygonBoolean {
	partial class Form1 {
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージ リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows フォーム デザイナーで生成されたコード

		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent() {
			this.lblResult = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.radOr = new System.Windows.Forms.RadioButton();
			this.radSub = new System.Windows.Forms.RadioButton();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.radFilter = new System.Windows.Forms.RadioButton();
			this.radExtract = new System.Windows.Forms.RadioButton();
			this.radXor = new System.Windows.Forms.RadioButton();
			this.radAnd = new System.Windows.Forms.RadioButton();
			this.cmbPolygonIndex = new System.Windows.Forms.ComboBox();
			this.btnClear = new System.Windows.Forms.Button();
			this.btnAddLoop = new System.Windows.Forms.Button();
			this.btnDelLoop = new System.Windows.Forms.Button();
			this.lblPos = new System.Windows.Forms.Label();
			this.cmbPol = new System.Windows.Forms.ComboBox();
			this.cmbHole = new System.Windows.Forms.ComboBox();
			this.btnDelPol = new System.Windows.Forms.Button();
			this.btnAddPol = new System.Windows.Forms.Button();
			this.cmbCurGroup = new System.Windows.Forms.ComboBox();
			this.cmbCurPolygon = new System.Windows.Forms.ComboBox();
			this.cmbCurLoop = new System.Windows.Forms.ComboBox();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.btnOpen = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			this.btnSearchEdge = new System.Windows.Forms.Button();
			this.btnSearchNode = new System.Windows.Forms.Button();
			this.tbSearch = new System.Windows.Forms.TextBox();
			this.cmbSrcHole = new System.Windows.Forms.ComboBox();
			this.cmbSrcPol = new System.Windows.Forms.ComboBox();
			this.btnDebug = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblResult
			// 
			this.lblResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lblResult.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblResult.Location = new System.Drawing.Point(545, 6);
			this.lblResult.Name = "lblResult";
			this.lblResult.Size = new System.Drawing.Size(337, 14);
			this.lblResult.TabIndex = 4;
			this.lblResult.Text = "label3";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(1, 105);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(35, 12);
			this.label1.TabIndex = 6;
			this.label1.Text = "label1";
			// 
			// radOr
			// 
			this.radOr.AutoSize = true;
			this.radOr.Checked = true;
			this.radOr.Location = new System.Drawing.Point(6, 18);
			this.radOr.Name = "radOr";
			this.radOr.Size = new System.Drawing.Size(35, 16);
			this.radOr.TabIndex = 0;
			this.radOr.TabStop = true;
			this.radOr.Text = "Or";
			this.radOr.UseVisualStyleBackColor = true;
			this.radOr.CheckedChanged += new System.EventHandler(this.radOr_CheckedChanged);
			// 
			// radSub
			// 
			this.radSub.AutoSize = true;
			this.radSub.Location = new System.Drawing.Point(142, 18);
			this.radSub.Name = "radSub";
			this.radSub.Size = new System.Drawing.Size(42, 16);
			this.radSub.TabIndex = 3;
			this.radSub.Text = "Sub";
			this.radSub.UseVisualStyleBackColor = true;
			this.radSub.CheckedChanged += new System.EventHandler(this.radSub_CheckedChanged);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.radFilter);
			this.groupBox1.Controls.Add(this.radExtract);
			this.groupBox1.Controls.Add(this.radXor);
			this.groupBox1.Controls.Add(this.radAnd);
			this.groupBox1.Controls.Add(this.radOr);
			this.groupBox1.Controls.Add(this.radSub);
			this.groupBox1.Location = new System.Drawing.Point(3, 60);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(338, 42);
			this.groupBox1.TabIndex = 9;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "演算方法";
			// 
			// radFilter
			// 
			this.radFilter.AutoSize = true;
			this.radFilter.Location = new System.Drawing.Point(256, 18);
			this.radFilter.Name = "radFilter";
			this.radFilter.Size = new System.Drawing.Size(50, 16);
			this.radFilter.TabIndex = 5;
			this.radFilter.Text = "Filter";
			this.radFilter.UseVisualStyleBackColor = true;
			// 
			// radExtract
			// 
			this.radExtract.AutoSize = true;
			this.radExtract.Location = new System.Drawing.Point(190, 18);
			this.radExtract.Name = "radExtract";
			this.radExtract.Size = new System.Drawing.Size(60, 16);
			this.radExtract.TabIndex = 4;
			this.radExtract.Text = "Extract";
			this.radExtract.UseVisualStyleBackColor = true;
			this.radExtract.CheckedChanged += new System.EventHandler(this.radExtract_CheckedChanged);
			// 
			// radXor
			// 
			this.radXor.AutoSize = true;
			this.radXor.Location = new System.Drawing.Point(47, 18);
			this.radXor.Name = "radXor";
			this.radXor.Size = new System.Drawing.Size(40, 16);
			this.radXor.TabIndex = 1;
			this.radXor.Text = "Xor";
			this.radXor.UseVisualStyleBackColor = true;
			this.radXor.CheckedChanged += new System.EventHandler(this.radXor_CheckedChanged);
			// 
			// radAnd
			// 
			this.radAnd.AutoSize = true;
			this.radAnd.Location = new System.Drawing.Point(93, 18);
			this.radAnd.Name = "radAnd";
			this.radAnd.Size = new System.Drawing.Size(43, 16);
			this.radAnd.TabIndex = 2;
			this.radAnd.Text = "And";
			this.radAnd.UseVisualStyleBackColor = true;
			this.radAnd.CheckedChanged += new System.EventHandler(this.radAnd_CheckedChanged);
			// 
			// cmbPolygonIndex
			// 
			this.cmbPolygonIndex.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbPolygonIndex.FormattingEnabled = true;
			this.cmbPolygonIndex.Location = new System.Drawing.Point(347, 74);
			this.cmbPolygonIndex.Name = "cmbPolygonIndex";
			this.cmbPolygonIndex.Size = new System.Drawing.Size(121, 20);
			this.cmbPolygonIndex.TabIndex = 10;
			this.cmbPolygonIndex.SelectedIndexChanged += new System.EventHandler(this.cmbPolygonIndex_SelectedIndexChanged);
			// 
			// btnClear
			// 
			this.btnClear.Location = new System.Drawing.Point(54, 2);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(56, 23);
			this.btnClear.TabIndex = 11;
			this.btnClear.Text = "クリア";
			this.btnClear.UseVisualStyleBackColor = true;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// btnAddLoop
			// 
			this.btnAddLoop.Location = new System.Drawing.Point(397, 2);
			this.btnAddLoop.Name = "btnAddLoop";
			this.btnAddLoop.Size = new System.Drawing.Size(68, 23);
			this.btnAddLoop.TabIndex = 12;
			this.btnAddLoop.Text = "ループ追加";
			this.btnAddLoop.UseVisualStyleBackColor = true;
			this.btnAddLoop.Click += new System.EventHandler(this.btnAddLoop_Click);
			// 
			// btnDelLoop
			// 
			this.btnDelLoop.Location = new System.Drawing.Point(471, 2);
			this.btnDelLoop.Name = "btnDelLoop";
			this.btnDelLoop.Size = new System.Drawing.Size(68, 23);
			this.btnDelLoop.TabIndex = 13;
			this.btnDelLoop.Text = "ループ削除";
			this.btnDelLoop.UseVisualStyleBackColor = true;
			this.btnDelLoop.Click += new System.EventHandler(this.btnDelLoop_Click);
			// 
			// lblPos
			// 
			this.lblPos.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblPos.Location = new System.Drawing.Point(474, 74);
			this.lblPos.Name = "lblPos";
			this.lblPos.Size = new System.Drawing.Size(115, 20);
			this.lblPos.TabIndex = 14;
			this.lblPos.Text = "label2";
			this.lblPos.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// cmbPol
			// 
			this.cmbPol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbPol.FormattingEnabled = true;
			this.cmbPol.Location = new System.Drawing.Point(595, 74);
			this.cmbPol.Name = "cmbPol";
			this.cmbPol.Size = new System.Drawing.Size(59, 20);
			this.cmbPol.TabIndex = 15;
			this.cmbPol.SelectedIndexChanged += new System.EventHandler(this.cmbPol_SelectedIndexChanged);
			// 
			// cmbHole
			// 
			this.cmbHole.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbHole.FormattingEnabled = true;
			this.cmbHole.Location = new System.Drawing.Point(660, 74);
			this.cmbHole.Name = "cmbHole";
			this.cmbHole.Size = new System.Drawing.Size(59, 20);
			this.cmbHole.TabIndex = 16;
			this.cmbHole.SelectedIndexChanged += new System.EventHandler(this.cmbHole_SelectedIndexChanged);
			// 
			// btnDelPol
			// 
			this.btnDelPol.Location = new System.Drawing.Point(229, 2);
			this.btnDelPol.Name = "btnDelPol";
			this.btnDelPol.Size = new System.Drawing.Size(56, 23);
			this.btnDelPol.TabIndex = 18;
			this.btnDelPol.Text = "ポリ削除削除";
			this.btnDelPol.UseVisualStyleBackColor = true;
			this.btnDelPol.Click += new System.EventHandler(this.btnDelPol_Click);
			// 
			// btnAddPol
			// 
			this.btnAddPol.Location = new System.Drawing.Point(167, 2);
			this.btnAddPol.Name = "btnAddPol";
			this.btnAddPol.Size = new System.Drawing.Size(56, 23);
			this.btnAddPol.TabIndex = 17;
			this.btnAddPol.Text = "ポリ追加";
			this.btnAddPol.UseVisualStyleBackColor = true;
			this.btnAddPol.Click += new System.EventHandler(this.btnAddPol_Click);
			// 
			// cmbCurGroup
			// 
			this.cmbCurGroup.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbCurGroup.FormattingEnabled = true;
			this.cmbCurGroup.Location = new System.Drawing.Point(3, 3);
			this.cmbCurGroup.Name = "cmbCurGroup";
			this.cmbCurGroup.Size = new System.Drawing.Size(45, 20);
			this.cmbCurGroup.TabIndex = 19;
			this.cmbCurGroup.SelectedIndexChanged += new System.EventHandler(this.cmbCurGroup_SelectedIndexChanged);
			// 
			// cmbCurPolygon
			// 
			this.cmbCurPolygon.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbCurPolygon.FormattingEnabled = true;
			this.cmbCurPolygon.Location = new System.Drawing.Point(116, 3);
			this.cmbCurPolygon.Name = "cmbCurPolygon";
			this.cmbCurPolygon.Size = new System.Drawing.Size(45, 20);
			this.cmbCurPolygon.TabIndex = 20;
			this.cmbCurPolygon.SelectedIndexChanged += new System.EventHandler(this.cmbCurPolygon_SelectedIndexChanged);
			// 
			// cmbCurLoop
			// 
			this.cmbCurLoop.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbCurLoop.FormattingEnabled = true;
			this.cmbCurLoop.Location = new System.Drawing.Point(346, 3);
			this.cmbCurLoop.Name = "cmbCurLoop";
			this.cmbCurLoop.Size = new System.Drawing.Size(45, 20);
			this.cmbCurLoop.TabIndex = 21;
			// 
			// pictureBox1
			// 
			this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox1.Location = new System.Drawing.Point(291, 2);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(49, 22);
			this.pictureBox1.TabIndex = 22;
			this.pictureBox1.TabStop = false;
			this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
			// 
			// btnOpen
			// 
			this.btnOpen.Location = new System.Drawing.Point(3, 31);
			this.btnOpen.Name = "btnOpen";
			this.btnOpen.Size = new System.Drawing.Size(75, 23);
			this.btnOpen.TabIndex = 23;
			this.btnOpen.Text = "開く";
			this.btnOpen.UseVisualStyleBackColor = true;
			this.btnOpen.Click += new System.EventHandler(this.btnOpen_Click);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.btnSearchEdge);
			this.panel1.Controls.Add(this.btnSearchNode);
			this.panel1.Controls.Add(this.tbSearch);
			this.panel1.Controls.Add(this.cmbSrcHole);
			this.panel1.Controls.Add(this.cmbSrcPol);
			this.panel1.Controls.Add(this.btnDebug);
			this.panel1.Controls.Add(this.cmbCurGroup);
			this.panel1.Controls.Add(this.btnOpen);
			this.panel1.Controls.Add(this.lblResult);
			this.panel1.Controls.Add(this.pictureBox1);
			this.panel1.Controls.Add(this.label1);
			this.panel1.Controls.Add(this.cmbCurLoop);
			this.panel1.Controls.Add(this.groupBox1);
			this.panel1.Controls.Add(this.cmbCurPolygon);
			this.panel1.Controls.Add(this.cmbPolygonIndex);
			this.panel1.Controls.Add(this.btnClear);
			this.panel1.Controls.Add(this.btnDelPol);
			this.panel1.Controls.Add(this.btnAddLoop);
			this.panel1.Controls.Add(this.btnAddPol);
			this.panel1.Controls.Add(this.btnDelLoop);
			this.panel1.Controls.Add(this.cmbHole);
			this.panel1.Controls.Add(this.lblPos);
			this.panel1.Controls.Add(this.cmbPol);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(885, 166);
			this.panel1.TabIndex = 24;
			this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
			// 
			// btnSearchEdge
			// 
			this.btnSearchEdge.Location = new System.Drawing.Point(602, 31);
			this.btnSearchEdge.Name = "btnSearchEdge";
			this.btnSearchEdge.Size = new System.Drawing.Size(75, 23);
			this.btnSearchEdge.TabIndex = 29;
			this.btnSearchEdge.Text = "エッジ検索";
			this.btnSearchEdge.UseVisualStyleBackColor = true;
			this.btnSearchEdge.Click += new System.EventHandler(this.btnSearchEdge_Click);
			// 
			// btnSearchNode
			// 
			this.btnSearchNode.Location = new System.Drawing.Point(521, 31);
			this.btnSearchNode.Name = "btnSearchNode";
			this.btnSearchNode.Size = new System.Drawing.Size(75, 23);
			this.btnSearchNode.TabIndex = 28;
			this.btnSearchNode.Text = "ノード検索";
			this.btnSearchNode.UseVisualStyleBackColor = true;
			this.btnSearchNode.Click += new System.EventHandler(this.btnSearchNode_Click);
			// 
			// tbSearch
			// 
			this.tbSearch.Location = new System.Drawing.Point(466, 33);
			this.tbSearch.Name = "tbSearch";
			this.tbSearch.Size = new System.Drawing.Size(49, 19);
			this.tbSearch.TabIndex = 27;
			this.tbSearch.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			// 
			// cmbSrcHole
			// 
			this.cmbSrcHole.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbSrcHole.FormattingEnabled = true;
			this.cmbSrcHole.Location = new System.Drawing.Point(243, 34);
			this.cmbSrcHole.Name = "cmbSrcHole";
			this.cmbSrcHole.Size = new System.Drawing.Size(59, 20);
			this.cmbSrcHole.TabIndex = 26;
			this.cmbSrcHole.SelectedIndexChanged += new System.EventHandler(this.cmbSrcHole_SelectedIndexChanged);
			// 
			// cmbSrcPol
			// 
			this.cmbSrcPol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbSrcPol.FormattingEnabled = true;
			this.cmbSrcPol.Location = new System.Drawing.Point(178, 34);
			this.cmbSrcPol.Name = "cmbSrcPol";
			this.cmbSrcPol.Size = new System.Drawing.Size(59, 20);
			this.cmbSrcPol.TabIndex = 25;
			this.cmbSrcPol.SelectedIndexChanged += new System.EventHandler(this.cmbSrcPol_SelectedIndexChanged);
			// 
			// btnDebug
			// 
			this.btnDebug.Location = new System.Drawing.Point(84, 31);
			this.btnDebug.Name = "btnDebug";
			this.btnDebug.Size = new System.Drawing.Size(75, 23);
			this.btnDebug.TabIndex = 24;
			this.btnDebug.Text = "デバッグ";
			this.btnDebug.UseVisualStyleBackColor = true;
			this.btnDebug.Click += new System.EventHandler(this.btnDebug_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(885, 614);
			this.Controls.Add(this.panel1);
			this.Name = "Form1";
			this.Text = "Form1";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion
		private System.Windows.Forms.Label lblResult;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.RadioButton radOr;
		private System.Windows.Forms.RadioButton radSub;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.RadioButton radAnd;
		private System.Windows.Forms.RadioButton radXor;
		private System.Windows.Forms.ComboBox cmbPolygonIndex;
		private System.Windows.Forms.RadioButton radExtract;
		private System.Windows.Forms.Button btnClear;
		private System.Windows.Forms.Button btnAddLoop;
		private System.Windows.Forms.Button btnDelLoop;
		private System.Windows.Forms.Label lblPos;
		private System.Windows.Forms.ComboBox cmbPol;
		private System.Windows.Forms.ComboBox cmbHole;
		private System.Windows.Forms.Button btnDelPol;
		private System.Windows.Forms.Button btnAddPol;
		private System.Windows.Forms.ComboBox cmbCurGroup;
		private System.Windows.Forms.ComboBox cmbCurPolygon;
		private System.Windows.Forms.ComboBox cmbCurLoop;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.RadioButton radFilter;
		private System.Windows.Forms.Button btnOpen;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button btnDebug;
		private System.Windows.Forms.ComboBox cmbSrcHole;
		private System.Windows.Forms.ComboBox cmbSrcPol;
		private System.Windows.Forms.Button btnSearchEdge;
		private System.Windows.Forms.Button btnSearchNode;
		private System.Windows.Forms.TextBox tbSearch;
	}
}

