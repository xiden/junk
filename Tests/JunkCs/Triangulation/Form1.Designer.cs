namespace Triangulation {
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
			this.btnClear = new System.Windows.Forms.Button();
			this.lblError = new System.Windows.Forms.Label();
			this.btnAddHole = new System.Windows.Forms.Button();
			this.btnDelHole = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// btnClear
			// 
			this.btnClear.Location = new System.Drawing.Point(12, 12);
			this.btnClear.Name = "btnClear";
			this.btnClear.Size = new System.Drawing.Size(75, 23);
			this.btnClear.TabIndex = 0;
			this.btnClear.Text = "Clear";
			this.btnClear.UseVisualStyleBackColor = true;
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			// 
			// lblError
			// 
			this.lblError.AutoSize = true;
			this.lblError.Location = new System.Drawing.Point(12, 38);
			this.lblError.Name = "lblError";
			this.lblError.Size = new System.Drawing.Size(35, 12);
			this.lblError.TabIndex = 1;
			this.lblError.Text = "label1";
			// 
			// btnAddHole
			// 
			this.btnAddHole.Location = new System.Drawing.Point(93, 12);
			this.btnAddHole.Name = "btnAddHole";
			this.btnAddHole.Size = new System.Drawing.Size(75, 23);
			this.btnAddHole.TabIndex = 2;
			this.btnAddHole.Text = "Add hole";
			this.btnAddHole.UseVisualStyleBackColor = true;
			this.btnAddHole.Click += new System.EventHandler(this.btnAddHole_Click);
			// 
			// btnDelHole
			// 
			this.btnDelHole.Location = new System.Drawing.Point(174, 12);
			this.btnDelHole.Name = "btnDelHole";
			this.btnDelHole.Size = new System.Drawing.Size(75, 23);
			this.btnDelHole.TabIndex = 3;
			this.btnDelHole.Text = "Del hole";
			this.btnDelHole.UseVisualStyleBackColor = true;
			this.btnDelHole.Click += new System.EventHandler(this.btnDelHole_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(687, 455);
			this.Controls.Add(this.btnDelHole);
			this.Controls.Add(this.btnAddHole);
			this.Controls.Add(this.lblError);
			this.Controls.Add(this.btnClear);
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button btnClear;
		private System.Windows.Forms.Label lblError;
		private System.Windows.Forms.Button btnAddHole;
		private System.Windows.Forms.Button btnDelHole;
	}
}

