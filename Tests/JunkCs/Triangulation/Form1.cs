using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Jk;

namespace Triangulation {
	public partial class Form1 : Form {
		List<PointF> _Lines = new List<PointF>();
		List<PointF> _Polygon = null;
		List<int> _Triangles = null;

		public Form1() {
			InitializeComponent();
		}

		protected override void OnMouseDown(MouseEventArgs e) {
			if (e.Button == MouseButtons.Left) {
				_Polygon = null;
				_Triangles = null;
				this._Lines.Add(new PointF((float)e.X, (float)e.Y));
				Invalidate();
			}
		}

		protected override void OnPaint(PaintEventArgs e) {
			Graphics g = e.Graphics;
			using (Pen pen1 = new Pen(Color.FromArgb(0, 0, 0), 1f))
			using (Pen pen2 = new Pen(Color.FromArgb(255, 0, 0), 1f))
			using (Pen penTri = new Pen(Color.FromArgb(0, 0, 255), 1f))
			using (var brsTri = new SolidBrush(Color.FromArgb(127, 127, 255))) {
				if (this._Polygon != null) {
					PointF[] a = this._Polygon.ToArray();
					g.DrawPolygon(pen1, a);
					for (int i = 0; i < this._Triangles.Count; i += 3) {
						PointF[] tri = new PointF[] {
							a[this._Triangles[i]],
							a[this._Triangles[i + 1]],
							a[this._Triangles[i + 2]]
						};
						g.FillPolygon(brsTri, tri);
						g.DrawPolygon(penTri, tri);
					}
					foreach(var p in _Polygon) { 
						g.DrawRectangle(pen2, new Rectangle((int)((double)p.X - 2.0), (int)((double)p.Y - 2.0), 4, 4));
					}
				} else {
					if (2 <= this._Lines.Count) {
						g.DrawLines(pen1, _Lines.ToArray());
					}
					foreach(var p in _Lines) { 
						g.DrawRectangle(pen2, new Rectangle((int)((double)p.X - 2.0), (int)((double)p.Y - 2.0), 4, 4));
					}
				}
			}
		}

		private void button1_Click(object sender, EventArgs e) {
			if (_Lines.Count < 3)
				return;

			_Triangles = TriangulationF.Do((from p in _Lines select new Vector2f(p.X, p.Y)).ToArray());
			_Polygon = _Lines;
			_Lines = new List<PointF>();
			Invalidate();
		}
	}
}
