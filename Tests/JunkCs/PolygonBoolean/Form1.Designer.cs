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
			this.radioButton1 = new System.Windows.Forms.RadioButton();
			this.radioButton2 = new System.Windows.Forms.RadioButton();
			this.lblResult = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.radOr = new System.Windows.Forms.RadioButton();
			this.radSub = new System.Windows.Forms.RadioButton();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.radExtract = new System.Windows.Forms.RadioButton();
			this.radXor = new System.Windows.Forms.RadioButton();
			this.radAnd = new System.Windows.Forms.RadioButton();
			this.cmbPolygonIndex = new System.Windows.Forms.ComboBox();
			this.btnClear = new System.Windows.Forms.Button();
			this.btnAddHole = new System.Windows.Forms.Button();
			this.btnDelHole = new System.Windows.Forms.Button();
			this.lblPos = new System.Windows.Forms.Label();
			this.cmbPol = new System.Windows.Forms.ComboBox();
			this.cmbHole = new System.Windows.Forms.ComboBox();
			this.btnDelPol = new System.Windows.Forms.Button();
			this.btnAddPol = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// radioButton1
			// 
			this.radioButton1.Appearance = System.Windows.Forms.Appearance.Button;
			this.radioButton1.AutoSize = true;
			this.radioButton1.Checked = true;
			this.radioButton1.Location = new System.Drawing.Point(12, 12);
			this.radioButton1.Name = "radioButton1";
			this.radioButton1.Size = new System.Drawing.Size(58, 22);
			this.radioButton1.TabIndex = 1;
			this.radioButton1.TabStop = true;
			this.radioButton1.Text = "ポリゴン１";
			this.radioButton1.UseVisualStyleBackColor = true;
			this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
			// 
			// radioButton2
			// 
			this.radioButton2.Appearance = System.Windows.Forms.Appearance.Button;
			this.radioButton2.AutoSize = true;
			this.radioButton2.Location = new System.Drawing.Point(76, 12);
			this.radioButton2.Name = "radioButton2";
			this.radioButton2.Size = new System.Drawing.Size(58, 22);
			this.radioButton2.TabIndex = 2;
			this.radioButton2.Text = "ポリゴン２";
			this.radioButton2.UseVisualStyleBackColor = true;
			this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
			// 
			// lblResult
			// 
			this.lblResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lblResult.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblResult.Location = new System.Drawing.Point(562, 17);
			this.lblResult.Name = "lblResult";
			this.lblResult.Size = new System.Drawing.Size(311, 14);
			this.lblResult.TabIndex = 4;
			this.lblResult.Text = "label3";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(10, 85);
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
			this.groupBox1.Controls.Add(this.radExtract);
			this.groupBox1.Controls.Add(this.radXor);
			this.groupBox1.Controls.Add(this.radAnd);
			this.groupBox1.Controls.Add(this.radOr);
			this.groupBox1.Controls.Add(this.radSub);
			this.groupBox1.Location = new System.Drawing.Point(12, 40);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(338, 42);
			this.groupBox1.TabIndex = 9;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "演算方法";
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
			this.cmbPolygonIndex.Location = new System.Drawing.Point(356, 54);
			this.cmbPolygonIndex.Name = "cmbPolygonIndex";
			this.cmbPolygonIndex.Size = new System.Drawing.Size(121, 20);
			this.cmbPolygonIndex.TabIndex = 10;
			this.cmbPolygonIndex.SelectedIndexChanged += new System.EventHandler(this.cmbPolygonIndex_SelectedIndexChanged);
			// 
			// btnClear
			// 
			this.btnClear.Location = new System.Drawing.Point(140, 12);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(75, 23);
			this.btnClear.TabIndex = 11;
			this.btnClear.Text = "クリア";
			this.btnClear.UseVisualStyleBackColor = true;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// btnAddHole
			// 
			this.btnAddHole.Location = new System.Drawing.Point(345, 12);
			this.btnAddHole.Name = "btnAddHole";
			this.btnAddHole.Size = new System.Drawing.Size(56, 23);
			this.btnAddHole.TabIndex = 12;
			this.btnAddHole.Text = "穴追加";
			this.btnAddHole.UseVisualStyleBackColor = true;
			this.btnAddHole.Click += new System.EventHandler(this.btnAddHole_Click);
			// 
			// btnDelHole
			// 
			this.btnDelHole.Location = new System.Drawing.Point(407, 12);
			this.btnDelHole.Name = "btnDelHole";
			this.btnDelHole.Size = new System.Drawing.Size(56, 23);
			this.btnDelHole.TabIndex = 13;
			this.btnDelHole.Text = "穴削除";
			this.btnDelHole.UseVisualStyleBackColor = true;
			this.btnDelHole.Click += new System.EventHandler(this.btnDelHole_Click);
			// 
			// lblPos
			// 
			this.lblPos.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblPos.Location = new System.Drawing.Point(483, 54);
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
			this.cmbPol.Location = new System.Drawing.Point(604, 54);
			this.cmbPol.Name = "cmbPol";
			this.cmbPol.Size = new System.Drawing.Size(59, 20);
			this.cmbPol.TabIndex = 15;
			this.cmbPol.SelectedIndexChanged += new System.EventHandler(this.cmbPol_SelectedIndexChanged);
			// 
			// cmbHole
			// 
			this.cmbHole.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbHole.FormattingEnabled = true;
			this.cmbHole.Location = new System.Drawing.Point(669, 54);
			this.cmbHole.Name = "cmbHole";
			this.cmbHole.Size = new System.Drawing.Size(59, 20);
			this.cmbHole.TabIndex = 16;
			this.cmbHole.SelectedIndexChanged += new System.EventHandler(this.cmbHole_SelectedIndexChanged);
			// 
			// btnDelPol
			// 
			this.btnDelPol.Location = new System.Drawing.Point(283, 12);
			this.btnDelPol.Name = "btnDelPol";
			this.btnDelPol.Size = new System.Drawing.Size(56, 23);
			this.btnDelPol.TabIndex = 18;
			this.btnDelPol.Text = "ポリ削除削除";
			this.btnDelPol.UseVisualStyleBackColor = true;
			this.btnDelPol.Click += new System.EventHandler(this.btnDelPol_Click);
			// 
			// btnAddPol
			// 
			this.btnAddPol.Location = new System.Drawing.Point(221, 12);
			this.btnAddPol.Name = "btnAddPol";
			this.btnAddPol.Size = new System.Drawing.Size(56, 23);
			this.btnAddPol.TabIndex = 17;
			this.btnAddPol.Text = "ポリ追加";
			this.btnAddPol.UseVisualStyleBackColor = true;
			this.btnAddPol.Click += new System.EventHandler(this.btnAddPol_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(885, 614);
			this.Controls.Add(this.btnDelPol);
			this.Controls.Add(this.btnAddPol);
			this.Controls.Add(this.cmbHole);
			this.Controls.Add(this.cmbPol);
			this.Controls.Add(this.lblPos);
			this.Controls.Add(this.btnDelHole);
			this.Controls.Add(this.btnAddHole);
			this.Controls.Add(this.btnClear);
			this.Controls.Add(this.cmbPolygonIndex);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.lblResult);
			this.Controls.Add(this.radioButton2);
			this.Controls.Add(this.radioButton1);
			this.Name = "Form1";
			this.Text = "Form1";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion
		private System.Windows.Forms.RadioButton radioButton1;
		private System.Windows.Forms.RadioButton radioButton2;
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
		private System.Windows.Forms.Button btnAddHole;
		private System.Windows.Forms.Button btnDelHole;
		private System.Windows.Forms.Label lblPos;
		private System.Windows.Forms.ComboBox cmbPol;
		private System.Windows.Forms.ComboBox cmbHole;
		private System.Windows.Forms.Button btnDelPol;
		private System.Windows.Forms.Button btnAddPol;
	}
}

