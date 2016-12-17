namespace PolygonBoolean {
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
			this.radAnd = new System.Windows.Forms.RadioButton();
			this.radXor = new System.Windows.Forms.RadioButton();
			this.cmbPolygonIndex = new System.Windows.Forms.ComboBox();
			this.radExtract = new System.Windows.Forms.RadioButton();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// radioButton1
			// 
			this.radioButton1.Appearance = System.Windows.Forms.Appearance.Button;
			this.radioButton1.AutoSize = true;
			this.radioButton1.Location = new System.Drawing.Point(12, 12);
			this.radioButton1.Name = "radioButton1";
			this.radioButton1.Size = new System.Drawing.Size(58, 22);
			this.radioButton1.TabIndex = 1;
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
			this.lblResult.AutoSize = true;
			this.lblResult.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblResult.Location = new System.Drawing.Point(140, 17);
			this.lblResult.Name = "lblResult";
			this.lblResult.Size = new System.Drawing.Size(37, 14);
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
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(885, 614);
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
	}
}

